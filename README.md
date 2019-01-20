# [systemc-clang]: A SystemC Parser using the Clang Front-end

[systemc-clang](https://git.uwaterloo.ca/caesr-pub//systemc-clang) parses SystemC 2.3. 
It parses RTL constructs and some TLM 2.0 constructs. 


Requirements
------------
* [llvm](http://llvm.org) 
* [clang](http://clang.llvm.org) (version 3.4)
* [SystemC](http://systemc.org) version 2.3
* Please see [SystemC Installation notes](https://github.com/anikau31/systemc-clang/blob/master/doc/systemc-install.mkd)

## Installation 

* This version of systemc-clang compiles as a dynamic library.  It requires LLVM and clang to be installed.   Please follow the directions below

Linux
===========
* You can install LLVM and Clang via source or via binaries. 

### LLVM/Clang from source 

* Follow the instructions to install clang: [Get Started](http://clang.llvm.org/get_started.html). Notice that you should also install LLVM as it is needed for compilation of the stand-along program. A suggestion is to install LLVM/clang in a location that is accessible by the user.
Note that the clang-llvm version systemc-clang supports is version 3.4. So, when following the instructions posted at [Get Started](http://clang.llvm.org/get_started.html). please substitute the phrase "trunk" with "tags/RELEASE_34/final".
 
* Clone systemc-clang repository.

```bash  
$ git clone gitlab@git.uwaterloo.ca/caesr-pub/systemc-clang.git
```

* Create a build directory for this stand-alone program.

```bash
$ mkdir systemc-clang-build
```

* Enter the cloned repository folder.

```bash
$ cd systemc-clang
```

* Edit the path for the `LLVM_BUILD_DIR` in paths.sh, and set it to the appropriate path where LLVM/clang was installed.

* Source the paths file (assuming bash shell).  This should setup certain environment variables that cmake will use.

```bash
$ source scripts/paths.sh
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

Testing on a small example
==========================

* Copy the compiled binary in the bin folder of LLVM_BUILD_DIR
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

Contact
-------
* [Anirudh M. Kaushik](https://ece.uwaterloo.ca/~anikau31/uwhtml/team/anirudh-kaushik/)

License
--------
* systemc-clang follows the same licensing as clang.  Please look at [LICENSE]( https://git.uwaterloo.ca/caesr-pub/systemc-clang/master/master/LICENSE.md).
