import asyncio
import logging
from math import ceil

import numpy as np
from pynq import DefaultIP
from pynq import allocate
from pynq.buffer import PynqBuffer
from pynq.lib.dma import DMA_TYPE_TX, DMA_TYPE_RX, MAX_C_SG_LENGTH_WIDTH

def _tx_rx_to_str(v: int):
    if v == DMA_TYPE_TX:
        return 'DMA_TYPE_TX'
    elif v == DMA_TYPE_RX:
        return 'DMA_TYPE_RX'
    else:
        raise ValueError('Invalid tx_rx value')

def _split64b(value: int):
    """split a 64-bit value into 32-bit high and 32-bit low

    :return: msb, lsb
    """
    return (value >> 32) & 0xffffffff, value & 0xffffffff


def _roundup(value: int, alignment: int):
    """round up to nearest alignment"""
    return (value - 1) // alignment * alignment + alignment


class MCDMADescriptor:
    ALIGNMENT = 64
    def __init__(self, next_descriptor,
                 buffer_address,
                 control,
                 control_sideband,
                 app0, app1, app2, app3, app4, tx_rx):
        self._next_descriptor_msb, self._next_descriptor = _split64b(next_descriptor)  # 0x04, 0x00
        self._buffer_address_msb, self._buffer_address = _split64b(buffer_address)  # 0x0C, 0x08
        self._control = control  # 0x14
        self._control_sideband = control_sideband  # 0x18
        # self._status = status  # 0x1c
        self._app0 = app0  # 0x20
        self._app1 = app1  # 0x24
        self._app2 = app2  # 0x28
        self._app3 = app3  # 0x2C
        self._app4 = app4  # 0x30
        self._tx_rx = tx_rx


    @staticmethod
    def descriptor_length():
        return 13

    @staticmethod
    def descriptor_size(aligned=True):
        if not aligned:
            return MCDMADescriptor.descriptor_length() * 4  # 4-byte word per descriptor
        else:
            return _roundup(MCDMADescriptor.descriptor_length() * 4, MCDMADescriptor.ALIGNMENT)

    @staticmethod
    def create_tx_control(tx_sof: bool, tx_eof: bool, buffer_length: int):
        tx_sof = 1 if tx_sof else 0
        tx_eof = 1 if tx_eof else 0
        return (tx_sof << 31) | (tx_eof << 30) | (buffer_length & 0x3ffffff)

    @staticmethod
    def create_tx_control_sideband(tid: int, tuser: int):
        return (tid & 0xff) | (tuser & 0xffff)

    @staticmethod
    def create_rx_control(buffer_length: int):
        return buffer_length & 0x3ffffff

    def dump_to_buffer(self, buf: PynqBuffer):
        """dump current descriptor into a buffer, the buffer should be 64-byte aligned

        :param buffer: a contiguous array allocated by the PYNQ library
        :type buffer: PynqBuffer
        """
        if self._tx_rx == DMA_TYPE_TX:
            self._dump_to_buffer_tx(buf)
        elif self._tx_rx == DMA_TYPE_RX:
            self._dump_to_buffer_rx(buf)

    def _dump_to_buffer_tx(self, buf: PynqBuffer):
        """dump the TX(MM2S) descriptor into a buffer"""
        buf = buf.view(dtype=np.uint32).reshape(-1)
        buf[0] = self._next_descriptor
        buf[1] = self._next_descriptor_msb
        buf[2] = self._buffer_address
        buf[3] = self._buffer_address_msb
        # buffer[4]  # RSVD
        buf[5] = self._control
        buf[6] = self._control_sideband
        buf[7] = 0  # status
        buf[8] = self._app0
        buf[9] = self._app1
        buf[10] = self._app2
        buf[11] = self._app3
        buf[12] = self._app4

    def _dump_to_buffer_rx(self, buf):
        """dump the RX(S2MM) descriptor into a buffer"""
        buf = buf.view(dtype=np.uint32).reshape(-1)
        buf[0] = self._next_descriptor
        buf[1] = self._next_descriptor_msb
        buf[2] = self._buffer_address
        buf[3] = self._buffer_address_msb
        # buffer[4]  # RSVD
        buf[5] = self._control
        buf[6] = 0  # status
        buf[7] = 0  # sideband status
        buf[8] = self._app0
        buf[9] = self._app1
        buf[10] = self._app2
        buf[11] = self._app3
        buf[12] = self._app4


