# A SystemC Parser using the Clang Front-end [![Build Status](https://travis-ci.com/anikau31/systemc-clang.svg?branch=master)](https://travis-ci.com/anikau31/systemc-clang)

[systemc-clang](https://git.uwaterloo.ca/caesr-pub//systemc-clang) parses SystemC 2.3.3.
It parses RTL constructs and some TLM 2.0 constructs. 

## Requirements
* [llvm/clang](https://releases.llvm.org/download.html)(version 7.0.0)
* [SystemC](http://systemc.org) version 2.3.3. Please see [SystemC Installation notes](https://github.com/anikau31/systemc-clang/blob/master/doc/systemc-install.mkd)
* c++14 is required.  We are using some features that necessitate c++14.  Down-porting it is also possible, but not supported.

## Installation 
* [Linux](INSTALL-linux.md)
* [OSX](INSTALL-osx.md)

## Plugin Xlat

  To compile with the Xlat plugin, run cmake with the `-DXLAT=on` flag. 
  
## Tests
  To enable compilation of tests, run cmake with the `-DENABLE_TESTS=on` flag.

## Contact

If you encounter problems, please create issues with a minimally working example that illustrates the issue.  

For other concerns and comments, please contact us directly.
* [Anirudh M. Kaushik](https://ece.uwaterloo.ca/~anikau31/uwhtml/team/anirudh-kaushik/)
* [Hiren Patel](https://caesr.uwaterloo.ca)

## License

systemc-clang follows the same licensing as clang.  Please look at [LICENSE]( https://git.uwaterloo.ca/caesr-pub/systemc-clang/LICENSE.md).
