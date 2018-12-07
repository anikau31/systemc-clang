Some useful commands and notes
==================

To speed up the parsing, we use pre-compiled headers. 

* Create the PCH for systemc.h, 

```bash
$ clang -cc1 $SYSTEMC/include/systemc.h -emit-pch -o systemc.h.pch -x c++ -I$SYSTEMC/include
```

* Dump the CFG using the following command.

```bash
clang++ -cc1  -analyze -analyzer-checker=debug.DumpCFG -x c++ <filename>.cpp
```
* Dump the Graphviz CFG using the following command.

```bash
clang++ -cc1  -analyze -analyzer-checker=debug.ViewCFG -x c++ <filename>.cpp
```


* Compile project for Xcode (Mac specific) using cmake

```bash
$ mkdir build
$ cmake -G "Xcode" <path-to-systemc-clang>/ 
```

