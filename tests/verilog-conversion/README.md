# Python Tests for Verilog Conversion and Utilites for Development

## Preliminaries

**Before going through this documentation, make sure you have built the systemc-clang successfully, this implies that the `scripts/paths.sh` is sourced.**

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

Firstly, create an empty directory outside `$SYSTEMC_CLANG_BUILD_DIR` and then switch to it:

```
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

If the generated file and the golden standard file are different, the diff will be output to the screen and will be stored in the time-stamped folder, together with the generated \_hdl.txt file.
```
$ ls results/2020-03-05_12-16-33/
add_hdl.txt  diff  systemc-clang.stderr  systemc-clang.stdout
```

If the generated file and the golden standard file are the same, the diff will not be present.

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

In the test infrastructure, the same underlying code is used for `run-compare.py`.

## Running the tests

To run the python tests, you need to be in `$SYSTEMC_CLANG_BUILD_DIR`:
```
$ cd $SYSTEMC_CLANG_BUILD_DIR
```

Next, if you have already built systemc-clang successfully, you may need to remove the files generated by CMake:
```
$ rm -rf ./CMakeCache.txt CMakeFiles/
```

Now, we will run `cmake` again to make sure the appropriate files are copied to `$SYSTEMC_CLANG_BUILD_DIR`.
To enable the conversion tests, append  `-DENABLE_VERILOG_TESTS=on` to the CMake command.
For example:
```
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

```
$ make
```

Now you should be prepared to run the tests, try the following command:

```
$ python -B run-verilog-tests.py -o test_custom -v
================================================ test session starts =================================================
platform linux -- Python 3.7.5, pytest-5.0.1, py-1.8.0, pluggy-0.13.1 -- /home/allen/anaconda3/envs/systemc-clang/bin/python
cachedir: .pytest_cache
rootdir: /home/allen/working
collected 55 items / 49 deselected / 6 selected

... /test_custom.py::test_custom_sexp[xor] PASSED                         [ 16%]
... /test_custom.py::test_custom_sexp[add] PASSED                         [ 33%]
... /test_custom.py::test_custom_verilog[xor] PASSED                      [ 50%]
... /test_custom.py::test_custom_verilog[add] PASSED                      [ 66%]
... /test_custom.py::test_custom_sexp_to_verilog[xor] PASSED              [ 83%]
... /test_custom.py::test_custom_sexp_to_verilog[add] PASSED              [100%]

====================================== 6 passed, 49 deselected in 2.61 seconds =======================================
```

For simplicity, the output does not show the full path of the tests, which should be straight forward.

In this command, we are using the `run-verilog-tests.py` script, which provide an interface for running tests.
The `-o` option selects the tests whose names match the specified value to run.
In this case, all tests with a name containing `test_custom` will be run.
And `-v` shows detailed output.

You might notice the `[xor]` and `[add]` prefix, these are the name of the **test data** of the design.
At a high level, when a user needs to add a test to the framework, this user provides necessary files, or **test data**, at per SystemC design level and organizes them in the `$SYSTEMC_CLANG` directory in a certain way.

The **test data** is composed of three part: 
1. A SystemC C++ file
2. A golden standard \_hdl.txt file that is generated by XLAT
3. A golden standard Verilog file that is generated by convert.py

In our example above, we provided the **test data** for two designs: `add` and `xor`.

To observe how these data are organized, you can observe the folder `$SYSTEMC_CLANG/tests/data/verilog-conversion-custom/` (Note: you may not have the `tree` command installed, but you can still verify the directory structure): 
```
$ tree $SYSTEMC_CLANG/tests/data/verilog-conversion-custom/
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

Take the `add` for example, its **test data** include a SystemC design file `add/add.cpp`, a golden stardard for \_hdl.txt `add/golden/add_hdl.txt` and a golden standard for Verilog `add/golden/add_hdl.txt.v`.

We will introduce how to add your test later.
But now, let's go back to the output of the test that we just run and focus only on the `add` to see what it is testing:
```
...
... /test_custom.py::test_custom_sexp[add] PASSED                         [ 33%]
...
... /test_custom.py::test_custom_verilog[add] PASSED                      [ 66%]
...
... /test_custom.py::test_custom_sexp_to_verilog[add] PASSED              [100%]

