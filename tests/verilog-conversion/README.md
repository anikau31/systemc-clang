# Python Tests for Verilog Conversion and Utilites for Development

## Prelimilaries
### CMake switch 
  Python tests are configured during the process of calling `cmake`, to enable the python tests, run `cmake` with `-DENABLE_VERILOG_TESTS=on` flag along with other build options when [building the binaries](/doc/README.md#Installation).
  
  For example, to enable the python Verilog tests along with XLAT, the `cmake` command will be similar to (make sure the `-DSYSTEMC_DIR` is correctly specified): 
  ```
  cmake ../systemc-clang -DXLAT=on -DENABLE_TESTS=on -DSYSTEMC_DIR=$SYSTEMC/ -G Ninja -DENABLE_VERILOG_TESTS=on
  ```

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

### Additional environment variables

  Also, before running the tests, `$SYSTEMC_CLANG` should point to the git repository directory, for example, with: 
  ```
  export SYSTEMC_CLANG=/home/$USER/systemc-clang/
  ``` 
  in shell.
  
  Make sure the following command list the root of the git repository:
  ```
  ls $SYSTEMC_CLANG
  ```


### Build systemc-clang

  After `cmake`, build the project using `make` or `ninja`.

### Done

  Note that before calling the `cmake`, you should already source the `script/paths.sh` and after **building the binaries**, the `systemc-clang` is generated in `$LLVM_INSTALL_DIR/bin`.

  The following steps will be dependent on the environment variables defined in the `script/paths.sh` to search for the binaries and the python scripts.

  We use `$SYSTEMC_CLANG_BUILD_DIR` to refer to the build directory created for `cmake` and `$SYSTEMC_CLANG` to refer to the git repository directory.


## Setup
  1. Python *3* is required to run these tests.
  2. Change the directory to the build directory: `cd $SYSTEMC_CLANG_BUILD_DIR`.
  3. Install `iverilog`. On Ubuntu, use `sudo apt install iverilog`
  4. To install necessary packages listed in `requirements.txt`, run
  ```
  pip install -r $SYSTEMC_CLANG/requirements.txt
  ``` 

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

  The tests might not all pass at this moment, we provide a set of sanity tests that should pass at this point:
  ```
  ctest -R verilog-sanity-tests --verbose
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
  python -B run-verilog-tests.py --only test_sanity_add_sexp_to_verilog
  ```

## Adding tests
  New tests can be added to `$SYSTEMC_CLANG/tests/data/verilog-conversion-custom/`, and they will be recognized automatically by the pytest without need to modify the script.

  The following example shows the directory structure of one conversion test for an add module:
  ```
  $SYSTEMC_CLANG/tests/data/verilog-conversion-custom/
  └── add
      ├── add.cpp
      └── golden
          ├── add_hdl.txt
          └── add_hdl.txt.v
   
  ```
  In this example, all necessary files are grouped in an `add` folder in `$SYSTEMC_CLANG/tests/data/verilog-conversion-custom/`.

  Within the `add` folder, there is a SystemC design file `add.cpp` and a `golden` folder that stores the reference file for `_hdl.txt` and Verilog.

  Note that all the file needs to have the same prefix.
  In this example, the prefix is `add`.

  To add another test for a different design, organize the `.cpp` SystemC design file, `_hdl.txt` file and Verilog file `_hdl.txt.v` as is shown in the previous example.

  For example, if we want to add an `xor` module, the final directory structure will end up being the following:
  ```
  $SYSTEMC_CLANG/tests/data/verilog-conversion-custom/
  ├── add
  │   ├── add.cpp
  │   └── golden
  │       ├── add_hdl.txt
  │       └── add_hdl.txt.v
  └── xor
      ├── golden
      │   ├── xor_hdl.txt
      │   └── xor_hdl.txt.v
      └── xor.cpp
  ```

  To run all these tests, use `ctest -R verilog-custom --verbose`.

  To observe detailed output, use the script `python -B run-verilog-tests.py -o test_custom -v`.

## Debug information on failure
  On a test failure, the output, together with the diff information is stored in the temporary folder.
  For example, suppose we run individual tests with `-v` option and the tests fail, the failure may be reported in the form of:
  ```
  ...
  ___________________________________________________________________ test_custom_sexp_to_verilog[add] ____________________________________________________________________

  tmpdir = local('/tmp/pytest-of-allen/pytest-161/test_custom_sexp_to_verilog_ad0'), customdriver = <driver.SystemCClangDriver object at 0x7f8750d6b910>
  tool_output = False
  ...
  ```
  In the output, `tmpdir = local('/tmp/pytest-of-allen/pytest-161/test_custom_sexp_to_verilog_ad0')` logs the temporary folder where the intermediate output are stored.
  And thus we can observe output in `/tmp/pytest-of-allen/pytest-161/test_custom_sexp_to_verilog_ad0`

---

## Running conversion comparison in shell
  The script requires `pyverilog` and `iverilog` to be installed, which can be achieved using the command: `pip install pyverilog` and `sudo apt install iverilog`:
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
     --hdl $SYSTEMC_CLANG_BUILD_DIR/tests/data/verilog-conversion/llnl-examples/golden/sreg_hdl.txt \
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

