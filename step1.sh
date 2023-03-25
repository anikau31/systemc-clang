#!/bin/bash

SCCL="/systemc-clang-build/systemc-clang"
SRC="/systemc-clang/fccm-case-studies"
RUN="/systemc-clang/scripts/run.sh"
# Run systemc-clang on the designs

# Go into the case study directory
cd $SRC;

# z3
$RUN zfpsynth/zfp3/z3test.cpp

# Add other tests
