Installation
============

* This version of systemc-clang compiles as a dynamic library.  It requires LLVM and clang to be installed.   Please follow the directions below


Steps (Linux OS)
===========
* Follow the instructions to install clang: [Get Started](http://clang.llvm.org/get_started.html). Notice that you should also install LLVM as it is needed for compilation of the stand-along program. A suggestion is to install LLVM/clang in a location that is accessible by the user.
Note that the clang-llvm version systemc-clang supports is version 3.4. So, when following the instructions posted at [Get Started](http://clang.llvm.org/get_started.html). please substitute the phrase "trunk" with "tags/RELEASE_34/final".
 
* Clone systemc-clang repository.

```bash  
$ git clone git@github.com:anikau31/systemc-clang.git
```

* Create a build directory for this stand-alone program.

```bash
$ mkdir systemc-clang-build
```

* Enter the cloned repository folder.

```bash
$ cd systemc-clang
```

* Edit the path for the LLVM_BUILD_DIR in paths.sh, and set it to the appropriate path where LLVM/clang was installed.

* Source the paths file (assuming bash shell).  This should setup certain environment variables that cmake will use.

```bash
$ source paths.sh
```

* Go to the build directory.

```bash
$ cd ../systemc-clang-build
```

* Create the makefiles using cmake.

```bash
$ cmake ../systemc-clang
```

* If there are no errors, then the compilation of Makefiles was successful.  So, you can go ahead and compile the stand-alone program.

```bash
$ make
```

Steps (MacOS)
===========
We tested this installation on OS X. 

* systemc-clang requires llvm-config and by default does not come with the llvm package preinstalled on MacOS. 

* Download the clang pre built binaries from [here](http://llvm.org/releases/download.html#3.3) for LLVM 3.4. 

* Untar the package into a suitable location, and modify the path.sh bash script in the systemc-clang folder to point to llvm-config, which will be found in the bin directory of the uncompressed binary. 

* Proceed with the steps for Linux OS, and systemc-clang binary build should be smooth.

Testing on a small example
==========================

* Copy the compiled binary in the bin folder of LLVM_BUILD_DIR

```bash
$ cp systemc-clang $LLVM_BUILD_DIR/bin
```

* Set the SYSTEMC variable.

```bash
$ export SYSTEMC=<path-to-systemc-install>
```

* Execute it on a simple example.  You should see some (debug) information print out on the screen.  Hope to make this more meaningful in the future.

```bash 
$ $LLVM_BUILD_DIR/bin/systemc-clang ../systemc-clang/tests/module1.hpp -- \
-D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS  -I/usr/include  \
-I$SYSTEMC/include  -x c++ -w -c
```
