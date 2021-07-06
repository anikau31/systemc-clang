#!/bin/bash

clang -fsyntax-only -Xclang -analyze -Xclang -analyzer-checker=debug.DumpCFG $1 -I$SYSTEMC/include/