```

Within the test framework, we provide a simple way for a user to add tests for the following purpose:
1. Test the conversion from a SystemC design file to \_hdl.txt and compare the generated \_hdl.txt against a golden standard, which corresponds to the `test_custom_sexp[add]`
2. Test the conversion from \_hdl.txt to Verilog and compare the generated Verilog against a golden standard, which corresponds to the `test_custom_verilog[add]`
3. Test the conversion from a SystemC design file to Verilog and compared the generated \_hdl.txt and Verilog to golden standard files, which corresponds to the `test_custom_sexp_to_vereilog[add]`

## Adding your own test

Next, we introduce how a user adds a test to the framework.
We will make another module to test: a `sub` module that takes two input and outputs their difference by doing subtraction.

To add a test, create another folder in `$SYSTEMC_CLANG/tests/data/verilog-conversion-custom/`:
```
mkdir $SYSTEMC_CLANG/tests/data/verilog-conversion-custom/sub
```

> If you run the test now with `python -B run-verilog-tests.py -o test_custom -v`, you will not see the difference and there will only be tests for `xor` and `add`

Next re-run `cmake` to make sure the tests are copied. Note that you don't need to remove `CMakefiles/` and `CMakeCache.txt`.
```
$ cd $SYSTEMC_CLANG_BUILD_DIR
$ rm -rf tests/data/verilog-conversion-custom/
$ cmake ../systemc-clang -DENABLE_TESTS=on -DXLAT=on -DENABLE_VERILOG_TESTS=on
```

And now, we run the tests again with:
```
$ python -B run-verilog-tests.py -o test_custom -v
...
... /test_custom.py::test_custom_sexp[xor] PASSED          [ 11%]
... /test_custom.py::test_custom_sexp[sub] FAILED          [ 22%]
... /test_custom.py::test_custom_sexp[add] PASSED          [ 33%]
... /test_custom.py::test_custom_verilog[xor] PASSED       [ 44%]
... /test_custom.py::test_custom_verilog[sub] FAILED       [ 55%]
... /test_custom.py::test_custom_verilog[add] PASSED       [ 66%]
... /test_custom.py::test_custom_sexp_to_verilog[xor] PASSED [ 77%]
... /test_custom.py::test_custom_sexp_to_verilog[sub] FAILED [ 88%]
... /test_custom.py::test_custom_sexp_to_verilog[add] PASSED [100%]
...

```

We can observe from the **start of the log** that the `[sub]` test is added, but it is failing.

The error information is logged after the tests fail and is divided by the name of the tests, which are colored red in terminal:

```
...
============================================== FAILURES ===============================================
________________________________________ test_custom_sexp[sub] ________________________________________

tmpdir = local('/tmp/pytest-of-allen/pytest-8/test_custom_sexp_sub_0')
customdriver = <driver.SystemCClangDriver object at 0x7fd15e49bd50>, tool_output = False
...
______________________________________ test_custom_verilog[sub] _______________________________________

tmpdir = local('/tmp/pytest-of-allen/pytest-8/test_custom_verilog_sub_0')
customdriver = <driver.SystemCClangDriver object at 0x7fd15c43af50>, tool_output = False

...
__________________________________ test_custom_sexp_to_verilog[sub] ___________________________________

tmpdir = local('/tmp/pytest-of-allen/pytest-8/test_custom_sexp_to_verilog_su0')
customdriver = <driver.SystemCClangDriver object at 0x7fd15c486bd0>, tool_output = False
...
========================== 3 failed, 6 passed, 49 deselected in 2.68 seconds ==========================
...
```

The error information of each tests provides some information about how the test fails but that is test-specific.

We now look at the `test_custom_sexp[sub]`. To run this test only, use the following command:
```
$ python -B run-verilog-tests.py -o test_custom_sexp[sub] -v
=====================test session starts ================
platform linux -- Python 3.7.5, pytest-5.0.1, py-1.8.0, pluggy-0.13.1 -- /home/allen/anaconda3/envs/systemc-clang/bin/python
cachedir: .pytest_cache
rootdir: /home/allen/working
collected 58 items / 57 deselected / 1 selected

../systemc-clang/tests/verilog-conversion/test_custom.py::test_custom_sexp[sub] FAILED          [100%]

========================FAILURES ========================
_________________test_custom_sexp[sub] __________________

tmpdir = local('/tmp/pytest-of-allen/pytest-9/test_custom_sexp_sub_0')
customdriver = <driver.SystemCClangDriver object at 0x7f29f2ced090>, tool_output = False

    def test_custom_sexp(tmpdir, customdriver, tool_output):
        conf = customdriver.conf
        test_name = conf.test_name
        filename = conf.get_module_name('{}.cpp'.format(test_name))
        output_folder = tmpdir
>       copy(filename, output_folder)

../systemc-clang/tests/verilog-conversion/test_custom.py:13:
...
../../anaconda3/envs/systemc-clang/lib/python3.7/shutil.py:248: in copy
    copyfile(src, dst, follow_symlinks=follow_symlinks)
  
  ...