class _SGDMAChannel:
    """Drives one channel of streams of the DMA engine"""

    def __init__(self, channel_id, mmio, max_size, width, tx_rx, dre, interrupt=None, sg_addr_width=64):
        """
        Parameters
        ----------
        channel_id : Int
            The channel id of this channel, each channel correspond to a different tdest in
            the outputting stream.
            channel_id starts from 0
        mmio : MMIO
            The MMIO controller used for MCDMA IP.
        max_size : int
            Max size of the DMA buffer. Exceeding this will hang the system.
        width : int
            Number of bytes for each data.
        tx_rx : int
            Set to DMA_TYPE_TX(1) for sending or DMA_TYPE_RX(0) for receiving.
        dre : bool
            Data alignment enable.
        interrupt : Interrupt
            Interrupt used by the DMA channel, not implemented
        sg_addr_width: int
            The address width of scatter gather
        """
        self._mmio = mmio
        self._channel_id = channel_id
        self._interrupt = interrupt
        if self._interrupt is not None:
            raise NotImplementedError("Interrupt is not supported")
        # self._interrupt = interrupt
        self._max_size = max_size
        self._active_buffer = None
        self._align = width
        self._tx_rx = tx_rx
        assert dre, 'MCDMA must be configured with unaligned transfer support support'
        self._dre = dre
        self._sg_addr_width = sg_addr_width

        if tx_rx == DMA_TYPE_RX:
            self._offset = 0x500 + 0x40 * (self._channel_id + 1)
            self._flush_before = False
        else:  # DMA_TYPE_TX
            self._offset = 0x40 * (self._channel_id + 1)
            self._flush_before = True

        self.transferred = 0
        self._transfer_started = False
        self._descr = None
        self._num_descr = 0
        self._tail_addr = None

    @property
    def idle(self):
        return self.status & 0x01 == 1

    def start_with_tail_desc(self, tail_addr):
        """start DMA transfer on the channel"""
        # self.control = 0b11101001
        self.control = 0b00000001  # assuming no support for interrupts
        self.tail_descriptor = tail_addr
        self._transfer_started = True


    @property
    def blk_size(self):
        """maximal possible size per block of descriptor"""
        return self._max_size - (self._max_size % self._align)

    def _clear_interrupt(self):
        raise NotImplementedError()

    def _construct_descriptors(self, array, start, nbytes, app0, app1, app2, app3, app4, tid, tuser):
        """

        :param array:
        :param start:
        :param nbytes:
        :return: descriptor buffer, the address of the last descriptor, required by the ip
        """
        if nbytes == 0:
            nbytes = array.nbytes - start
        # check for alignment
        remain = nbytes
        blk_size = self.blk_size
        num_descr = (remain + blk_size - 1) // blk_size
        desc_size = MCDMADescriptor.descriptor_size()
        descr = allocate(shape=(num_descr, desc_size), dtype=np.uint8)
        base_addr = descr.physical_address
        tail_addr = descr.physical_address + (num_descr - 1) * desc_size

        self._num_descr = num_descr
        self._descr = descr

        logging.debug("descriptor base: {:x}".format(base_addr))
        logging.debug("# descriptor: {}".format(num_descr))
        logging.debug("desc_size: {}".format(desc_size))
        logging.debug("start: {}".format(start))
        logging.debug("nbytes: {}".format(nbytes))

        for i in range(self._num_descr):
            next_addr = base_addr + ((i + 1) % self._num_descr) * desc_size
            buffer_addr = array.physical_address + start  + (i * blk_size)
            start_of_frame = i == 0
            end_of_frame = i == self._num_descr - 1
            buffer_len = blk_size if remain > blk_size else remain
            remain -= buffer_len
            if self._tx_rx == DMA_TYPE_TX:
                control = MCDMADescriptor.create_tx_control(start_of_frame, end_of_frame, buffer_len)
                control_sideband = MCDMADescriptor.create_tx_control_sideband(tid, tuser)
            else:
                control = MCDMADescriptor.create_rx_control(buffer_len)
                control_sideband = 0

            desc = MCDMADescriptor(next_descriptor=next_addr, buffer_address=buffer_addr,
                                   control=control, control_sideband=control_sideband,
                                   app0=app0,
                                   app1=app1,
                                   app2=app2,
                                   app3=app3,
                                   app4=app4,
                                   tx_rx=self._tx_rx
                                   )
            desc.dump_to_buffer(descr.reshape(-1)[i * desc_size:(i + 1) * desc_size])
        return descr, tail_addr

    def start_transfer_phase_1(self, array: PynqBuffer, start=0, nbytes=0, app0=0, app1=0, app2=0, app3=0, app4=0, tid=0, tuser=0):
        """Start the transfer as a coroutine

        :param array: An contiguously allocated array to be transferred
        :type array: PynqBuffer
        :param start: Offset into array to start. Default is 0.
        :type start: int, optional
        :param nbytes: Number of bytes to transfer. Default is 0.
        :type nbytes: int, optional
        """
        if not self.idle:
            raise RuntimeError('MCDMA Channel {} not idle'.format(self._channel_id))

        logging.debug("Array to transfer: {:016x}".format(array.physical_address))

        self._transfer_started = False

        descr, tail_addr = self._construct_descriptors(array, start, nbytes, app0, app1, app2, app3, app4, tid, tuser)
        descr_addr = descr.physical_address
        if self._flush_before:
            array.flush()
        descr.flush()

        d = descr.view(dtype=np.uint32).reshape((-1, ))
        for i, a in enumerate(d):
            logging.debug('@{:08x}: {:08x}'.format(descr.physical_address + i * 4, a))

        self.current_descriptor = descr_addr
        self._tail_addr = tail_addr

    def start_transfer_phase_2(self):
        self.start_with_tail_desc(self._tail_addr)
        pass

    def wait(self):
        if not self._transfer_started:
            raise RuntimeError('Transfer not started')

        while not self.idle:
            pass

        self._transfer_started = False
        self._tail_addr = None

    @property
    def control(self):
        return self._mmio.read(self._offset + 0)

    @control.setter
    def control(self, value):
        self._mmio.write(self._offset + 0x0, value)

    @property
    def status(self):
        return self._mmio.read(self._offset + 0x4)

    @status.setter
    def status(self, value):
        self._mmio.write(self._offset + 0x4, value)

    @property
    def current_descriptor_lsb(self):
        return self._mmio.read(self._offset + 0x8)

    @current_descriptor_lsb.setter
    def current_descriptor_lsb(self, value):
        self._mmio.write(self._offset + 0x8, value)

    @property
    def current_descriptor_msb(self):
        return self._mmio.read(self._offset + 0xc)
    @current_descriptor_msb.setter
    def current_descriptor_msb(self, value):
        self._mmio.write(self._offset + 0xc, value)

    @property
    def current_descriptor(self):
        lsb = self.current_descriptor_lsb
        msb = self.current_descriptor_msb if self._sg_addr_width > 32 else 0
        return (msb << 32) | lsb

    @current_descriptor.setter
    def current_descriptor(self, value):
        lsb = value & 0xffffffff
        msb = (value >> 32) & 0xffffffff
        self.current_descriptor_lsb = lsb
        if self._sg_addr_width:
            self.current_descriptor_msb = msb

    @property
    def tail_descriptor_lsb(self):
        return self._mmio.read(self._offset + 0x10)

    @tail_descriptor_lsb.setter
    def tail_descriptor_lsb(self, value):
        self._mmio.write(self._offset + 0x10, value)

    @property
    def tail_descriptor_msb(self):
        return self._mmio.read(self._offset + 0x14)

    @tail_descriptor_msb.setter
    def tail_descriptor_msb(self, value):
        self._mmio.write(self._offset + 0x14, value)

    @property
    def tail_descriptor(self):
        lsb = self.tail_descriptor_lsb
        msb = self.tail_descriptor_msb if self._sg_addr_width > 32 else 0
        return (msb << 32) | lsb

    @tail_descriptor.setter
    def tail_descriptor(self, value):
        lsb = value & 0xffffffff
        msb = (value >> 32) & 0xffffffff
        self.tail_descriptor_lsb = lsb
        if self._sg_addr_width:
            self.tail_descriptor_msb = msb

    @property
    def packet_count_stat(self):
        if self._tx_rx == DMA_TYPE_TX:
            return self._mmio.read(self._offset + 0x18)
        else:  # self._tx_rx == DMA_TYPE_RX:
            return self._mmio.read(self._offset + 0x1C)

    @property
    def packet_drop_stat(self):
        if self._tx_rx == DMA_TYPE_RX:
            return self._mmio.read(self._offset + 0x18)
        else:
            raise RuntimeError('packet_drop_stat is only available for RX channel')


