#!/bin/bash

# Replace with your path for systemc
SYSTEMC_HOME=/usr/local/systemc231

# Replace with your path to `include` for clang
CLANG_INCLUDE=/home/jimmy/clang34/lib/clang/3.4/include

# Replace with your path to the systemc-clang executable
SYSTEMC_CLANG=/home/jimmy/Documents/SE499/systemc-clang-build/systemc-clang

if [ "$#" -ne 1 ]; then
    echo "usage: runTest <filename.cpp>"
    exit
fi

if [ -z $SYSTEMC_HOME ]; then 
    echo -e "environment variable \$SYSTEMC_HOME not defined, set the variable to SystemC directory\n(e.g. export SYSTEMC_HOME=\"/usr/local/systemc-2.3.1\")";
else
    $SYSTEMC_CLANG $1 -- -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -I/usr/include -I/$SYSTEMC_HOME/include -I$CLANG_INCLUDE -x c++ -w -c;
fi;
