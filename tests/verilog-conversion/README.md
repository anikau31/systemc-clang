<!-- The scripts mentioned in this folder should be in sync with the tests/verilog-conversion/run_example.sh so that the commands can be tested -->
# Python Tests for Verilog Conversion and Utilites for Development

## Preliminaries

**Before going through this documentation, make sure you have built the systemc-clang successfully, this means that the `scripts/paths.sh` script is sourced appropriately.** 
In addition, ensure that you are using [Release 2.2.0](https://github.com/anikau31/systemc-clang/releases/tag/2.2.0).

### Repository path
There are a few key environment variables that must be set. So, ensure that they are correctly set after executing `scripts/path.sh`. These environment variables are: `SYSTEMC_CLANG_BUILD_DIR` and `SYSTEMC_CLANG`.
We use the environment variable `$SYSTEMC_CLANG` to specify the systemc-clang git repository directory.  This is the source directory.

```bash
$ ls $SYSTEMC_CLANG
cmake/  CMakeLists.txt  doc/  driver-tooling.cpp  driver-xlat.cpp  examples/  externals/  LICENSE  Makefile.systemc  plugins/  README.md  README.rst  requirements.txt  scripts/  src/  tests/  tests-old/
```

### Python dependencies

#### Python version
We use Python 3 in this documentation.
```
$ python3 --version
Python 3.7.5
```

#### Dependencies
- We use certain Python packages for the testing infrastructure and the translation to Verilog. These packages are listed in `requirements.txt`.  To install them, run
```bash
$ pip3 install -r $SYSTEMC_CLANG/requirements.txt
``` 

## The conversion script

We provide command-line scripts as shortcuts to generate different types of targets within the flow, as shown in the following examples.

### Generating SystemVerilog from C++

Use the `systemc-clang.py` script with the same parameters passed to the `systemc-clang` binary and both the `_hdl.txt` and the SystemVerilog file will be created in the same directory as the source C++ file.
Notice that you may also provide include directory for LLVM as well as SystemC, but the script can also pickup these directories from the environment variables.

```bash
$ ls /tmp/test
add.cpp

$ scripts/run-cpp-sv.sh /tmp/test/add.cpp
...

$ ls /tmp/test
add.cpp  add_hdl.txt  add_hdl.txt.v
```

### Generating `_hdl.txt`

Use the `systemc-clang` binary to generate `_hdl.txt` file from C++. 
Notice that you need to specify the LLVM directory as well as the SystemC directory.
```bash
$ ls /tmp/test
add.cpp

$ scripts/run.sh /tmp/test/add.cpp

$ ls /tmp/test
add.cpp  add_hdl.txt
```

### Generating SystemVerilog from `_hdl.txt`

Use the `hcode2verilog.py` script to translate `_hdl.txt` into SystemVerilog file.
```bash
$ ls /tmp/test
add_hdl.txt

$ scripts/run-hcode-sv.sh ./add_hdl.txt
...

$ ls /tmp/test
add_hdl.txt  add_hdl.txt.v
```


## The conversion tool

We provide a command-line tool that helps the development process for testing.
The tool does the following.

- Convert the SystemC model to an intermediate file with the suffix `_hdl.txt`.  The resulting file has the intermediate representation from which we synthesize Verilog.
- Convert the file suffixed with `_hdl.txt` to Verilog.
- The conversion result will be stored in time-stamped folders

The tool is located at `$SYSTEMC_CLANG/tests/verilog-conversion/run-compare.py`.  It can be invoked as follows.
```bash
$ python3 $SYSTEMC_CLANG/tests/verilog-conversion/run-compare.py -h
usage: A tool for running and comparing against a golden standard
       [-h] [--cpp CPP] [--hdl HDL]
       [--golden-intermediate GOLDEN_INTERMEDIATE]
       [--golden-verilog GOLDEN_VERILOG]
       [--include-path [INCLUDE_PATH [INCLUDE_PATH ...]]]
       [--output-dir OUTPUT_DIR] [--verbose]
       {cpp-to-hdl,hdl-to-v,cpp-to-v}
       ...
 ```

Each of the option is explained as follows:
- The positional argument takes `cpp-to-hdl`, `hdl-to-v` or `cpp-to-v`.  
`cpp-to-v` converts a SystemC design to Verilog.  
`hdl-to-v` converts an intermediate file to Verilog.  
`cpp-to-hdl` converts a SystemC design to an intermediate file.  
- `-h` prints help message.
- `--cpp` specifies the SystemC design C++ file and is only valid when choosing `cpp-to-hdl` or `cpp-to-v`.
- `--hdl` specifies the intermediate file and is only valid when choosing `hdl-to-v`.
- `--golden-intermediate` specifies a golden intermediate file to check for the differences between the golden file and the output and is only valid when choosing `cpp-to-hdl` or `cpp-to-v`.
- `--golden-verilog` specifies a golden Verilog file to check for the differences between the golden file and the output and is only valid when choosing `hdl-to-v` or `cpp-to-v`.
- `--include-path` specifies include paths for the SystemC design.
- `--output-dir` sepecifies the output directory to place the time-stamped output.
- `--verbose` enables detailed output.



### Examples
In this section, we provide examples on how to use the testing tool.

Firstly, create an empty directory outside `$SYSTEMC_CLANG_BUILD_DIR` and then switch to it:

```bash
$ mkdir systemc-clang-test-tool && cd systemc-clang-test-tool/
```

Next, in the directory, create a file called `add.cpp` with the following content, note that this is a SystemC adder module:

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

Alternatively, you can also copy this example file from `$SYSTEMC_CLANG`:
```bash
$ cp $SYSTEMC_CLANG/tests/verilog-conversion/data/add.cpp .
```

This is the SystemC model that we will be using in this documentation.

#### Converting add.cpp to add\_hdl.txt
As a part of the translation process, we first translate the `add.cpp` into an intermediate representation resulting in `add_hdl.txt`.

Make a folder called `results` and the folder structure will be:
```bash
$ mkdir results && ls
add.cpp  results
```

Next, we convert `add.cpp` to `add_hdl.txt` using our script:
```bash
$ python3 -B $SYSTEMC_CLANG/tests/verilog-conversion/run-compare.py \
     cpp-to-hdl \
     --output-dir ./results/  \
     --cpp add.cpp \
     --verbose
```
Note that the `-B` flag prevents `.pyc` file generation, which keeps the directory clean.

The output should be similar to:
```
path: add.cpp
/systemc-clang-build/systemc-clang add.cpp --debug -- -x c++ -w -c -std=c++17 _-D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DRVD -I /opt/clang-11.0.0/lib/clang/11.0.0/include -I /opt/systemc-2.3.3/include
sexp_loc: add_hdl.txt
sexp_filename: add_hdl.txt
The _hdl.txt file is written to: /tmp/tmp.kUUW0fiyam/systemc-clang-test-tool/results/2021-01-13_06-02-11/add_hdl.txt
```

Note that the `cmd: ...` shows the real command that is running within the script. 
Also, your time-stamp is likely different from what we have in the output above.

If we check the output folder, we observe something similar to:
```bash
$ ls -R
.:
add.cpp  results

./results:
2021-01-13_06-26-00

./results/2021-01-13_06-26-00:
add_hdl.txt.txt
```
Apart from the conversion, one can specify a golden standard file with `--golden-intermediate` option, and the tool will do a diff at the end of the conversion against the specified golden standard.

For example, create a directory called `golden`, copy our provided golden standard file `add_hdl.txt` to the `golden` and confirm the directory structure:
```bash
$ mkdir golden && cp $SYSTEMC_CLANG/tests/verilog-conversion/data/add_hdl.txt golden/
$ ls .
add.cpp  golden  results
$ ls golden/
add_hdl.txt
```

Next, run the tool again, with `--golden-intermediate` option enabled:
```
$ python3 -B $SYSTEMC_CLANG/tests/verilog-conversion/run-compare.py \
     cpp-to-hdl \
     --output-dir ./results/  \
     --cpp add.cpp \
     --verbose \
     --golden-intermediate ./golden/add_hdl.txt
```

If the generated file and the golden standard file are different, the diff will be presented to the console, and will be stored in the time-stamped folder, together with the generated `add_hdl.txt` file.
```
$ ls -R
.:
add.cpp  golden  results

./golden:
add_hdl.txt

./results:
...  2021-01-13_22-36-23

...

./results/2021-01-13_22-36-23:
add_hdl.txt  diff.hdl
```

If the generated file and the golden standard file are the same, the diff will not output to the screen.

The `hdl-to-v` works similarly and it converts `_hdl.txt` file to Verilog file. 
The `_hdl.txt` file is supplied with the option `--hdl`.

#### Converting add_hdl.txt to add_hdl.txt.v
   ```bash
   python3 -B $SYSTEMC_CLANG/tests/verilog-conversion/run-compare.py \
     hdl-to-v \
     --output-dir results/ \
     --hdl golden/add_hdl.txt \
     --verbose
   ```
   Note that here the `--hdl` is specified with the golden `_hdl.txt` file.
   The generated file will be placed into a timestamped directory in `/tmp/`, for example, `/tmp/2020-02-17_01-28-52/sreg-driver.v`

#### Converting add_hdl.txt to add.v and show the diff of an existing Verilog file
   ```
   python3 -B $SYSTEMC_CLANG/tests/verilog-conversion/run-compare.py \
     hdl-to-v \
     --output-dir results/ \
     --hdl golden/add_hdl.txt \
     --verbose \
     --golden-verilog $SYSTEMC_CLANG/tests/verilog-conversion/add_hdl.txt.v
   ```

Now, you should be able to use the `run-compare.py` script when doing development.

If you encounter problems when using the tool, please report back or file an issue, thank you!

---

We also provide ways in which you can make your development tests persisent in the repository, such as the `add.cpp` and `add_hdl.txt` files in the previous example.

In the test infrastructure, the same underlying code is used for `run-compare.py`.

## Running the tests

To run the python tests, you need to be in `$SYSTEMC_CLANG_BUILD_DIR`:
```bash
$ cd $SYSTEMC_CLANG_BUILD_DIR
```

Next, if you have already built systemc-clang successfully, you may need to remove the files generated by CMake. This is only to ensure that the new flags that are passed in the next step actually take effect.
```bash
$ rm -rf ./CMakeCache.txt CMakeFiles/
```
Now, we run `cmake` again to make sure the appropriate files are copied to `$SYSTEMC_CLANG_BUILD_DIR`.
To enable the conversion tests, append  `-DENABLE_VERILOG_TESTS=on` to the CMake command.
For example:
```bash
$ cmake ../systemc-clang -DENABLE_TESTS=on -DXLAT=on -DENABLE_VERILOG_TESTS=on
...
-- Submodule update
...
-- [ Tests         ]
...
-- Build Verilog tests   : on
...
-- Python found          : TRUE
-- Python interpreter    : /home/allen/anaconda3/bin/python3
...
```

Make sure that the output contains the presented lines, which include the path to the Python interpreter.

Then, build systemc-clang by invoking `make` or `ninja`:

```bash
$ make
```

Now you should be prepared to run the tests, try the following command:

```bash
$ python3 -B -m pytest -v --color=yes $SYSTEMC_CLANG/tests -k examples
==================================================== test session starts =====================================================
platform linux -- Python 3.8.5, pytest-5.0.1, py-1.10.0, pluggy-0.13.1 -- /usr/bin/python3
cachedir: .pytest_cache
rootdir: /systemc-clang/tests
plugins: steps-1.7.3
collected 6 items / 2 deselected / 4 selected

../systemc-clang/tests/verilog-conversion/test_examples.py::test_translation[add-hcode] PASSED                         [ 25%]
../systemc-clang/tests/verilog-conversion/test_examples.py::test_translation[add-translation] PASSED                   [ 50%]
../systemc-clang/tests/verilog-conversion/test_examples.py::test_translation[add-translation-match-check] PASSED       [ 75%]
../systemc-clang/tests/verilog-conversion/test_examples.py::test_translation[add-synthesis] SKIPPED                    [100%]

===================================== 3 passed, 1 skipped, 2 deselected in 1.03 seconds ======================================
```

Notice the `add` prefix in `[add-hcode], [add-translation], [add-translation-match-check]` and `[add-synthesis]`. 
These are the names of the **test data** of the design.
At a high level, when a user needs to add a test to the framework, this user provides the necessary files, or **test data**, for each SystemC design and organizes them in the `$SYSTEMC_CLANG` directory in a certain way.

The **test data** is composed of three parts. 
1. A SystemC model file
2. A golden standard `_hdl.txt` file that is generated by XLAT
3. A golden standard Verilog file that is generated by hcode2verilog.py

In our example above, we provided the **test data** for one designs: `add`.

To observe how these files are organized, you can see the folder `$SYSTEMC_CLANG/tests/verilog-conversion/data/` (Note: you may not have the `tree` command installed, but you can still verify the directory structure): 
```bash
$ tree $SYSTEMC_CLANG/tests/verilog-conversion/data
...
├── add.cpp
├── add_hdl.txt
└── add_hdl.txt.v
```

Take the `add` for example, its **test data** include a SystemC design file `add.cpp`, a golden stardard for \_hdl.txt `add_hdl.txt` and a golden standard for Verilog `add_hdl.txt.v`.

We will show how to add your tests later.
But now, let's go back to the output of the test that we just run and focus only on the `add` to see what it is testing:
```
...
test_examples.py::test_translation[add-hcode] PASSED                         [ 25%]
test_examples.py::test_translation[add-translation] PASSED                   [ 50%]
test_examples.py::test_translation[add-translation-match-check] PASSED       [ 75%]
test_examples.py::test_translation[add-synthesis] SKIPPED                    [100%]
...
```

Within the test framework, we provide a simple way for a user to add tests for the following purpose:
1. Test the conversion from a SystemC design file to \_hdl.txt and compare the generated \_hdl.txt against a golden standard, which corresponds to the `test_translation[add-hcode]`
2. Test the conversion from \_hdl.txt to Verilog without triggering any exception, which corresponds to `test_translation[add-translation]`.
3. Compare the generated Verilog against a golden standard, which corresponds to the `test_translation[add-translation-match-check]`
4. Test that the translated Verilog is synthesizable, which corresponds to `test_translation[add-synthesis]`. Note that currently we check the synthesizability only when the Vivado tool is found in the environmental variables.

## Adding your own test

We describe how a user can add a test to the framework.
We will make another module to test: a `sub` module that takes two inputs and outputs their difference by doing subtraction.
We assume that all commands done in this section are done in the path `$SYSTEMC_CLANG_BUILD_DIR`.

To add a test, first add test spec in the `test_data` list in `$SYSTEMC_CLANG/tests/conftest.py`:
```python
test_data = [
    ┊   ('add',    load_file(testdata / 'add.cpp'), None, load_file(testdata / 'add_hdl.txt.v')),
    ┊   ('sub',    load_file(testdata / 'sub.cpp'), None, load_file(testdata / 'sub_hdl.txt.v')),  # the added new spec
    ...
]
```

A spec is composed of a four element tuple: `(name, design, extra_args, golden)` where `name` is the name of the test, `design` is the C++ design, `extra_args` are extra arguments passed to the systemc-clang binary such as extra include directories and `golden` is the golden Verilog file to be checked against.

We provide handy variables/helper functions such as `load_file` to load the content from a file and `testdata` to refer to `$SYSTEMC_CLANG/tests/verilog-conversion/data`.
The definition of these helpers can be found on top of the `conftest.py` file.

Now, we run the tests again:
```bash
$ python3 -B -m pytest -v --color=yes $SYSTEMC_CLANG/tests -k examples
...
/systemc-clang/tests/verilog-conversion/conftest.py:38: in <module>
    ('sub',    load_file(testdata / 'sub.cpp'), None, load_file(testdata / 'sub_hdl.txt.v')),
/systemc-clang/tests/verilog-conversion/conftest.py:27: in load_file
    with open(path, 'r') as f:
E   FileNotFoundError: [Errno 2] No such file or directory: '/systemc-clang/tests/verilog-conversion/data/sub.cpp'
...

```

We can observe that the tests are failing because we have not added the `sub` design.
This is true because we did not add any SystemC model file.  Now we create `$SYSTEMC_CLANG/tests/verilog-conversion/data/sub.cpp` with the following content:
```c++
// sub.cpp
#include "systemc.h"
SC_MODULE(topsub2) {
  sc_in_clk clk;
  sc_in<int> in_port_1;
  sc_in<int> in_port_2;
  sc_out<int> out_port;

  SC_CTOR(topsub2) {
  ┊ SC_METHOD(topEntry);
  ┊ sensitive<<clk.pos();
  }

  void topEntry() {
  ┊ out_port.write(in_port_1.read() - in_port_2.read());
  }

};

int sc_main(int argc, char *argv[]){

  sc_clock CLOCK("clock", 5);
  sc_signal<int> input_1;
  sc_signal<int> input_2;
  sc_signal<int> output;
  topsub2 t1 ("t1");

  t1.clk(CLOCK);
  t1.in_port_1(input_1);
  t1.in_port_2(input_2);
  t1.out_port(output);

  sc_start(10, SC_NS);

  return 0;
}

```

We then use the golden standard of the `add` module, which is incorrect for `sub`, to show how to interpret the error information.
```bash
$ cp $SYSTEMC_CLANG/tests/verilog-conversion/data/{add,sub}_hdl.txt.v
```

Now the `$SYSTEMC_CLANG/tests/verilog-conversion/data` folder looks similar to:
```bash
$ tree $SYSTEMC_CLANG/tests/verilog-conversion/data
├── add.cpp
├── add_hdl.txt
├── add_hdl.txt.v
├── sub.cpp
└── sub_hdl.txt.v
```

Now we run the test again and can observe that the `sub` tests are present and `[sub-translation-match-check]` is failing:
```bash
$ python3 -B -m pytest -v --color=yes $SYSTEMC_CLANG/tests -k examples
...
test_examples.py::test_translation[sub-hcode] PASSED                                         [ 62%]
test_examples.py::test_translation[sub-translation] PASSED                                   [ 75%]
test_examples.py::test_translation[sub-translation-match-check] FAILED                       [ 87%]
test_examples.py::test_translation[sub-synthesis] SKIPPED                                    [100%]
...
_____ test_translation[sub-translation-match-check] _____

...
_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _

tmp_path = PosixPath('/tmp/pytest-of-root/pytest-13/test_translation_sub_hcode_0'), name = 'sub'
...
>               assert False, '\nTranslated file and golden file mismatch, diff result:\n' + ''.join(diff_res)
E               AssertionError:
E                 Translated file and golden file mismatch, diff result:
E                   module _sc_module_0 (
E                     input logic [0:0] clk,
E                     input logic signed[31:0] in_port_1,
E                     input logic signed[31:0] in_port_2,
E                     output logic signed[31:0] out_port
E                   );
E                     initial begin
E
E                     end
E                     always @(posedge clk) begin: topEntry
E
E
E                 -     out_port <= (in_port_1) + (in_port_2);
E                 ?                             ^
E                 +     out_port <= (in_port_1) - (in_port_2);
E                 ?                             ^
E                     end
E                   endmodule
E               assert False

/systemc-clang/tests/verilog-conversion/test_examples.py:63: AssertionError

...
```

Now, the test fails because the operation symbol does not match.

Here, we can fix the Verilog golden standard file by replacing `+` with `-`.
The change is made in `$SYSTEMC_CLANG/tests/verilog-conversion/data/sub_hdl.txt.v`

Also, pay attention to the line where it says `tmp_path`, it logs the temporary directory where the program output are stored:
```bash
$ ls /tmp/pytest-of-root/pytest-13/test_translation_sub_hcode_0
sub.cpp  sub_hdl.txt  sub_hdl.txt.v
```
These files are useful for debugging purpose.


After making changes to the golden standard file, run the the test again and it should pass:
```bash
$ python3 -B -m pytest -v --color=yes $SYSTEMC_CLANG/tests -k examples
...
test_examples.py::test_translation[add-hcode] PASSED                                         [ 12%]
test_examples.py::test_translation[add-translation] PASSED                                   [ 25%]
test_examples.py::test_translation[add-translation-match-check] PASSED                       [ 37%]
test_examples.py::test_translation[add-synthesis] SKIPPED                                    [ 50%]
test_examples.py::test_translation[sub-hcode] PASSED                                         [ 62%]
test_examples.py::test_translation[sub-translation] PASSED                                   [ 75%]
test_examples.py::test_translation[sub-translation-match-check] PASSED                       [ 87%]
test_examples.py::test_translation[sub-synthesis] SKIPPED                                    [100%]
...
```

Now, we have covered the basics of adding a test to the test framework.


# Appendix

## Other tests

There are other tests as well when running the python scripts.
You can view all the tests using the following script:
```bash
$ python3 -B -m pytest -v --color=yes $SYSTEMC_CLANG/tests --collect-only -v
collected 10 items
<Module verilog-conversion/test_examples.py>
  <Function test_translation[add-hcode]>
  <Function test_translation[add-translation]>
  <Function test_translation[add-translation-match-check]>
  <Function test_translation[add-synthesis]>
  <Function test_translation[sub-hcode]>
  <Function test_translation[sub-translation]>
  <Function test_translation[sub-translation-match-check]>
  <Function test_translation[sub-synthesis]>
<Module verilog-conversion/test_systemc_clang_wrapper.py>
  <Function test_syntax_error>
  <Function test_generate_file>
...
```

Some of the tests are for sanity check, to make sure that the tools are set-up correctly.