class ChannelTransferDescription:
    """describes the necessary information for transferring form one channel"""
    def __init__(self, channel_id, array, start=0, nbytes=0):
        """

        :param channel_id: the channel to deliver, id starts from 0
        :param array: A contiguous array for holding the data to transform
        :type array: PynqBuffer
        :param start: offset into array to start. Default is 0.
        :type start: int
        :param nbytes: Number of bytes to transfer. Default is 0, meaning transfer the whole array to the end
        :type nbytes: int
        """
        self._channel_id = channel_id
        self._array = array
        self._start = start
        self._nbytes = nbytes

    @property
    def channel_id(self):
        return self._channel_id

    @property
    def array(self):
        return self._array

    @property
    def start(self):
        return self._start

    @property
    def nbytes(self):
        return self._nbytes


class _SGDMAControl:
    def __init__(self, mmio, tx_rx, channels):
        self._mmio = mmio
        self._tx_rx = tx_rx
        if tx_rx == DMA_TYPE_TX:
            self._offset = 0
        else:
            self._offset = 0x500
        self._channels = channels

    @property
    def running(self):
        """True indicating the MCDMA is running
        """
        return self.status & 0b01 == 0

    @property
    def halted(self):
        """True indicating the MCDMA is stopped
        """
        return self.status & 0b01 == 1

    @property
    def idle(self):
        """True indicating the MCDMA is idle, all channels reach the tail
        pointers and all queued descriptors have been processed
        """
        return self.status & 0b10 != 0

    @property
    def has_error(self):
        return self.error & 0b1110111 != 0

    def reset(self):
        self.control = 0b100
        while self.control & 0b100 != 0:
            pass

    def start(self):
        """Start the (whole) MCDMA engine

        :return:
        """
        self.control = 0b1
        while not self.running:  # wait until DMA control channel is not halted
            pass

    def stop(self):
        """Stops the MCDMA engine and aborts current transfer

        :return:
        """
        self.control = 0
        while self.running:
            pass

    def start_transfer(self, description):
        """Transfer the channel

        :param description: a list of descriptors on how the transfer is conducted
        :type description: Iterable[ChannelTransferDescription]
        :return:
        """
        logging.debug('Starting transfer')
        if not self.idle and not self.halted:
            raise RuntimeError('MCDMA engine not halted')

        # self.start()
        # enable channels
        self.channel_enable = 0
        channel_enable = 0
        for desc in description:
            channel_id = desc.channel_id
            channel_enable |= (1 << channel_id)
        # 1. Enable the required channels
        self.channel_enable = channel_enable

        tasks = []

        # 2. Program Current Descriptor
        # 3. Program CHANNEL.Fetch
        for desc in description:
            channel_id = desc.channel_id
            con_array = desc.array
            start = desc.start 
            nbytes = desc.nbytes
            if len(self._channels) <= channel_id:
                raise ValueError('Transfer channel_id out of bound: {}, '.format(channel_id))
            # tasks.append(self._channels[channel_id].start_transfer_phase_1(con_array, start, nbytes))
            self._channels[channel_id].start_transfer_phase_1(con_array, start, nbytes)
        # await asyncio.gather(*tasks)

        # 4. MCDMA.RS
        self.start()

        # 5. Interrupt threshold, enalbe interrupts
        # 6. Queue Scheduler
        # 7. TD register
        tasks = []
        for desc in description:
            channel_id = desc.channel_id
            if len(self._channels) <= channel_id:
                raise ValueError('Transfer channel_id out of bound: {}, '.format(channel_id))
            # tasks.append(self._channels[channel_id].start_transfer_phase_2())
            self._channels[channel_id].start_transfer_phase_2()

        self._description = description


    def transfer(self, description, timeout=None):
        """a blocking version of transfering the data"""
        self.start_transfer(description)

    def wait(self):
        logging.debug('Waiting for {} channels ending'.format(_tx_rx_to_str(self._tx_rx)))
        while not self.idle:
            # TODO: Add error handling
            # if self.error & 0b1110111 != 0:
            #     logging.debug('Error: {:b}'.format(self.error))
            #     break
            pass
        for desc in self._description:
            channel_id = desc.channel_id
            logging.debug('Channel {} Status: {:b}'.format(channel_id, self._channels[channel_id].status))
            self._channels[channel_id].status = 0
            while not self._channels[channel_id].idle:
                pass
            bd = self._channels[channel_id]._descr.view(np.uint32).reshape(-1)
            st = bd[7] if self._tx_rx == DMA_TYPE_TX else bd[6]
            logging.debug('Status (BD): {:b}'.format(st))
            logging.debug('Channel {} done'.format(channel_id))
        logging.debug('DMA Engine: error {:b}'.format(self.error))
        # while not self.halted:
        #     pass
        # self.stop()
        logging.debug('DMA Engine status: {:b}'.format(self.status))

    @property
    def control(self):
        """Control register"""
        return self._mmio.read(self._offset + 0)
    @control.setter
    def control(self, value):
        self._mmio.write(self._offset + 0, value)

    @property
    def status(self):
        return self._mmio.read(self._offset + 0x4)

    @property
    def channel_enable(self):
        return self._mmio.read(self._offset + 0x8)
    @channel_enable.setter
    def channel_enable(self, value):
        self._mmio.write(self._offset + 0x8, value)

    @property
    def channel_in_progress(self):
        return self._mmio.read(self._offset + 0xC)

    @property
    def error(self):
        return self._mmio.read(self._offset + 0x10)
    @error.setter
    def error(self, value):
        self._mmio.write(self._offset + 0x10, value)

    @property
    def channel_schedule_type(self):
        if not self.tx_rx != DMA_TYPE_TX:
            raise ValueError('channel_schedule_type only available for TX channel')
        return self._mmio.read(self._offset + 0x14)
    @channel_schedule_type.setter
    def channel_schedule_type(self, value):
        if not self.tx_rx != DMA_TYPE_TX:
            raise ValueError('channel_schedule_type only available for TX channel')
        self._mmio.write(self._offset + 0x14, value)

    @property
    def packet_drop(self):
        if not self.tx_rx != DMA_TYPE_RX:
            raise ValueError('packet_drop only available for RX channel')
        return self._mmio.read(self._offset + 14)
    @packet_drop.setter
    def packet_drop(self, value):
        if not self.tx_rx != DMA_TYPE_RX:
            raise ValueError('packet_drop only available for RX channel')
        self._mmio.write(self._offset + 14, value)

    @property
    def weighted_round_robin_1(self):
        if not self.tx_rx != DMA_TYPE_TX:
            raise ValueError('weighted_round_robin_1 only available for TX channel')
        return self._mmio.read(self._offset + 0x18)
    @weighted_round_robin_1.setter
    def weighted_round_robin_1(self, value):
        if not self.tx_rx != DMA_TYPE_TX:
            raise ValueError('weighted_round_robin_1 only available for TX channel')
        self._mmio.write(self._offset + 0x18, value)

    @property
    def weighted_round_robin_2(self):
        if not self.tx_rx != DMA_TYPE_TX:
            raise ValueError('weighted_round_robin_2 only available for TX channel')
        return self._mmio.read(self._offset + 0x1C)
    @weighted_round_robin_2.setter
    def weighted_round_robin_2(self, value):
        if not self.tx_rx != DMA_TYPE_TX:
            raise ValueError('weighted_round_robin_2 only available for TX channel')
        self._mmio.write(self._offset + 0x1C, value)

    @property
    def serviced(self):
        if self.tx_rx == DMA_TYPE_TX:
            return self._mmio.read(self._offset + 0x20)
        else:  # self.tx_rx == DMA_TYPE_RX
            return self._mmio.read(self._offset + 0x18)

    @property
    def arcache_aruser(self):
        if not self.tx_rx != DMA_TYPE_TX:
            raise ValueError('arcache_aruser only available for TX channel')
        return self._mmio.read(self._offset + 0x24)
    @arcache_aruser.setter
    def arcache_aruser(self, value):
        if not self.tx_rx != DMA_TYPE_TX:
            raise ValueError('arcache_aruser only available for TX channel')
        self._mmio.write(self._offset + 0x24, value)

    @property
    def awcache_awuser(self):
        if not self.tx_rx != DMA_TYPE_RX:
            raise ValueError('packet_drop only available for RX channel')
        return self._mmio.read(self._offset + 0x1C)
    @awcache_awuser.setter
    def awcache_awuser(self, value):
        if not self.tx_rx != DMA_TYPE_RX:
            raise ValueError('packet_drop only available for RX channel')
        self._mmio.write(self._offset + 0x1C, value)

    @property
    def interrupt_status(self):
        if self.tx_rx == DMA_TYPE_TX:
            return self._mmio.read(self._offset + 0x28)
        else:  # self.tx_rx == DMA_TYPE_RX
            return self._mmio.read(self._offset + 0x20)

    @interrupt_status.setter
    def interrupt_status(self, value):
        if self.tx_rx == DMA_TYPE_TX:
            self._mmio.write(self._offset + 0x28, value)
        else:  # self.tx_rx == DMA_TYPE_RX
            self._mmio.write(self._offset + 0x20, value)


