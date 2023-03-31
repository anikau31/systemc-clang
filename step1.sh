#!/bin/bash

SCCL="/systemc-clang-build/systemc-clang"
SRC="/systemc-clang/fccm-case-studies"
RUN="/systemc-clang/scripts/sc"
# Run systemc-clang on the designs

# Go into the case study directory
cd $SRC;

# The following commands will generate SystemVerilog directly

# A. MA
$RUN ultra96-vitis-ae/moving-average-d16-w16/moving-average.cpp -I$SRC/moving-average -I$SRC/zfpsynth
$RUN ultra96-vitis-ae/moving-average-d16-w64/moving-average.cpp -I$SRC/moving-average -I$SRC/zfpsynth
$RUN ultra96-vitis-ae/moving-average-d64-w16/moving-average.cpp -I$SRC/moving-average -I$SRC/zfpsynth
$RUN ultra96-vitis-ae/moving-average-d64-w64/moving-average.cpp -I$SRC/moving-average -I$SRC/zfpsynth

Shift
$RUN ultra96-vitis-ae/moving-average-shift-d16-w16/moving-average.cpp -I$SRC/moving-average-shift -I$SRC/zfpsynth
$RUN ultra96-vitis-ae/moving-average-shift-d16-w64/moving-average.cpp -I$SRC/moving-average-shift -I$SRC/zfpsynth
$RUN ultra96-vitis-ae/moving-average-shift-d64-w16/moving-average.cpp -I$SRC/moving-average-shift -I$SRC/zfpsynth
$RUN ultra96-vitis-ae/moving-average-shift-d64-w64/moving-average.cpp -I$SRC/moving-average-shift -I$SRC/zfpsynth

# pipe
$RUN ultra96-vitis-ae/moving-average-pipe-d16-w16/moving-average.cpp -I$SRC/moving-average-pipe -I$SRC/zfpsynth
$RUN ultra96-vitis-ae/moving-average-pipe-d16-w64/moving-average.cpp -I$SRC/moving-average-pipe -I$SRC/zfpsynth
$RUN ultra96-vitis-ae/moving-average-pipe-d64-w16/moving-average.cpp -I$SRC/moving-average-pipe -I$SRC/zfpsynth
$RUN ultra96-vitis-ae/moving-average-pipe-d64-w64/moving-average.cpp -I$SRC/moving-average-pipe -I$SRC/zfpsynth

# B.ZFP enc/dec
# E1
$RUN zfpsynth/zfp3/z3test.cpp
cp $SRC/zfpsynth/zfp3/z3test_hdl.txt{.v,.sv}
# E2
$RUN zfpsynth/zfp3-fp32/z3test.cpp
cp $SRC/zfpsynth/zfp3-fp32/z3test_hdl.txt{.v,.sv}
# D1
$RUN zfpsynth/zfp7/z7test.cpp
cp $SRC/zfpsynth/zfp7/z7test_hdl.txt{.v,.sv}
# D2
$RUN zfpsynth/zfp7_fp32_2d/z7test.cpp
cp $SRC/zfpsynth/zfp7_fp32_2d/z7test_hdl.txt{.v,.sv}

# C.systolic array
$RUN ultra96-vitis-ae/mm-n8-d8/mm.cpp
$RUN ultra96-vitis-ae/mm-n16-d8/mm.cpp

# D.Threads with div
$RUN ultra96-vitis-ae/div-8/divider.cpp -I$SRC/divider-thread -I$SRC/zfpsynth
$RUN ultra96-vitis-ae/div-16/divider.cpp -I$SRC/divider-thread -I$SRC/zfpsynth
$RUN ultra96-vitis-ae/div-32/divider.cpp -I$SRC/divider-thread -I$SRC/zfpsynth
$RUN ultra96-vitis-ae/div-64/divider.cpp -I$SRC/divider-thread -I$SRC/zfpsynth

# Rename from sv -> v
