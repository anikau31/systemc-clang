# Python Tests for Verilog Conversion and Utilites for Development

## Preliminaries

**Before going through this documentation, make sure you have built the systemc-clang successfully, this means that the `scripts/paths.sh` script is sourced appropriately.**
In addition, ensure that you are using [Release 2.2.0](https://github.com/anikau31/systemc-clang/releases/tag/2.2.0).

### Repository path
We use the environment variable `$SYSTEMC_CLANG` to specify the systemc-clang git repository directory.  This is the source directory.

```bash
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
- Install `iverilog`. On Ubuntu, use 
```bash 
$ sudo apt install iverilog
```
- We use certain Python packages for the testing infrastructure. These packages are listed in `requirements.txt`.  To install them, run
```bash
$ pip install -r $SYSTEMC_CLANG/requirements.txt
``` 

## The Convesion Tool

We provide a command-line tool that helps the development process for testing.
The tool does the following.

- Convert the SystemC model to a file with the suffix `_hdl.txt`.  The resulting file has the intermediate representation from which we synthesize Verilog.
- Convert the file suffixed with `_hdl.txt` to Verilog.
- The conversion result will be stored in time-stamped folders

The tool is located at `$SYSTEMC_CLANG/tests/verilog-conversion/run-compare.py`.  It can be invoked as follows.
```bash
$ python $SYSTEMC_CLANG/tests/verilog-conversion/run-compare.py -h
usage: A tool for running and comparing against a golden standard
       [-h] [--cpp CPP] [--hdl HDL]
       [--golden-intermediate GOLDEN_INTERMEDIATE]
       [--golden-verilog GOLDEN_VERILOG]
       [--include-path [INCLUDE_PATH [INCLUDE_PATH ...]]]
       [--output-dir OUTPUT_DIR] [--verbose]
       {cpp-to-hdl,hdl-to-v,cpp-to-v}
       ...
 ```

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
$ cp $SYSTEMC_CLANG/tests/data/verilog-conversion-custom/add/add.cpp .
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
$ python -B $SYSTEMC_CLANG/tests/verilog-conversion/run-compare.py \
     cpp-to-hdl \
     --output-dir ./results/  \
     --cpp add.cpp \
     --verbose
```
Note that the `-B` flag prevents `.pyc` file generation, which keeps the directory clean.

The output should be similar to:
```
path:  add.cpp
sexp_loc:  add_hdl.txt
sexp_filename:  add_hdl.txt
output_filename:  /home/allen/working/systemc-clang-test-tool/results/2020-03-05_11-49-11/add_hdl.txt
cmd:  /home/allen/working/clang+llvm-9.0.0//bin/systemc-clang add.cpp -- -I /home/allen/working/systemc//include/ -std=c++14 -I /usr/include/ -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -x c++ -w -c -I .
The _hdl.txt file is written to: /home/allen/working/systemc-clang-test-tool/results/2020-03-05_11-49-11/add_hdl.txt
Using systemc-clang binary: /home/allen/working/clang+llvm-9.0.0//bin/systemc-clang
Using hcode2verilog.py command: python /home/allen/working/systemc-clang//plugins/xlat/hcode2verilog.py
```

Note that the `cmd: ...` shows the real command that is running within the script. 
Also, your time-stamp is likely different from what we have in the output above.

If we check the output folder, we observe something similar to:
```bash
$ ls results/2020-03-05_11-49-11/
add_hdl.txt  systemc-clang.stderr  systemc-clang.stdout
```

Apart from the output file `add_hdl.txt`, the standard output/standard error of the systemc-clang tool is also captured, which is useful when debugging.

Now, copy the `add_hdl.txt` file for later use, and the directory structure should be as follows:
```
$ cp results/2020-03-05_11-49-11/add_hdl.txt . && ls
add.cpp  add_hdl.txt  results
```

Apart from the conversion, one can specify a golden standard file with `--hdl` option, and the tool will do a diff at the end of the conversion against the specified golden standard.

For example, create a directory called `golden`, copy our provided golden standard file `add_hdl.txt` to the `golden` and confirm the directory structure:
```bash
$ mkdir golden && cp $SYSTEMC_CLANG/tests/data/verilog-conversion-custom/add/golden/add_hdl.txt golden/
$ ls .
add.cpp  golden  results
$ ls golden/
add_hdl.txt
```

Next, run the tool again, with `--hdl` option enabled:
```
$ python -B $SYSTEMC_CLANG/tests/verilog-conversion/run-compare.py \
     cpp-to-hdl \
     --output-dir ./results/  \
     --cpp add.cpp \
     --verbose \
     --golden-intermediate ./golden/add_hdl.txt
```

If the generated file and the golden standard file are different, the diff will be presented to the console, and will be stored in the time-stamped folder, together with the generated `add_hdl.txt` file.
```
$ ls results/2020-03-05_12-16-33/
add_hdl.txt  diff  systemc-clang.stderr  systemc-clang.stdout
```

If the generated file and the golden standard file are the same, the diff will not output to the screen.

The `hdl-to-v` works similarly and it converts `_hdl.txt` file to Verilog file.  

#### Converting add_hdl.txt to add_hdl.txt.v
   ```bash
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
     --golden-verilog $SYSTEMC_CLANG/tests/data/verilog-conversion-custom/add/golden/add_hdl.txt.v
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

For clarity, the output does not show the full path of the tests, which should be straight forward.

In this command, we are using the `run-verilog-tests.py` script, which provides an interface for running tests.
* The `-o` option selects the tests whose names match the specified value to run. In this case, all tests with a name containing `test_custom` will be run.
* The `-v` option shows detailed output.

Notice the `[xor]` and `[add]` prefix. These are the names of the **test data** of the design.
At a high level, when a user needs to add a test to the framework, this user provides the necessary files, or **test data**, for each SystemC design and organizes them in the `$SYSTEMC_CLANG` directory in a certain way.

The **test data** is composed of three parts. 
1. A SystemC model file
2. A golden standard `_hdl.txt` file that is generated by XLAT
3. A golden standard Verilog file that is generated by hcode2verilog.py

In our example above, we provided the **test data** for two designs: `add` and `xor`.

To observe how these files are organized, you can see the folder `$SYSTEMC_CLANG/tests/data/verilog-conversion-custom/` (Note: you may not have the `tree` command installed, but you can still verify the directory structure): 
```bash
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

We will show how to add your tests later.
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

We describe how a user can add a test to the framework.
We will make another module to test: a `sub` module that takes two inputs and outputs their difference by doing subtraction.
We assume that all commands done in this section are done in the path `$SYSTEMC_CLANG_BUILD_DIR`.

To add a test, create another folder in `$SYSTEMC_CLANG/tests/data/verilog-conversion-custom/`:
```
mkdir $SYSTEMC_CLANG/tests/data/verilog-conversion-custom/sub
```

> If you run the test now with `python -B run-verilog-tests.py -o test_custom -v`, you will not see the difference and there will only be tests for `xor` and `add`

Next re-run `cmake` to make sure the tests are copied. Note that you don't need to remove `CMakefiles/` and `CMakeCache.txt`.
```bash
$ cd $SYSTEMC_CLANG_BUILD_DIR
$ rm -rf tests/data/verilog-conversion-custom/
$ cmake ../systemc-clang -DENABLE_TESTS=on -DXLAT=on -DENABLE_VERILOG_TESTS=on
```

And now, we run the tests again with:
```bash
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
```bash
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

This is true because we did not add any SystemC model file.  Now we create `$SYSTEMC_CLANG/tests/data/verilog-conversion-custom/sub/sub.cpp` with the following content:
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

Next **run `cmake`** and run the the test again:
```bash
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

This time, the golden standard \_hdl.txt file is missing, as the information states: `E       FileNotFoundError: [Errno 2] No such file or directory: '/home/allen/working/systemc-clang-build//tests/data/verilog-conversion-custom/sub/golden//sub_hdl.txt'`.

Here, we use the golden standard of the `add` module, which is incorrect for `sub`, to show how to interpret the error information.

To add a golden standard file, create a folder named `golden` under `sub` and put the golden standard file inside:
```
$ mkdir $SYSTEMC_CLANG/tests/data/verilog-conversion-custom/sub/golden
$ cp $SYSTEMC_CLANG/tests/data/verilog-conversion-custom/add/golden/add_hdl.txt \
     $SYSTEMC_CLANG/tests/data/verilog-conversion-custom/sub/golden/sub_hdl.txt
```

Now, the `verilog-conversion-custom` folder looks similar to:
```bash
$ tree $SYSTEMC_CLANG/tests/data/verilog-conversion-custom/
/home/allen/working/systemc-clang//tests/data/verilog-conversion-custom/
├── add
│   ├── add.cpp
│   └── golden
│       ├── add_hdl.txt
│       └── add_hdl.txt.v
├── sub
│   ├── golden
│   │   └── sub_hdl.txt
│   └── sub.cpp
└── xor
    ├── golden
    │   ├── xor_hdl.txt
    │   └── xor_hdl.txt.v
    └── xor.cpp
```

**Run `cmake`** and run the the test again:
```bash
$ cmake ../systemc-clang -DENABLE_TESTS=on -DXLAT=on -DENABLE_VERILOG_TESTS=on
$ python -B run-verilog-tests.py -o test_custom_sexp[sub] -v
...
______________ test_custom_sexp[sub] ____________
tmpdir = local('/tmp/pytest-of-allen/pytest-14/test_custom_sexp_sub_0')
customdriver = <driver.SystemCClangDriver object at 0x7fc2f46b7b10>, tool_output = False
...
>       assert not diff_res, 'should match golden standard'
E       AssertionError: should match golden standard
E       assert not True

../systemc-clang/tests/verilog-conversion/test_custom.py:33: AssertionError
-------------- Captured stdout call -------------
...
-   hModule topadd2_0 [
?              ^^^
+   hModule topsub2_0 [
?              ^^^
      hPortsigvarlist  NONAME [
        hPortin in_port_1 NOLIST
...
        hTypeinfo  NONAME [
          hType sc_in NOLIST
-             hBinop + [
?                    ^
+             hBinop - [
?                    ^
```

Now, the test fails because the module name and the operation symbol does not match.

Here, we can fix the golden standard file by replacing `topadd2_0` with `topsub2_0` and replacing `hBinop +` with `hBinop -`.
These changes are made in `$SYSTEMC_CLANG/tests/data/verilog-conversion-custom/sub/golden/sub_hdl.txt`

Also, pay attention to the line where it says `tmpdir`, it logs the temporary directory where the program output are stored:
```bash
$ ls /tmp/pytest-of-allen/pytest-14/test_custom_sexp_sub_0
diff  sub.cpp  sub_hdl.txt  systemc-clang.stderr  systemc-clang.stdout
```
These files are useful for debugging purpose.


After making changes to the golden standard file, **Run `cmake`** and run the the test again and it should pass:
```bash
$ cmake ../systemc-clang -DENABLE_TESTS=on -DXLAT=on -DENABLE_VERILOG_TESTS=on
$ python -B run-verilog-tests.py -o test_custom_sexp[sub] -v
```

Similarly, we can copy the golden Verilog file of `add`:
```
$ cp $SYSTEMC_CLANG/tests/data/verilog-conversion-custom/add/golden/add_hdl.txt.v $SYSTEMC_CLANG/tests/data/verilog-conversion-custom/sub/golden/sub_hdl.txt.v
```
Then we make changes to the file so that it ends up being the following form:
```verilog
module topsub2_0(
input wire [31:0] in_port_1,
input wire [31:0] in_port_2,
input wire clk,
output reg [31:0] out_port
);


always @(posedge clk) begin: topEntry

out_port <= (in_port_1) - (in_port_2);
end // topEntry
endmodule // topsub2_0
```


After making changes to the golden standard Verilog, **Run `cmake`** and run the the test again and it should pass:
```bash
$ cmake ../systemc-clang -DENABLE_TESTS=on -DXLAT=on -DENABLE_VERILOG_TESTS=on
$ python -B run-verilog-tests.py -o test_custom -v
```

Now, we have covered the basics of adding a test to the test framework.


# Appendix

## Other tests

There are other tests as well when running the python scripts.
You can view all the tests using the following script:
```bash
$ python -B run-verilog-tests.py --collect-only -v
systemc-clang/tests/verilog-conversion/test_custom.py::test_custom_sexp[xor]
systemc-clang/tests/verilog-conversion/test_custom.py::test_custom_sexp[sub]
systemc-clang/tests/verilog-conversion/test_custom.py::test_custom_sexp[add]
systemc-clang/tests/verilog-conversion/test_custom.py::test_custom_verilog[xor]
systemc-clang/tests/verilog-conversion/test_custom.py::test_custom_verilog[sub]
systemc-clang/tests/verilog-conversion/test_custom.py::test_custom_verilog[add]
systemc-clang/tests/verilog-conversion/test_custom.py::test_custom_sexp_to_verilog[xor]
systemc-clang/tests/verilog-conversion/test_custom.py::test_custom_sexp_to_verilog[sub]
systemc-clang/tests/verilog-conversion/test_custom.py::test_custom_sexp_to_verilog[add]
...
```

Some of the tests are for sanity check, to make sure that the tools are set-up correctly.

Some of the tests are for the example folder because they don't follow certain naming convention and they need different driver for detecting modules.