src = '/home/allen/working/systemc-clang-build//tests/data/verilog-conversion-custom/sub//sub.cpp'
dst = '/tmp/pytest-of-allen/pytest-9/test_custom_sexp_sub_0/sub.cpp'

    def copyfile(src, dst, *, follow_symlinks=True):
        """Copy data from src to dst.

        If follow_symlinks is not set and src is a symbolic link, a new
        symlink will be created instead of copying the file it points to.

        """
        if _samefile(src, dst):
            raise SameFileError("{!r} and {!r} are the same file".format(src, dst))

        for fn in [src, dst]:
            try:
                st = os.stat(fn)
            except OSError:
                # File most likely does not exist
                pass
            else:
                # XXX What about other special files? (sockets, devices...)
                if stat.S_ISFIFO(st.st_mode):
                    raise SpecialFileError("`%s` is a named pipe" % fn)

        if not follow_symlinks and os.path.islink(src):
            os.symlink(os.readlink(src), dst)
        else:
>           with open(src, 'rb') as fsrc:
E           FileNotFoundError: [Errno 2] No such file or directory: '/home/allen/working/systemc-clang-build//tests/data/verilog-conversion-custom/sub//sub.cpp'

../../anaconda3/envs/systemc-clang/lib/python3.7/shutil.py:120: FileNotFoundError
```

On the last lines of the output, it indicates that the file `home/allen/working/systemc-clang-build//tests/data/verilog-conversion-custom/sub//sub.cpp` is not found.

This is true because we did not add any SystemC file, now we create `$SYSTEMC_CLANG/tests/data/verilog-conversion-custom/sub/sub.cpp` with the following content:

```
// sub.cpp
#include "systemc.h"
SC_MODULE(topsub2) {
  sc_in_clk clk;
  sc_in<int> in_port_1;
  sc_in<int> in_port_2;
  sc_out<int> out_port;

  SC_CTOR(topadd2) {
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

Next **run `cmake`** and run the the test again:
```
$ cmake ../systemc-clang -DENABLE_TESTS=on -DXLAT=on -DENABLE_VERILOG_TESTS=on
$ python -B run-verilog-tests.py -o test_custom_sexp[sub] -v
...
_________________test_custom_sexp[sub] ___________________

tmpdir = local('/tmp/pytest-of-allen/pytest-11/test_custom_sexp_sub_0')
customdriver = <driver.SystemCClangDriver object at 0x7f2eeaea21d0>, tool_output = False

...

../systemc-clang/tests/verilog-conversion/test_custom.py:25:

this_filename = '/tmp/pytest-of-allen/pytest-11/test_custom_sexp_sub_0/sub_hdl.txt'
that_filename = '/home/allen/working/systemc-clang-build//tests/data/verilog-conversion-custom/sub/golden//sub_hdl.txt'

    def sexpdiff(this_filename, that_filename):
        # Test the equality of golden versus the generated
        with open(this_filename, 'r') as f:
            sexp_lines = f.readlines()
>       with open(that_filename, 'r') as f:
E       FileNotFoundError: [Errno 2] No such file or directory: '/home/allen/working/systemc-clang-build//tests/data/verilog-conversion-custom/sub/golden//sub_hdl.txt'

../systemc-clang/tests/verilog-conversion/util/sexpdiff.py:8: FileNotFoundError
```

This time, the file that is not found is the golden standard \_hdl.txt file, as the information states: `E       FileNotFoundError: [Errno 2] No such file or directory: '/home/allen/working/systemc-clang-build//tests/data/verilog-conversion-custom/sub/golden//sub_hdl.txt'`.





# Appendix

## Other tests

There are other tests as well when running the python scripts.

Some of the tests are for sanity check, to make sure that the tools are set-up correctly.
Some of the tests are for the example folder because they don't follow certain naming convention and the need different driver for detecting modules.



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

### Done

  Note that before calling the `cmake`, you should already source the `script/paths.sh` and after **building the binaries**, the `systemc-clang` is generated in `$LLVM_INSTALL_DIR/bin`.

  The following steps will be dependent on the environment variables defined in the `script/paths.sh` to search for the binaries and the python scripts.

  We use `$SYSTEMC_CLANG_BUILD_DIR` to refer to the build directory created for `cmake` and `$SYSTEMC_CLANG` to refer to the git repository directory.


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

  Note that when providing the name to `-o`, only provide the name **after** `::`.

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
  New tests can be added to `$SYSTEMC_CLANG/tests/data/verilog-conversion-custom/`, and they will be recognized automatically by the pytest, without the need to modify the script or updating the submodules.

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
  
  **After making changes, re-run** `cmake` **to sync the changes from** `$SYSTEMC_CLANG` **to** `$SYSTEMC_CLANG_BUILD_DIR`.

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
