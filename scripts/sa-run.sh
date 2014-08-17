#!/bin/bash

if [ -z "$1" ]; then
    echo Error: You must specify a filename;
    exit;
fi
if [ -z "$SYSTEMC_CLANG_BUILD_DIR" ]; then 
    echo Error: Environment variable SYSTEMC_CLANG_BUILD_DIR is not set or empty; 
    exit;
fi

if [ -z "$LLVM_BUILD_DIR" ]; then 
    echo Error: Environment variable LLVM_BUILD_DIR is not set or empty; 
    exit;
fi


if [ -z "$SYSTEMC" ]; then 
    echo Error: Environment variable SYSTEMC is not set or empty; 
    exit;
fi

# copy the binary systemc-clang to LLVM_BUILD_DIR
cp $SYSTEMC_CLANG_BUILD_DIR/systemc-clang $LLVM_BUILD_DIR/bin/systemc-clang
$LLVM_BUILD_DIR/bin/systemc-clang $1 -- -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS  -I/usr/include -I$SYSTEMC/include -x c++ -w -c 
#-include $SYSTEMC_CLANG_BUILD_DIR/systemc.h.pch 
#$LLVM_BUILD_DIR/bin/systemc-clang -include $SYSTEMC_CLANG_BUILD_DIR/systemc.h.pch $1  -x c++
#clang -include test.h test.c -o test
