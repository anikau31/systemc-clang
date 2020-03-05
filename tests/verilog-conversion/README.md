# Python Tests for Verilog Conversion and Utilites for Development

## Preliminaries

**Before going through this documentation, make sure you have build the systemc-clang successfully, this implies that the `scripts/paths.sh` is sourced.**

### Repository path
Throughout the documentation, we use `$SYSTEMC_CLANG` to specify the systemc-clang git repository directory.
```
$ ls $SYSTEMC_CLANG
cmake/  CMakeLists.txt  doc/  driver-tooling.cpp  driver-xlat.cpp  examples/  externals/  LICENSE  Makefile.systemc  plugins/  README.md  README.rst  requirements.txt  scripts/  src/  tests/  tests-old/
```

### Python dependencies

#### Python version
We use Python 3 in this documentation.
```
$ python --version
Python 3.7.5
```

#### Dependencies
- Install `iverilog`. On Ubuntu, use `sudo apt install iverilog`
- To install necessary packages listed in `requirements.txt`, run
```
pip install -r $SYSTEMC_CLANG/requirements.txt
``` 

The python packages to install in the `requirements.txt` are: 
- `pytest` for running tests
- `lark-parser` for parsing and translating
- `pyverilog` for parsing Verilog for verification.


## The Tool

We provide a command-line tool that helps the development process.
The tool has the following features:

- Convert SystemC to `_hdl.txt`
- Convert `_hdl.txt` to Verilog
- Convert SystemC to Verilog
- The convert result will be stored in time-stamped folders

The tool is located at `/tests/verilog-conversion/run-compare.py` and its usage is 
```
$ python $SYSTEMC_CLANG/tests/verilog-conversion/run-compare.py -h
usage: A tool for running and comparing against a golden standard
     [-h] [--cpp CPP] [--hdl HDL] [--verilog VERILOG]
     [--include-path [INCLUDE_PATH [INCLUDE_PATH ...]]]
     [--output-dir OUTPUT_DIR] [--verbose]
     {cpp-to-hdl,hdl-to-v,cpp-to-v}
 ```

### Examples
In this section, we provide examples on how to use the tool.

Firstly, switch to an empty directory: 

```
$ mkdir systemc-clang-test-tool && cd systemc-clang-test-tool/
```

Next, in the directory, create a file called `add.cpp` with the following content, note that this is a SystemC adder:

```c++
#include "systemc.h"
SC_MODULE(topadd2) {
  sc_in_clk clk;
  sc_in<int> in_port_1;
  sc_in<int> in_port_2;
  sc_out<int> out_port;

  SC_CTOR(topadd2) {
    SC_METHOD(topEntry);
    sensitive<<clk.pos();
  }

  void topEntry() {
    out_port.write(in_port_1.read() + in_port_2.read());
	}

};

int sc_main(int argc, char *argv[]){

  sc_clock CLOCK("clock", 5);
  sc_signal<int> input_1;
  sc_signal<int> input_2;
  sc_signal<int> output;
  topadd2 t1 ("t1");

  t1.clk(CLOCK);
  t1.in_port_1(input_1);
  t1.in_port_2(input_2);
  t1.out_port(output);

  sc_start(10, SC_NS);

  return 0;
}
```

Alternatively, you can also copy this file from `$SYSTEMC_CLANG`:
```
cp $SYSTEMC_CLANG/tests/data/verilog-conversion-custom/add/add.cpp .
```

This is the design file that we will be using in this documentation.

#### Converting add.cpp to add\_hdl.txt
As a part of the translation process, we can first translate the `add.cpp` into an intermediate representation, called `add_hdl.txt`.

Make a folder called `results` and the folder structure will be:
```
$ mkdir results && ls
add.cpp  results
```

Next, we convert add.cpp to add\_hdl.txt using our script:
```
$ python -B $SYSTEMC_CLANG/tests/verilog-conversion/run-compare.py \
     cpp-to-hdl \
     --output-dir ./results/  \
     --cpp add.cpp \
     --verbose
```

