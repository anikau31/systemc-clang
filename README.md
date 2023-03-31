# SCCL: FCCM AE 2023

SCCL: A SystemC translator to RTL
==========================================

(systemc-clang)[https://github.com/anikau31/systemc-clang] parses SystemC 2.3.3. It parses RTL constructs and some TLM 2.0 constructs. There is a HDL synthesis plugin that generates Verilog from SystemC RTL descriptions. 

Requirements
------------

*  `llvm/clang <https://releases.llvm.org/download.html>`_ (version 13.0.0)
*  `SystemC <http://systemc.org>`_ version 2.3.3. 
*  c++17 is required. 
*  docker 23.0.1

Docker use
-----------

- Clone the FCCM AE SCCL repository. TODO: Update the repository link.
```
git clone https://github.com/rseac/systemc-clang.git
```

- Change into the directory
```
cd systemc-clang;
```

- Ensure that docker is present. We use version 23.0.1.
```
docker --version
```

- Build the docker container.
```
docker build -t fccm-ae
```

- After building the docker image, go to the folder containing the systemc-clang, start the docker image with start.sh with the following command
```
cd ..
mkdir systemc-clang-build
./systemc-clang/start.sh fccm-ae fccm-ae-dev systemc-clang systemc-clang-build
```

- The following steps should be conducted in the container.

- Build systemc-clang within the docker image under `/systemc-clang-build` using `cmake` and `ninja`.
```
cmake ../systemc-clang -DHDL=ON -DENABLE_TESTS=ON -DENABLE_VERILOG_TESTS=ON -DCMAKE_BUILD_TYPE=Debug -DENABLE_HARDWARE_FLOW=ON -G Ninja
```

- Within the systemc-clang source directory, execute `step1.sh`. 
This command automatically runs systemc-clang and generates the IR and the SystemVerilog design files.
```
cd /systemc-clang/;
sh step1.sh;
```

- Leave the docker container and the generated files could be found in various directories in the SCCL folders, specifically in `systemc-clang/fccm-case-studies`.

- Go to the bitstream generation folder on the host, and modify `env.sh`, such that `ULTRA96_V2_SSH_NAME` points to the Ultra96V2 host name and `ULTRA96_V2_PLATFORM` points to the official Ultra96V2 platform file:
```
cd systemc-clang/fccm-case-studies/ultra96-vitis-ae/
cat env.sh

export ULTRA96_V2_PLATFORM=/opt/vitis_platforms/u96v2_sbc_base/u96v2_sbc_base.xpfm
export ULTRA96_V2_SSH_NAME=ultra96v2
```

- Next, source the env.sh
```
source env.sh
```

- Ensure that Vivado is 2022.1
```
vivado --version

Vivado v2022.1 (64-bit)
SW Build 3526262 on Mon Apr 18 15:47:01 MDT 2022
IP Build 3524634 on Mon Apr 18 20:55:01 MDT 2022
Tool Version Limit: 2022.04

v++ --version

****** v++ v2022.1 (64-bit)
  **** SW Build 3524075 on 2022-04-13-17:42:45
    ** Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
```

- Build the bitstreams/utilization
```
make bitstreams
make utils
```

- To build the bitstreams for the zhw encoder/decoder, go to the parent folder of the repository, and create another directory.
```
mkdir systemc-clang-build-zfp && cd systemc-clang-build-zfp
cmake ../systemc-clang -DHDL=ON -DENABLE_HARDWARE_FLOW=ON -DClang_DIR=/opt/clang-13/
make DS_top_z7_d64_bitstream DS_top_z7_d32_bitstream DS_top_z3_d64_bitstream DS_top_z3_d32_bitstream
```

- Go back to `ultra96-vitis-ae` folder and upload the bitstreams
```
cd systemc-clang/fccm-case-studies/ultra96-vitis-ae/
bash upload.sh
```
- We provide a notebook file for running the bitstreams at `systemc-clang/fccm-case-studies/ultra96-vitis-ae/FCCM-2023-AE.ipynb`
