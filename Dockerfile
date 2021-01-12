####################
# base image setup #
####################
# clang works with 20.04
FROM ubuntu:20.04

# https://github.com/moby/moby/issues/27988
# This is to prevent the keyboard interaction
# Another way is to install dialog apt-utils packages first, but I just use echo so that we install less packages
RUN echo 'debconf debconf/frontend select Noninteractive' | debconf-set-selections

# We first install necessary packages for file manipulation,
# this helps reduce turn around time for adding packages
RUN apt-get update && apt-get install -y -q --no-install-recommends \
      curl \
      xz-utils \
      ca-certificates

# Copy necessary files and install necessary dependencies in opt
WORKDIR /tmp

# Set up llvm/clang
# in /tmp now
ENV CLANG_VERSION=11.0.0
ENV TARGET_ARCH=linux64

# run systemc
RUN mkdir /opt/systemc-2.3.3 && curl -L https://github.com/rseac/systemc-travisci-cache/raw/master/systemc-2.3.3.tar.gz \
      | tar -xzC /tmp/ \
      && mv systemc-2.3.3/systemc/* /opt/systemc-2.3.3 \
      && rm -rf systemc-2.3.3 \
      && mkdir /opt/clang-$CLANG_VERSION && curl -L https://github.com/llvm/llvm-project/releases/download/llvmorg-11.0.0/clang+llvm-11.0.0-x86_64-linux-gnu-ubuntu-20.04.tar.xz \
      |  tar -xJC /tmp/  \
      && mv clang+llvm-11.0.0-x86_64-linux-gnu-ubuntu-20.04/* /opt/clang-$CLANG_VERSION/ \
      && rm -rf /tmp/*

# Install other requirements
RUN apt-get update && apt-get install -y -q --no-install-recommends \
#g++ \
  cmake \
#ccache \
  ninja-build \
  python3 \
  python3-pip 
#git \
#  libz-dev \
#  libncurses-dev


# Set up environment variables across images
ENV LLVM_INSTALL_DIR=/opt/clang-$CLANG_VERSION
ENV SYSTEMC=/opt/systemc-2.3.3
ENV CXX=$LLVM_INSTALL_DIR/bin/clang++
#g++
ENV CC=$LLVM_INSTALL_DIR/bin/clang
#gcc

ENV LLVMCONFIG=$LLVM_INSTALL_DIR/bin/llvm-config
ENV LLVM_LD_FLAGS=-L$LLVM_INSTALL_DIR/lib
ENV LLVM_CXX_FLAGS="-I/home/travis/build/rseac/systemc-clang/DEPENDS/clang-$CLANG_VERSION/include  -fPIC -fvisibility-inlines-hidden -std=c++14 -Wall -Wextra -Wno-unused-parameter -Wwrite-strings -Wcast-qual -Wmissing-field-initializers -pedantic -Wno-long-long -Wnon-virtual-dtor -Wdelete-non-virtual-dtor -ffunction-sections -fdata-sections -O3 -DNDEBUG  -fno-exceptions -fno-rtti -D_GNU_SOURCE -fvisibility-inlines-hidden -Wsign-compare"
ENV LLVM_LIBS="-lLLVMLTO -lLLVMPasses -lLLVMObjCARCOpts -lLLVMSymbolize -lLLVMDebugInfoPDB -lLLVMDebugInfoDWARF -lLLVMMIRParser -lLLVMFuzzMutate -lLLVMCoverage -lLLVMTableGen -lLLVMDlltoolDriver -lLLVMOrcJIT -lLLVMXCoreDisassembler -lLLVMXCoreCodeGen -lLLVMXCoreDesc -lLLVMXCoreInfo -lLLVMXCoreAsmPrinter -lLLVMSystemZDisassembler -lLLVMSystemZCodeGen -lLLVMSystemZAsmParser -lLLVMSystemZDesc -lLLVMSystemZInfo -lLLVMSystemZAsmPrinter -lLLVMSparcDisassembler -lLLVMSparcCodeGen -lLLVMSparcAsmParser -lLLVMSparcDesc -lLLVMSparcInfo -lLLVMSparcAsmPrinter -lLLVMPowerPCDisassembler -lLLVMPowerPCCodeGen -lLLVMPowerPCAsmParser -lLLVMPowerPCDesc -lLLVMPowerPCInfo -lLLVMPowerPCAsmPrinter -lLLVMNVPTXCodeGen -lLLVMNVPTXDesc -lLLVMNVPTXInfo -lLLVMNVPTXAsmPrinter -lLLVMMSP430CodeGen -lLLVMMSP430Desc -lLLVMMSP430Info -lLLVMMSP430AsmPrinter -lLLVMMipsDisassembler -lLLVMMipsCodeGen -lLLVMMipsAsmParser -lLLVMMipsDesc -lLLVMMipsInfo -lLLVMMipsAsmPrinter -lLLVMLanaiDisassembler -lLLVMLanaiCodeGen -lLLVMLanaiAsmParser -lLLVMLanaiDesc -lLLVMLanaiAsmPrinter -lLLVMLanaiInfo -lLLVMHexagonDisassembler -lLLVMHexagonCodeGen -lLLVMHexagonAsmParser -lLLVMHexagonDesc -lLLVMHexagonInfo -lLLVMBPFDisassembler -lLLVMBPFCodeGen -lLLVMBPFAsmParser -lLLVMBPFDesc -lLLVMBPFInfo -lLLVMBPFAsmPrinter -lLLVMARMDisassembler -lLLVMARMCodeGen -lLLVMARMAsmParser -lLLVMARMDesc -lLLVMARMInfo -lLLVMARMAsmPrinter -lLLVMARMUtils -lLLVMAMDGPUDisassembler -lLLVMAMDGPUCodeGen -lLLVMAMDGPUAsmParser -lLLVMAMDGPUDesc -lLLVMAMDGPUInfo -lLLVMAMDGPUAsmPrinter -lLLVMAMDGPUUtils -lLLVMAArch64Disassembler -lLLVMAArch64CodeGen -lLLVMAArch64AsmParser -lLLVMAArch64Desc -lLLVMAArch64Info -lLLVMAArch64AsmPrinter -lLLVMAArch64Utils -lLLVMObjectYAML -lLLVMLibDriver -lLLVMOption -lLLVMWindowsManifest -lLLVMX86Disassembler -lLLVMX86AsmParser -lLLVMX86CodeGen -lLLVMGlobalISel -lLLVMSelectionDAG -lLLVMAsmPrinter -lLLVMX86Desc -lLLVMMCDisassembler -lLLVMX86Info -lLLVMX86AsmPrinter -lLLVMX86Utils -lLLVMMCJIT -lLLVMLineEditor -lLLVMInterpreter -lLLVMExecutionEngine -lLLVMRuntimeDyld -lLLVMCodeGen -lLLVMTarget -lLLVMCoroutines -lLLVMipo -lLLVMInstrumentation -lLLVMVectorize -lLLVMScalarOpts -lLLVMLinker -lLLVMIRReader -lLLVMAsmParser -lLLVMInstCombine -lLLVMBitWriter -lLLVMAggressiveInstCombine -lLLVMTransformUtils -lLLVMAnalysis -lLLVMProfileData -lLLVMObject -lLLVMMCParser -lLLVMMC -lLLVMDebugInfoCodeView -lLLVMDebugInfoMSF -lLLVMBitReader -lLLVMCore -lLLVMBinaryFormat -lLLVMSupport -lLLVMDemangle"

WORKDIR /

##################################
# finished setting up base image #
##################################

# The following setup will be dependent on usage (either development or test)
# Ideally, we would use a multistage docker file, but we won't save much space as the unzipped clang is very large
#RUN mkdir systemc-clang
# RUN pip3 install -r systemc-clang/requirements.txt
ENV SYSTEMC_CLANG_BUILD_DIR=/systemc-clang-build
ENV SYSTEMC_CLANG=/systemc-clang
WORKDIR /systemc-clang-build
#COPY scripts/build-travis.sh /systemc-clang-build
COPY requirements.txt /systemc-clang-build
RUN pip3 install -r $SYSTEMC_CLANG_BUILD_DIR/requirements.txt
# RUN cmake ../systemc-clang -DHDL=ON -DENABLE_VERILOG_TESTS=OFF -DENABLE_TESTS=ON -G "Ninja" && ninja