And the output should be similar to:
```
path:  add.cpp
sexp_loc:  add_hdl.txt
sexp_filename:  add_hdl.txt
output_filename:  /home/allen/working/systemc-clang-test-tool/results/2020-03-05_11-49-11/add_hdl.txt
cmd:  /home/allen/working/clang+llvm-9.0.0//bin/systemc-clang add.cpp -- -I /home/allen/working/systemc//include/ -std=c++14 -I /usr/include/ -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -x c++ -w -c -I .
The _hdl.txt file is written to: /home/allen/working/systemc-clang-test-tool/results/2020-03-05_11-49-11/add_hdl.txt
Using systemc-clang binary: /home/allen/working/clang+llvm-9.0.0//bin/systemc-clang
Using convert.py command: python /home/allen/working/systemc-clang//plugins/xlat/convert.py
```

Note that the `cmd: ...` shows the real command that is running under the hood.
Also, your time-stamp is likely different from what we have in the output above.

If we check the output folder, we would observe something similar to:
```
$ ls results/2020-03-05_11-49-11/
add_hdl.txt  systemc-clang.stderr  systemc-clang.stdout
```

Apart from the output file add\_hdl.txt, the standard output/standard error of the systemc-clang tool is also captured, which might come handy when debugging.

Now, copy the add\_hdl.txt file for later use, and the directory structure should be as follows:
```
$ cp results/2020-03-05_11-49-11/add_hdl.txt . && ls
add.cpp  add_hdl.txt  results
```

Apart from conversion, you can also specify a golden standard file with `--hdl` option and the tool will do a diff at the end of the conversion against the specified golden standard.

To try out this feature, make a directory called `golden`, copy our provided golden standard file for `add_hdl.txt` to the `golden` and confirm the directory structure:
```
$ mkdir golden && cp $SYSTEMC_CLANG/tests/data/verilog-conversion-custom/add/golden/add_hdl.txt golden/
$ ls .
add.cpp  golden  results
$ ls golden/
add_hdl.txt
```

Next, we run the tool again, with `--hdl` option enabled:
```
$ python -B $SYSTEMC_CLANG/tests/verilog-conversion/run-compare.py \
     cpp-to-hdl \
     --output-dir ./results/  \
     --cpp add.cpp \
     --verbose \
     --hdl ./golden/add_hdl.txt
```

If there is a difference between the generated file and the golden standard, the diff will be output to the screen, and also it will be stored in the time-stamped folder together with the \_hdl.txt file called diff.
```
$ ls results/2020-03-05_12-16-33/
add_hdl.txt  diff  systemc-clang.stderr  systemc-clang.stdout
```

The `hdl-to-v` works similarly and it converts \_hdl.txt file to Verilog file, we provide the following command example to show their usage.

#### Converting add_hdl.txt to add_hdl.txt.v
   ```
   python -B $SYSTEMC_CLANG/tests/verilog-conversion/run-compare.py \
     hdl-to-v \
     --output-dir results/ \
     --hdl results/2020-03-05_11-49-11/add_hdl.txt \
     --verbose
   ```
   Note that here the `--hdl` is specified with a previously generated `_hdl.txt` file.
   The generated file will be placed into a timestamped directory in `/tmp/`, for example, `/tmp/2020-02-17_01-28-52/sreg-driver.v`

#### Converting add_hdl.txt to add.v and show the diff of an existing Verilog file
   ```
   python -B $SYSTEMC_CLANG/tests/verilog-conversion/run-compare.py \
     hdl-to-v \
     --output-dir results/ \
     --hdl results/2020-03-05_11-49-11/add_hdl.txt \
     --verbose \
     --verilog $SYSTEMC_CLANG/tests/data/verilog-conversion-custom/add/golden/add_hdl.txt.v
   ```

Now, you should be able to use the `run-compare.py` script when doing development.

If you encounter problems when using the tool, please report back or file an issue, thank you!

---

We also provide ways in which you can make your development tests persisent in the repository, such as the `add.cpp` and `add_hdl.txt` files in the previous example.

## Prelimilaries

