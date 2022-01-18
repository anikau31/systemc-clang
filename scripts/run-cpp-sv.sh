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

python3 $SYSTEMC_CLANG/plugins/hdl/systemc-clang.py $1 -- -x c++ -w -c -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DRVD -I$LLVM_INSTALL_DIR/lib/clang/13.0.0/include/ -I/usr/include -I$SYSTEMC/include
