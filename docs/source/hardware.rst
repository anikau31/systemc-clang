.. highlight:: console

=============
Hardware Flow
=============

With `systemc-clang` compiled successfully, it is possible to generate SystemVerilog files and synthesize the design with CAD tools.
We provide examples and codes in which the SystemC design is converted to bitstreams using Xilinx Vivado and tested on board.

Preparation
-----------

``systemc-clang`` setup
^^^^^^^^^^^^^^^^^^^^^^^

To enable the targets for hardware flow, specify ``-DENABLE_HARDWARE_FLOW=ON`` when compiling ``systemc-clang``.
::
  $ cmake ../systemc-clang -DHDL=ON -DENABLE_HARDWARE_FLOW=ON
  
**After** the ``systemc-clang`` is successfully compiled, it is possible to proceed with the hardware flow.

Vivado setup
^^^^^^^^^^^^

We use Vivado 2020.1 to generate bitstreams and it is necessary to ensure that the Vivado executable is in the ``$PATH`` environment variable.
::
  $ vivado -version

  Vivado v2020.1 (64-bit)
  SW Build 2902540 on Wed May 27 19:54:35 MDT 2020
  IP Build 2902112 on Wed May 27 22:43:36 MDT 2020
  Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.

PYNQ setup
^^^^^^^^^^

Make sure that the PYNQ board is running **PYNQ v2.6.0** image.
The PYNQ images can be found at the `this page <https://github.com/Xilinx/PYNQ/releases>`_.


Compiling
---------

We can generate the bitstream for ``hash`` module from ``examples/llnl-examples/modules/hash-unit/hash-unit.cpp``.
::
  $ make DS_top_hash_bitstream


A ``zhw_encode`` module with ``DIMS=2`` is instantiated in ``examples/llnl-examples/zfpsynth/zfp3/z3test.cpp``.
To generate the bitstream for the module, one can build the ``DS_top_z3_bitstream`` target.
::
  $ make DS_top_z3_bitstream

The commands will generate necessary IP blocks, block design as well as the static design accordingly in the ``hardware`` directory.
Most importantly, the bitstream as well as various design check points are generated in the ``hardware/rtl/DS_top_hash`` and ``hardware/rtl/DS_top_z3/`` directory. Here we only list the bitstream files and the partial bitstream files that are most relevant.
::
  $ ls hardware/rtl/DS_top_z3/

  ...
  DS_top_z3.bit
  DS_top_z3_inst_rp_partial.bit

  $ ls hardware/rtl/DS_top_hash/

  ...
  DS_top_hash.bit
  DS_top_hash_inst_rp_partial.bit


Testing
-------

To test the generated bitstreams on board, one can copy the generated bitstream and the hardware handoff file to the PYNQ board, for example, with the ``scp`` command, where the ``systemc-clang`` is an arbitrary directory on the PYNQ board.
::
  $ scp hardware/rtl/DS_top_z3/{DS_top_z3,DS_top_z3_inst_rp_partial}.bit hardware/rtl/DS_top_hash/{DS_top_hash,DS_top_hash_inst_rp_partial}.bit xilinx@pynq:~/systemc-clang/ 
  $ scp hardware/bd/bd/.srcs/sources_1/bd/bd/hw_handoff/bd.hwh xilinx@pynq:~/systemc-clang/

Next, also copy the application python code to the PYNQ board.
::
  $ scp -r $SYSTEMC_CLANG/hardware/app/* xilinx@pynq:~/systemc-clang/


On the PYNQ board, change the working directory to ``~/systemc-clang`` and prepare the hardware handoff file with the following commands.
::
  $ cd ~/systemc-clang && cp bd.hwh DS_top_hash.hwh && cp bd.hwh DS_top_z3.hwh

Run the tests with the following commands. 
Note that the second command ``sudo python3 test_hash_mcdma.py`` loads a partial bitstream instead of the full bitstream.
::
  $ sudo python3 test_z3.py

  Static region is not loaded, downloading static design first
  Test passed

  $ sudo python3 test_hash_mcdma.py
  Test passed

