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

  1. In the **repo_** directory, use `git submodule update` to pull the Verilog conversion data.
  2. To enable the python tests, run cmake with the `-DENABLE_PYTHON_TESTS=on` flag. Note that `python 3` should be installed.
  3. To install necessary packages listed in `requirements.txt`, run `pip install -r requirements.txt` in the **repo** directory.
  4. To run the python tests, switch to the `$SYSTEMC_CLANG_BUILD_DIR` build directory and run `ctest -R python --output` after the build completes.

#### Details

  - To observe the failing tests, one can run the tests in `tests/verilog-conversion/` by running `pytest -s -v -q --tool-output`.
  The `-s` option captures the standard output for the test, `-v` enables verbose mode, and `-q --tool-output` captures the output of the binary.


### Running conversion comparison in shell
  The script requires `pyverilog` to be installed, which can be achieved using the command: `pip install pyverilog`
  Note that when using the script, the working directory needs to be `tests/verilog-conversion`
  ```
  $ python -m run-compare
  usage: A tool for running and comparing against a golden standard
       [-h] [--cpp CPP] [--hdl HDL] [--verilog VERILOG]
       [--include-path [INCLUDE_PATH [INCLUDE_PATH ...]]]
       [--output-dir OUTPUT_DIR] [--verbose]
       {cpp-to-hdl,hdl-to-v,cpp-to-v}
   ```
   
#### Examples
##### Converting sreg-driver.cpp to sreg-driver_hdl.txt_
   ```
   python -B -m run-compare cpp-to-hdl --output-dir /tmp/  --cpp ../data/llnl-examples/sreg-driver.cpp --include-path ../../examples/llnl-examples/ --verbose
   ```
   The generated file will be placed into a timestamped directory in `/tmp/`, for example, `/tmp/2020-02-17_01-28-52/sreg-driver_hdl.txt`
##### Converting sreg-driver.cpp to sreg-driver_hdl.txt and show the diff of an existing _`_hdl.txt` file
   Compared to the last command, we need an addition `--hdl` option to specify the file to compare against:
   ```
   python -B -m run-compare cpp-to-hdl --output-dir /tmp/  --cpp ../data/llnl-examples/sreg-driver.cpp --include-path ../../examples/llnl-examples/ --hdl ../data/verilog-conversion/llnl-examples/sreg_hdl.txt
   ```
##### Converting sreg-driver_hdl.txt to sreg-driver.v
   ```
   python -B -m run-compare hdl-to-v --output-dir /tmp/ --hdl /tmp/2020-02-16_23-53-13/sreg-driver_hdl.txt --verbose
   ```
   The generated file will be placed into a timestamped directory in `/tmp/`, for example, `/tmp/2020-02-17_01-28-52/sreg-driver.v`
##### Converting sreg-driver_hdl.txt to sreg-driver.v and show the diff of an existing Verilog file
   ```
   python -B -m run-compare hdl-to-v --output-dir /tmp/ --hdl /tmp/2020-02-16_23-53-13/sreg-driver_hdl.txt --verilog ../data/verilog-conversion/llnl-exmples/handcrafted/sreg.v
   ```


## Contact

If you encounter problems, please create issues with a minimally working example that illustrates the issue.  

For other concerns and comments, please contact us directly.
* [Anirudh M. Kaushik](https://ece.uwaterloo.ca/~anikau31/uwhtml/team/anirudh-kaushik/)
* [Hiren Patel](https://caesr.uwaterloo.ca)

## License

systemc-clang follows the same licensing as clang.  Please look at [LICENSE](https://github.com/anikau31/systemc-clang/blob/master/LICENSE).
