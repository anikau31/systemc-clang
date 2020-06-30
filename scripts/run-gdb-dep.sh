#!/bin/bash
##############################################################################
#
# Author: Anirudh
# This is a convenience script for testing. 
# 
#
# Usage: run.sh <example-file>.cpp
# Requirement: 
# 						- Set CLANG environment variable to point to the build directory
# 						- Set SYSTEMC to point to the SystemC install directory
##############################################################################

platform='unknown'
unamestr=`uname`
libparse='xxx'
GDB='gdb --command=debug.gdb --args'

#CPPINCLUDE='-I/usr/local/include -I/usr/include/c++/4.2.1/ -I/usr/include/c++/4.2.1/bits'
#CPPINCLUDE='-I/usr/local/include/c++/3.3.2/'


if [ -z "$1" ]; then
    echo Error: You must specify a filename;
    exit;
fi

if [ "$1" == "-gdb" ]; then
    GDB='gdb --command=debug.gdb --args' 
fi

if [[ "$unamestr" == 'Linux' ]]; then
    platform='Linux'
    libparse='parse-systemc.so'
    CPPINCLUDE=' -I/usr/include/c++/4.4 -I/usr/include/c++/4.4/x86_64-linux-gnu -I/usr/include/c++/4.4/backward -I/usr/local/include -I/usr/lib/gcc/x86_64-linux-gnu/4.4.3/include -I/usr/lib/gcc/x86_64-linux-gnu/4.4.3/include-fixed -I/usr/include/x86_64-linux-gnu -I/usr/include'
else
    platform='OSX'
    libparse='parse-systemc.so'
fi

if [ -z "$CLANG" ]; then 
    echo Error: Environment variable CLANG is either not set or empty; 
    exit;
fi

if [ -z "$SYSTEMC" ]; then 
    echo Error: Environment variable SYSTEMC is not set or empty; 
    exit;
fi

# Print some  information
echo ===========================================
echo + CLANG environment: $CLANG
echo + Platform: $platform
echo + libparse: $libparse
echo + GDB: $GDB
echo + CLANG information:
$CLANG/bin/clang --version
echo ===========================================

# This is with cmake build
#$CLANG/bin/clang -cc1 -x c++ -w -load $CLANG/lib/$libparse -I$SYSTEMC/include $CPPINCLUDE -plugin parse-systemc $@

# Use GDB flag
$GDB $CLANG/bin/clang -cc1 -x c++ -w -load $CLANG/lib/$libparse -I$SYSTEMC/include $CPPINCLUDE -plugin parse-systemc $@



#gdb --args $CLANG/bin/clang -cc1 -x c++ -w -load $CLANG/lib/$libparse -I$SYSTEMC/include $CPPINCLUDE -plugin parse-systemc $@
#$CLANG/bin/clang -emit-ast -I$SYSTEMC/include $CPPINCLUDE $@
#$CLANG/Debug+Asserts/bin/clang -cc1 -load $CLANG/Debug+Asserts/lib/$libparse -plugin parse-cpp $1
