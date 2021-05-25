# coding: utf-8
import logging
import numpy as np
from MCDMA import MCDMADriver, ChannelTransferDescription
from pynq import Overlay, allocate, Device, Bitstream
from utils import load_memory, pr_download, static_bitstream

# logging.basicConfig(level = logging.DEBUG)

partial_bitstream = 'DS_top_z3_inst_rp_partial.bit'

def main():
    inp_data = load_memory('data/z3.mem', bytes_per_line=4) 
    out_data_golden = load_memory('data/z3_encoded.mem', bytes_per_line=8)
    out_data = allocate(shape=inp_data.shape, dtype=inp_data.dtype)

    # Loading partial bitstream
    dev = Device.active_device
    base = Overlay(static_bitstream, download=False)
    if not base.is_loaded():
        print('Static region is not loaded, downloading static design first')
        base.download()
    partial_bs = Bitstream(partial_bitstream, partial=True)
    pr_download(dev, partial_bs)

    dma = base.axi_mcdma_0
    gpio = base.axi_gpio_0  # gpio for reset

    gpio.channel1.write(0, mask=0xffffffff)  # reset active low
    gpio.channel1.write(1, mask=0xffffffff)  # deassert reset

    # Prepare for receiving
    logging.debug('Preparing for receving data')


    # Currently, sending data flits that are too large hangs the MCDMA engine
    dma.sendchannel.reset()
    dma.recvchannel.reset()
    send_description = [
        ChannelTransferDescription(channel_id=0, array=inp_data, nbytes=inp_data.size * inp_data.itemsize),  # Sent over channel 0
    ]

    recv_description = [
        ChannelTransferDescription(channel_id=0, array=out_data, nbytes=out_data.size * out_data.itemsize)
    ]

    dma.recvchannel.transfer(recv_description)
    dma.sendchannel.transfer(send_description)

    logging.debug('Waiting to receive')
    dma.sendchannel.wait()
    dma.recvchannel.wait()
    # dma.recvchannel.wait()
    for i in range(1111):
        pass
    out_data.invalidate()
    for i in range(out_data.view(np.uint64).shape[0]):
        assert out_data_golden.view(np.uint64)[i] == out_data.view(np.uint64)[i]

    print('Test passed')


if __name__ == '__main__':
    main()
    pass
