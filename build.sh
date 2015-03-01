#!/bin/bash

# The path where you have installed LLVM/CLANG.
# This is where make install put the executables.

export LLVM_BUILD_DIR=/home/jimmy/clang34

# The path where you are building systemc-clang
export SYSTEMC_CLANG_BUILD_DIR=/home/jimmy/Documents/SE499/systemc-clang-build
LLVMCOMPONENT=cppbackend
RTTIFLAG=-fno-rtti 
LLVMCONFIG=$LLVM_BUILD_DIR/bin/llvm-config

export LLVM_CXX_FLAGS=`$LLVMCONFIG --cxxflags`
export LLVM_CXX_FLAGS="$LLVM_CXX_FLAGS -fvisibility-inlines-hidden"
export LLVM_LIBS=`$LLVMCONFIG --libs`
export LLVM_LD_FLAGS=`$LLVMCONFIG --ldflags`
export LLVM_LD_FLAGS=`echo $LLVM_LD_FLAGS | sed 's/ *$//g'`

OLD_DIR=`pwd`

cd $SYSTEMC_CLANG_BUILD_DIR
if [ "$1" == "full-rebuild" ]; then
    cmake ../systemc-clang
    make clean
fi
make
cd $OLD_DIR
