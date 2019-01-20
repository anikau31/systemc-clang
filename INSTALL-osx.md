# Mac OSX
We tested this installation on OS X. 

* systemc-clang requires llvm-config and by default does not come with the llvm package preinstalled on MacOS. 

* Download the clang pre built binaries from [here](http://llvm.org/releases/download.html#3.3) for LLVM 3.4. 

* Untar the package into a suitable location, and modify the path.sh bash script in the systemc-clang folder to point to llvm-config, which will be found in the bin directory of the uncompressed binary. 

* Proceed with the steps for Linux OS, and systemc-clang binary build should be smooth.

Testing on a small example
==========================

* Copy the compiled binary in the bin folder of `LLVM_BUILD_DIR`

```bash
$ cp systemc-clang $LLVM_BUILD_DIR/bin
```

* Set the `SYSTEMC` variable.

```bash
$ export SYSTEMC=<path-to-systemc-install>
```

* Execute it on a simple example.  You should see some (debug) information print out on the screen.  Hope to make this more meaningful in the future.

```bash 
$ $LLVM_BUILD_DIR/bin/systemc-clang ../systemc-clang/tests/module1.hpp -- \
-D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS  -I/usr/include  \
-I$SYSTEMC/include  -x c++ -w -c
```
