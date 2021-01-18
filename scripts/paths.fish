#! /usr/bin/fish
###===================================================================
##
## It is important to use this script to set all the necessary paths.
## This is a fish shell script.
##===================================================================
# Path for where the binaries are for clang
# SET this.
set -x LLVM_INSTALL_DIR /home/$USER/bin/clang-11.0.0/

# Path where SystemC is installed
# SET this.
set -x SYSTEMC /home/$USER/code/systemc-2.3.3/systemc

# Path for the systemc-clang build directory
# SET this.
set -x SYSTEMC_CLANG_BUILD_DIR /home/$USER/code/github/systemc-clang-build/
##===================================================================

set -x LLVMCOMPONENT cppbackend
set -x RTTIFLAG -fno-rtti
set -x LLVMCONFIG $LLVM_INSTALL_DIR/bin/llvm-config

# New llvm/clang uses flags that are different than GNU gcc's
# Alternatively, you can use gcc and g++, but some flags don't work.
set -x CC $LLVM_INSTALL_DIR/bin/clang #clang
set -x CXX $LLVM_INSTALL_DIR/bin/clang++
set -x LLVM_CXX_FLAGS (eval $LLVMCONFIG --cxxflags)

# Generate all the flags.
# -fno-aligned-allocation needed for c++17
set -x LLVM_CXX_FLAGS "$LLVM_CXX_FLAGS -fvisibility-inlines-hidden -fno-aligned-allocation -Wsign-compare"
set -x LLVM_LIBS (eval $LLVMCONFIG --libs)
set -x LLVM_LD_FLAGS (eval $LLVMCONFIG --ldflags)
set -x LLVM_LD_FLAGS (echo $LLVM_LD_FLAGS | sed 's/ *$//g')

# DO NOT SET
set -x SYSTEMC_CLANG $SYSTEMC
