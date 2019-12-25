# A SystemC Parser using the Clang Front-end [![Build Status](https://travis-ci.com/anikau31/systemc-clang.svg?branch=master)](https://travis-ci.com/anikau31/systemc-clang)

[systemc-clang](https://git.uwaterloo.ca/caesr-pub//systemc-clang) parses SystemC 2.3.3.
It parses RTL constructs and some TLM 2.0 constructs. 

## Requirements

* [llvm/clang](https://releases.llvm.org/download.html)(version 7.0.0)
* [SystemC](http://systemc.org) version 2.3.3. Please see [SystemC Installation notes](https://github.com/anikau31/systemc-clang/blob/master/doc/systemc-install.mkd)
* c++14 is required.  We are using some features that necessitate c++14.  Down-porting it is also possible, but not supported.

## Installation 

* [Linux](doc/INSTALL-linux.md)
* [OSX](doc/INSTALL-osx.md)

## Plugin Xlat

  To compile with the Xlat plugin, run cmake with the `-DXLAT=on` flag. 
  
## Tests
  To enable compilation of tests, run cmake with the `-DENABLE_TESTS=on` flag and also the `-DSYSTEMC_DIR=<path>` flag to pass the location for SystemC.  Without specifying the `SYSTEMC_DIR` path, it will not be possible to run the unit tests.

### Python Tests for Verilog conversion

  1. To enable the python tests, run cmake with the `-DENABLE_PYTHON_TESTS=on` flag. Note that `python 3` should be installed.
  2. To install necessary packages listed in `requirements.txt`, run `pip -r requirements.txt` in the **repo** directory.
  3. To run the python tests, switch to the `$SYSTEMC_CLANG_BUILD_DIR` build directory and run `ctest -R python --output` after the build completes.

## Contact

If you encounter problems, please create issues with a minimally working example that illustrates the issue.  

For other concerns and comments, please contact us directly.
* [Anirudh M. Kaushik](https://ece.uwaterloo.ca/~anikau31/uwhtml/team/anirudh-kaushik/)
* [Hiren Patel](https://caesr.uwaterloo.ca)

## License

systemc-clang follows the same licensing as clang.  Please look at [LICENSE](https://github.com/anikau31/systemc-clang/blob/master/LICENSE).
