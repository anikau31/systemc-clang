# The path where you have installed LLVM/CLANG.
# This is where make install put the executables.

export LLVM_BUILD_DIR=/home/twiga/clang-7.0.0/ 

# The path where you are building systemc-clang
export SYSTEMC=/home/twiga/sw/systemc-2.3.3/
export SYSTEMC_CLANG_BUILD_DIR=/home/twiga/code/systemc-clang-build/
LLVMCOMPONENT=cppbackend
RTTIFLAG=-fno-rtti 
LLVMCONFIG=$LLVM_BUILD_DIR/bin/llvm-config


#export LLVM_CXX_FLAGS=`$LLVMCONFIG --cxxflags`

# The flags have changed 
export LLVM_CXX_FLAGS='-I/home/twiga/$LLVM_BUILD_DIR/include -fPIC -fvisibility-inlines-hidden -Werror=date-time -std=c++11 -Wall -Wextra -Wno-unused-parameter -Wwrite-strings -Wcast-qual -Wmissing-field-initializers -pedantic -Wno-long-long -Wnon-virtual-dtor -Wdelete-non-virtual-dtor -ffunction-sections -fdata-sections -O3 -DNDEBUG  -fno-exceptions -fno-rtti -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS'

export LLVM_CXX_FLAGS="$LLVM_CXX_FLAGS -fvisibility-inlines-hidden"
export LLVM_LIBS=`$LLVMCONFIG --libs` 
export LLVM_LD_FLAGS=`$LLVMCONFIG --ldflags`
export LLVM_LD_FLAGS=`echo $LLVM_LD_FLAGS | sed 's/ *$//g'`
