#!/bin/bash
if [ "$#" -ne 1 ]; then
    echo "usage: runTest <filename.cpp>"
    exit
fi

if [ -z $SYSTEMC_HOME ]; then 
    echo -e "environment variable \$SYSTEMC_HOME not defined, set the variable to SystemC directory\n(e.g. export SYSTEMC_HOME=\"/usr/local/systemc-2.3.1\")";
else
    systemc-clang $1 -- -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -I/usr/include/c++/4.2.1 -I/usr/include -I/$SYSTEMC_HOME/include;
fi;