### CMake switch 
  Python tests are configured during the process of calling `cmake`, to enable the python tests, run `cmake` with `-DENABLE_VERILOG_TESTS=on` flag along with other build options when [building the binaries](/doc/README.md#Installation).
  
  For example, to enable the python Verilog tests along with XLAT, the `cmake` command will be similar to (make sure the `-DSYSTEMC_DIR` is correctly specified): 
  ```
  cmake ../systemc-clang -DXLAT=on -DENABLE_TESTS=on -DSYSTEMC_DIR=$SYSTEMC/ -G Ninja -DENABLE_VERILOG_TESTS=on
  ```

  If the python interpreter is found, `cmake` should report lines similar to:
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
  To run the Verilog tests, switch to `$SYSTEMC_CLANG_BUILD_DIR`, and make sure `systemc-clang` is built.
  Then run:
  ```
  ctest -R verilog-tests
  ``` 
  If the tests do not pass, running the following command will provide more information on what command is called:
  ```
  ctest -R verilog-tests --verbose
  ```

  The tests might not all pass at the moment, we provide a set of sanity tests that should pass at this point:
  ```
  ctest -R verilog-sanity-tests --verbose
  ```

## Running all tests manually
  It is also possible to run the tests manually from python scripts for fine-grained control over what to test.

  To run all the Verilog tests manually, in `$SYSTEMC_CLANG_BUILD_DIR`:
  ```
  python -B run-verilog-tests.py
  ```

### Individual tests
  It is possible to run individual python tests to observe the output with `-o` or `--only` option.
  For example, in `$SYSTEMC_CLANG_BUILD_DIR`:
  ```
  python -B run-verilog-tests.py --only test_sanity_add_sexp_to_verilog
  ```
### List all tests
  It is possible to list all tests that can be supplied for the `-o` argument for `run-verilog-tests.py`.
  To list the number available tests in each file, in `$SYSTEMC_CLANG_BUILD_DIR`, run:
  ```
  python -B run-verilog-tests.py --collect-only
  ```
  For a list of all available tests, use `-v` option:
  ```
  python -B run-verilog-tests.py --collect-only -v
  ```
  The returned list contains the filename in which the tests are defined, followed by `::` and the corresponding test names.

  Note that when providing name to `-o`, only provide the name after `::`.

  For example, in order to run one of the sanity tests:
  ```
  $ python -B run-verilog-tests.py --collect-only -v
  ...
  systemc-clang/tests/verilog-conversion/test_sanity.py::test_sanity_add_sexp
  systemc-clang/tests/verilog-conversion/test_sanity.py::test_sanity_add_verilog
  systemc-clang/tests/verilog-conversion/test_sanity.py::test_sanity_add_sexp_to_verilog
  ...
  $ python -B run-verilog-tests.py --only test_sanity_add_sexp_to_verilog
  =================================== test session starts ===================================
  platform linux -- Python 3.7.4, pytest-5.0.1, py-1.8.1, pluggy-0.13.1
  rootdir: /home/ubuntu/working
  collected 55 items / 54 deselected / 1 selected

  ../systemc-clang/tests/verilog-conversion/test_sanity.py .                          [100%]

  ========================= 1 passed, 54 deselected in 0.35 seconds =========================
  ```

## Adding tests
  New tests can be added to `$SYSTEMC_CLANG/tests/data/verilog-conversion-custom/`, and they will be recognized automatically by the pytest, without the need to modify the script.

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
  ______test_custom_sexp_to_verilog[add] ______

  tmpdir = local('/tmp/pytest-of-allen/pytest-161/test_custom_sexp_to_verilog_ad0'), customdriver = <driver.SystemCClangDriver object at 0x7f8750d6b910>
  tool_output = False
  ...
  ```
  In the output, `tmpdir = local('/tmp/pytest-of-allen/pytest-161/test_custom_sexp_to_verilog_ad0')` logs the temporary folder where the intermediate output are stored.
  And thus we can observe the output in `/tmp/pytest-of-allen/pytest-161/test_custom_sexp_to_verilog_ad0/`

---
