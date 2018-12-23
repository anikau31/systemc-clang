##===================================================================
## 
## It is important to use this script to set all the necessary paths.
## 


# Path for where the binaries are for clang
# SET this.
export LLVM_BUILD_DIR=/home/$USER/bin/clang-7.0.0/ 

# Path where SystemC is installed
# SET this.
export SYSTEMC=/home/$USER/code/systemc-2.3.3/systemc/

# Path for the systemc-clang build directory
# SET this.
export SYSTEMC_CLANG_BUILD_DIR=/home/$USER/code/systemc-clang-build/
##===================================================================

LLVMCOMPONENT=cppbackend
RTTIFLAG=-fno-rtti 
LLVMCONFIG=$LLVM_BUILD_DIR/bin/llvm-config

# New llvm/clang uses flags that are different than GNU gcc's
# Alternatively, you can use gcc and g++, but some flags don't work.
export CC=clang
export CXX=clang++
export LLVM_CXX_FLAGS=`$LLVMCONFIG --cxxflags`

# Generate all the flags.
export LLVM_CXX_FLAGS="$LLVM_CXX_FLAGS -fvisibility-inlines-hidden"
export LLVM_LIBS=`$LLVMCONFIG --libs` 
export LLVM_LD_FLAGS=`$LLVMCONFIG --ldflags`
export LLVM_LD_FLAGS=`echo $LLVM_LD_FLAGS | sed 's/ *$//g'`