class MCDMADriver(DefaultIP):
    """A simplistic multi-channel DMA driver"""

    def __init__(self, description, *args, **kwargs):
        super().__init__(description=description)
        self.description = description

        if "parameters" not in description:
            raise RuntimeError(
                "Unable to get parameters from description; "
                "Users must use *.hwh files for overlays."
            )

        parameters = description["parameters"]

        # Parameter validation
        if "c_include_sg" in parameters:
            self._sg = bool(int(parameters["c_include_sg"]))
        else:
            raise NotImplementedError("Non-SG mode is not implemented")
            self._sg = False

        if "c_micro_dma" in parameters:
            raise NotImplementedError("Micro DMA mode is not supported")

        if self.description.get("interrupts"):
            raise NotImplementedError("Interrupts are not supported")

        if "c_sg_length_width" in parameters:
            self.buffer_max_size = 1 << int(parameters["c_sg_length_width"])
        else:
            self.buffer_max_size = 1 << MAX_C_SG_LENGTH_WIDTH

        self.sendcontrol = None
        self.recvcontrol = None
        self.sendchannels = []
        self.recvchannels = []

        self.set_up_tx_channels()
        self.set_up_rx_channels()

        self.set_up_tx_control()
        self.set_up_rx_control()

    @property
    def sendchannel(self):
        return self.sendcontrol

    @property
    def recvchannel(self):
        return self.recvcontrol

    def set_up_tx_control(self):
        self.sendcontrol = _SGDMAControl(self.mmio, DMA_TYPE_TX, self.sendchannels)

    def set_up_rx_control(self):
        self.recvcontrol = _SGDMAControl(self.mmio, DMA_TYPE_RX, self.recvchannels)

    def set_up_tx_channels(self):
        """Set up transmit channels."""
        parameters = self.description["parameters"]
        if bool(int(parameters.get("c_include_mm2s", 0))):
            dre = bool(int(parameters.get("c_include_mm2s_dre", 0)))
            data_width = int(parameters.get("c_m_axi_mm2s_data_width")) >> 3  # in bytes
            max_size = self.buffer_max_size  # potential support for micro DMA
            tx_channels_cnt = int(parameters.get("c_num_mm2s_channels"))
            for channel_idx in range(tx_channels_cnt):
                self.set_up_tx_channel(channel_idx, data_width, max_size, dre)
        else:  # TX channel not found
            pass

    def set_up_rx_channels(self):
        """Set up receive channels."""
        parameters = self.description["parameters"]
        if bool(int(parameters.get("c_include_s2mm", 0))):
            dre = bool(int(parameters.get("c_include_s2mm_dre", 0)))
            data_width = int(parameters.get("c_m_axi_s2mm_data_width")) >> 3  # in bytes
            max_size = self.buffer_max_size  # potential support for micro DMA
            rx_channels_cnt = int(parameters.get("c_num_s2mm_channels"))
            for channel_idx in range(rx_channels_cnt):
                self.set_up_rx_channel(channel_idx, data_width, max_size, dre)
        else:  # RX channel not found
            pass

        pass

    def set_up_tx_channel(self, channel_idx, data_width, max_size, dre):
        self.sendchannels.append(
            _SGDMAChannel(
                channel_idx, self.mmio, max_size, data_width, DMA_TYPE_TX, dre
            )
        )
        pass

    def set_up_rx_channel(self, channel_idx, data_width, max_size, dre):
        self.recvchannels.append(
            _SGDMAChannel(
                channel_idx, self.mmio, max_size, data_width, DMA_TYPE_RX, dre
            )
        )
        pass

    bindto = ["xilinx.com:ip:axi_mcdma:1.1"]
