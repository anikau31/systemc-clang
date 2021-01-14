#!/bin/bash

T=$(mktemp -d)
trap "rm -rf $T" EXIT

# Testing the scripts in the README.md executes as expected

echo "\$ ls \$SYSTEMC_CLANG"
ls $SYSTEMC_CLANG
echo ""

echo "\$ pip install -r \$SYSTEMC_CLANG/requirements.txt"
pip3 install -r $SYSTEMC_CLANG/requirements.txt
echo ""

echo "\$ python3 \$SYSTEMC_CLANG/tests/verilog-conversion/run-compare.py -h"
python3 $SYSTEMC_CLANG/tests/verilog-conversion/run-compare.py -h
echo ""

cd $T
echo "\$ mkdir systemc-clang-test-tool && cd systemc-clang-test-tool/"
mkdir systemc-clang-test-tool && cd systemc-clang-test-tool/
pwd
echo ""

echo "\$ cp \$SYSTEMC_CLANG/tests/verilog-conversion/data/add.cpp ."
cp $SYSTEMC_CLANG/tests/verilog-conversion/data/add.cpp .
cat $SYSTEMC_CLANG/tests/verilog-conversion/data/add.cpp .
echo ""

echo "\$ mkdir results && ls"
mkdir results && ls
echo ""

echo "$ python3 -B \$SYSTEMC_CLANG/tests/verilog-conversion/run-compare.py \\
     cpp-to-hdl \\
     --output-dir ./results/  \\
     --cpp add.cpp \\
     --verbose"
python3 -B $SYSTEMC_CLANG/tests/verilog-conversion/run-compare.py \
     cpp-to-hdl \
     --output-dir ./results/  \
     --cpp add.cpp \
     --verbose
echo ""

echo "ls -R"
ls -R
echo ""


echo "$ mkdir golden && cp \$SYSTEMC_CLANG/tests/verilog-conversion/data/add_hdl.txt golden/"
mkdir golden && cp $SYSTEMC_CLANG/tests/verilog-conversion/data/add_hdl.txt golden/

echo "$ ls ."
ls .

echo "$ ls golden/"
ls golden/

echo "$ python3 -B \$SYSTEMC_CLANG/tests/verilog-conversion/run-compare.py \\
     cpp-to-hdl \\
     --output-dir ./results/  \\
     --cpp add.cpp \\
     --verbose \\
     --golden-intermediate ./golden/add_hdl.txt
"
python3 -B $SYSTEMC_CLANG/tests/verilog-conversion/run-compare.py \
     cpp-to-hdl \
     --output-dir ./results/  \
     --cpp add.cpp \
     --verbose \
     --golden-intermediate ./golden/add_hdl.txt

echo "$ ls -R"
ls -R
echo ""

echo "$ python3 -B \$SYSTEMC_CLANG/tests/verilog-conversion/run-compare.py \\
     hdl-to-v \\
     --output-dir results/ \\
     --hdl golden/add_hdl.txt \\
     --verbose"
python3 -B $SYSTEMC_CLANG/tests/verilog-conversion/run-compare.py \
     hdl-to-v \
     --output-dir results/ \
     --hdl golden/add_hdl.txt \
     --verbose
echo ""

echo "$ ls -R"
ls -R
echo ""

echo "$ python3 -B \$SYSTEMC_CLANG/tests/verilog-conversion/run-compare.py \\
     hdl-to-v \\
     --output-dir results/ \\
     --hdl golden/add_hdl.txt \\
     --verbose \\
     --golden-verilog $SYSTEMC_CLANG/tests/verilog-conversion/data/add_hdl.txt.v"
python3 -B $SYSTEMC_CLANG/tests/verilog-conversion/run-compare.py \
     hdl-to-v \
     --output-dir results/ \
     --hdl golden/add_hdl.txt \
     --verbose \
     --golden-verilog $SYSTEMC_CLANG/tests/verilog-conversion/data/add_hdl.txt.v
echo ""
