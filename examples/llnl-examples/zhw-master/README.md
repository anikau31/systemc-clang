# ZHW - ZFP Hardware Implementation
ZFP is a floating-point compression format gaining traction in high-performance computing applications. A software implementation has demonstrated the ability to reduce data movement across communication channels and to reduce the footprint of floating-point arrays in memory. Nevertheless, any benefit in performance is limited to the spare compute cycles available before reaching bandwidth limitations. A hardware implementation of ZFP has the potential to substantially improve HPC application performance. Hardware compression speedup over software alone on a single CPU core ranges from 15x for 1-D arrays to over 200x for 3-D arrays.

The hardware implementation of ZFP is sourced in SystemC to facilitate its evaluation in various architectures. The encode pipeline consists of several modules. Uncompressed blocks of floating-point numbers in IEEE format flow into the pipeline in a stream and a compressed bitstream flows out from the pipeline. The decode pipeline is the inverse of the encode pipeline. A modular design enables number formats other than IEEE (such as posits) to be considered with minor adaptations. Best performance will be realized from the hardware ZFP unit when batches of blocks are processed at a time. The implementation supports 1-D, 2-D and 3-D blocks of floating-point numbers. C++ template parameters are used to specify the bit width of floating-point numbers and the array dimension of the encoder.

A test bench program has been created with several test cases, some with continuous data that is ideal for ZFP, and others with extreme cases containing numbers near or at the maximum or minimum values supported by the number format. The software implementation of ZFP {2] has been used to validate the hardware implementation of ZFP.

### Getting Started
- Install the SystemC library
- Clone or copy the zhw repository to your system
- Type "make help" from the top directory for available build and execution options

Environment variables:

- SCDIR:  SystemC source directory
- ZFPDIR: ZFP (in software) source directory

### References
1. Peter Lindstrom, "Fixed-Rate Compressed Floating-Point Arrays," IEEE Transactions on Visualization and Computer Graphics, 20(12):2674-2683, December 2014. [doi:10.1109/TVCG.2014.2346458](http://doi.org/10.1109/TVCG.2014.2346458).

2. ZFP on GitHub, [http://github.com/LLNL/zfp](http://github.com/LLNL/zfp)

### Dependencies
The open source SystemC library is needed for simulation of ZHW.
[http://workspace.accellera.org/downloads/standards/systemc](http://workspace.accellera.org/downloads/standards/systemc)

Install the software version of ZFP if validation of the hardware implementation is desired. Validation of ZHW output is enabled with a compile option to the test program.
[http://github.com/LLNL/zfp](http://github.com/LLNL/zfp)

### Contributing
Bug fixes, new features, applications, or hardware modules can be submitted as [pull requests](http://help.github.com/articles/using-pull-requests/).
All new contributions must be open source and not contain any proprietary code.

### License
ZHW is distributed under the terms of the BSD 3-Clause license.
See LICENSE and NOTICE for details.
SPDX-License-Identifier: BSD-3-Clause

LLNL-CODE-811758
