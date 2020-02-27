# Python Tests for Verilog Conversion and Utilites for Development

## Prelimilaries
  Python tests are configured during the process of calling `cmake`, to enable the python tests, run `cmake` with `-DENABLE_VERILOG_TESTS=on` flag along with other build options when [building the binaries](/doc/README.md#Installation).
  
  For example, to enable the python Verilog tests along with XLAT, the `cmake` command will be similar to:  `cmake ../systemc-clang -DXLAT=on -DENABLE_TESTS=on -DSYSTEMC_DIR=$SYSTEMC/systemc/ -G Ninja -DENABLE_VERILOG_TESTS=on`.

  If the python interpreter are found, `cmake` should report lines similar to:
  ```
  -- [ Tests         ]
  -- Build tests           : on
  -- Build Verilog tests   : on
  ...
  -- Python found          : TRUE
  -- Python interpreter    : /home/allen/anaconda3/bin/python3
  --
  ```
  Also, before running the tests, `$SYSTEMC_CLANG` should point to the git repository directory, for example, with `export SYSTEMC_CLANG=~/systemc-clang/` in shell.

  Note that before calling the `cmake`, you should already source the `script/paths.sh` and after **building the binaries**, you should copy the generated `systemc-clang` to `$LLVM_INSTALL_DIR/bin`.

  The following steps will be dependent on the environment variables defined in the `script/paths.sh` to search for the binaries and the python scripts.

  We use `$SYSTEMC_CLANG_BUILD_DIR` to refer to the build directory created for `cmake` and `$SYSTEMC_CLANG` to refer to the git repository directory.

## Setup
  1. Python *3* is required to run these tests.
  2. To install necessary packages listed in `requirements.txt`, run `pip install -r requirements.txt` in `$SYSTEMC_CLANG`. 
  Note that you might need to make sure whether `pip` command matches the python reported in the `cmake`.

  The python packages to install are: 
  - `pytest` for running tests
  - `lark-parser` for parsing and translating
  - `pyverilog` for parsing Verilog for verification.

## Running tests from ctest
  To run the Verilog tests, switch to `$SYSTEMC_CLANG_BUILD_DIR`, build `systemc-clang` and copy the binary.
  Then run:
  ```
  ctest -R verilog-tests
  ``` 
  If the tests do not pass, running the following command will provide more information:
  ```
  ctest -R verilog-tests --verbose
  ```

## Running tests manually
  It is also possible to run the tests manually from python scripts for fine-grained control over what to test.
  To run all the Verilog tests manually, in `$SYSTEMC_CLANG_BUILD_DIR`:
  ```
  python -B run-verilog-tests.py
  ```

### List all tests
  It is possible to list all tests that can be supplied for the `-o` argument for `run-verilog-tests.py`.
  To list all available tests, in `$SYSTEMC_CLANG_BUILD_DIR`, run:
  ```
  python -B run-verilog-tests.py --collect-only
  ```
  For a more detailed list, use `-v` option:
  ```
  python -B run-verilog-tests.py --collect-only -v
  ```
  The returned list contains the filename in which the tests are defined, followed by `::` and the corresponding test names.

### Individual tests
  It is possible to run individual python tests to observe the output with `-o` option.
  For example, in `$SYSTEMC_CLANG_BUILD_DIR`:
  ```
  python -B run-verilog-tests.py --only test_ex_sexp[1]
  ```

---

## Running conversion comparison in shell
  The script requires `pyverilog` to be installed, which can be achieved using the command: `pip install pyverilog`
  Note that when using the script, the working directory needs to be `tests/verilog-conversion`
  ```
  $ python $SYSTEMC_CLANG/tests/verilog-conversion/run-compare.py -h
  usage: A tool for running and comparing against a golden standard
       [-h] [--cpp CPP] [--hdl HDL] [--verilog VERILOG]
       [--include-path [INCLUDE_PATH [INCLUDE_PATH ...]]]
       [--output-dir OUTPUT_DIR] [--verbose]
       {cpp-to-hdl,hdl-to-v,cpp-to-v}
   ```
   
### Examples
#### Converting sreg-driver.cpp to sreg-driver_hdl.txt
   ```
   python -B $SYSTEMC_CLANG/tests/verilog-conversion/run-compare.py \
     cpp-to-hdl \
     --output-dir /tmp/  \
     --cpp $SYSTEMC_CLANG_BUILD_DIR/tests/data/llnl-examples/sreg-driver.cpp \
     --include-path $SYSTEMC_CLANG/examples/llnl-examples/ --verbose
   ```
   The generated file will be placed into a timestamped directory in `/tmp/`, for example, `/tmp/2020-02-17_01-28-52/sreg-driver_hdl.txt`
#### Converting sreg-driver.cpp to sreg-driver_hdl.txt and show the diff of an existing `_hdl.txt` file
   Compared to the last command, we need an addition `--hdl` option to specify the file to compare against:
   ```
   python -B $SYSTEMC_CLANG/tests/verilog-conversion/run-compare.py \
     cpp-to-hdl \
     --output-dir /tmp/ \
     --cpp $SYSTEMC_CLANG_BUILD_DIR/tests/data/llnl-examples/sreg-driver.cpp \
     --include-path $SYSTEMC_CLANG/tests/examples/llnl-examples/ \
     --hdl $SYSTEMC_CLANG_BUILD_DIR/tests/data/verilog-conversion/llnl-examples/golden/sreg_hdl.txt
     --verbose
   ```
#### Converting sreg-driver_hdl.txt to sreg-driver.v
   ```
   python -B $SYSTEMC_CLANG/tests/verilog-conversion/run-compare.py \
     hdl-to-v \
     --output-dir /tmp/ \
     --hdl /tmp/2020-02-16_23-53-13/sreg-driver_hdl.txt \
     --verbose
   ```
   Note that here the `--hdl` is specified with a previously generated `_hdl.txt` file.
   The generated file will be placed into a timestamped directory in `/tmp/`, for example, `/tmp/2020-02-17_01-28-52/sreg-driver.v`
#### Converting sreg-driver_hdl.txt to sreg-driver.v and show the diff of an existing Verilog file
   ```
   python -B $SYSTEMC_CLANG/tests/verilog-conversion/run-compare.py \
     hdl-to-v \
     --output-dir /tmp/ \
     --hdl /tmp/2020-02-16_23-53-13/sreg-driver_hdl.txt \
     --verilog $SYSTEMC_CLANG_BUILD_DIR/tests/data/verilog-conversion/llnl-exmples/golden/sreg.v
   ```

