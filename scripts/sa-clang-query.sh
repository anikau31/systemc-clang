#!/bin/bash

################################################################################
# Script to allow running clang-query on SystemC programs. 
################################################################################

if [ -z "$1" ]; then
    echo [sa-clang-query] Error: You must specify a filename;
    exit;
fi

if [ -z "$SYSTEMC" ]; then 
    echo [sa-clang-query] Error: Environment variable SYSTEMC is not set or empty;
    exit;
fi

# Execute clang query.
clang-query $1 -extra-arg=-I$SYSTEMC/include

