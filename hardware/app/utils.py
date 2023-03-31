import logging
import numpy as np
from pynq import Overlay, allocate, Device
# from pynq.pl_server.device import _preload_binfile

# static_bitstream = 'DS_top_hash.bit'

def load_memory(filename, bytes_per_line, base=16):
    byte_array = []
    with open(filename, 'r') as f:
        int_arr = [ int(f.strip(), base) for f in f.readlines()]
        for word in int_arr:
            for i in range(bytes_per_line):
                byte_array.append(word & 0xff)
                word = word >> 8
    byte_array = np.asarray(byte_array, dtype=np.uint8)
    res = allocate(shape=byte_array.shape, dtype=np.uint8)
    for i in range(byte_array.shape[0]):
        res[i] = byte_array[i]
    return res

# def pr_download(self, bitstream):
#     """Download partial bitstream
# 
#     Note that this does not provide any checks and simply
#     push the bitstream (converted to .bin by _preload_binfile) over the ICAP interface maintained by the PYNQ
#     library
#     
#     """
#     logging.debug('downloading partial bitstream: {}'.format(bitstream.bitfile_name))
#     if not bitstream.binfile_name:
#         _preload_binfile(bitstream)
# 
#     assert bitstream.partial
#     flag = '1'
#     with open(self.BS_FPGA_MAN_FLAGS, 'w') as fd:
#         fd.write(flag)
#     with open(self.BS_FPGA_MAN, 'w') as fd:
#         fd.write(bitstream.binfile_name)
