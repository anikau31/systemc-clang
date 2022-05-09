#!/bin/bash

if [ -z "$1" ]; then
    echo Error: You must specify a filename;
    exit;
fi
if [ -z "$SYSTEMC_CLANG_BUILD_DIR" ]; then 
    echo Error: Environment variable SYSTEMC_CLANG_BUILD_DIR is not set or empty; 
    exit;
fi

if [ -z "$LLVM_INSTALL_DIR" ]; then 
    echo Error: Environment variable LLVM_INSTALL_DIR is not set or empty; 
    exit;
fi


if [ -z "$SYSTEMC" ]; then 
    echo Error: Environment variable SYSTEMC is not set or empty; 
    exit;
fi

if [ -z "$SYSTEMC_CLANG" ]; then 
    echo Error: Environment variable SYSTEMC_CLANG is not set or empty; 
    exit;
fi

python3 $SYSTEMC_CLANG/plugins/hdl/hcode2verilog.py $@
