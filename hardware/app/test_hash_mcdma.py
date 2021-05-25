# coding: utf-8
import logging
import numpy as np
from MCDMA import MCDMADriver, ChannelTransferDescription
from pynq import Overlay, allocate, Device, Bitstream
from utils import load_memory, pr_download, static_bitstream

# logging.basicConfig(level = logging.DEBUG)

partial_bitstream = 'DS_top_mcdma_static_inst_rp_partial.bit'
def main():
    logging.debug('hash block')
    # Loading data
    hdata = load_memory('data/hdata.mem', bytes_per_line=4)
    htaptohash = load_memory('data/htap.mem', bytes_per_line=4)
    htapfromshash_golden = load_memory('data/htapfromshash.mem', bytes_per_line=32)
    htapfromshash_design = allocate(shape=htapfromshash_golden.shape, dtype=htapfromshash_golden.dtype)

    # Loading bitstream
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

    # Construct data to send
    nflit = 107
    flit_per_transfer = 4
    channel0_flit = 17
    channel1_flit = 32


    # Currently, sending data flits that are too large hangs the MCDMA engine
    dma.sendchannel.reset()
    for i in range(0, nflit, flit_per_transfer):
        dma.sendchannel.reset()
        logging.debug('Main for-loop: {}'.format(i))

        nf = min(nflit - i, flit_per_transfer)

        send_description = [
            ChannelTransferDescription(channel_id=0, array=hdata, start=channel0_flit * i, nbytes=nf * channel0_flit),  # Sent over channel 0
            ChannelTransferDescription(channel_id=1, array=htaptohash, start=channel1_flit * i, nbytes=nf * channel1_flit),  # Sent over channel 1
        ]

        recv_description = [ChannelTransferDescription(channel_id=0, array=htapfromshash_design, start=i * channel1_flit, nbytes=nf * channel1_flit)]

        dma.recvchannel.transfer(recv_description)
        dma.sendchannel.transfer(send_description)

        dma.sendchannel.wait()
        dma.recvchannel.wait()

    logging.debug('Waiting to receive')
    dma.recvchannel.wait()
    htapfromshash_design.invalidate()
    for i in range(htapfromshash_design.view(np.uint64).shape[0]):
        assert htapfromshash_design.view(np.uint64)[i] == htapfromshash_golden.view(np.uint64)[i]

    print('Test passed')


if __name__ == '__main__':
    main()
