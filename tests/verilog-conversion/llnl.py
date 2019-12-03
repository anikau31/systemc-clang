import os
import re
import copy
import subprocess
import argparse
import difflib

# TODO: fix these paths... and command line options
_systemc_clang_bin_path = os.environ['LLVM_INSTALL_DIR'] + "bin/systemc-clang"
# TODO: we may not get the directory of the source folder, need to add that in the script
_test_file = os.environ['SYSTEMC_CLANG_BUILD_DIR'] + "../systemc-clang/tests/llnl-examples/{}.cpp"
_example_header_folder = os.environ['SYSTEMC_CLANG_BUILD_DIR'] + "../systemc-clang/examples/llnl-examples/"
_systemc_clang_argument = ["-I", "{}/systemc/include/".format(os.environ['SYSTEMC']),
        "-std=c++14",
        "-I", "{}".format(_example_header_folder),
        "-I", "/usr/include/",
        "-D__STDC_CONSTANT_MACROS",
        "-D__STDC_LIMIT_MACROS",
        "-x c++ -w -c"]
_cmdline = [_systemc_clang_bin_path, _test_file, "--"] + _systemc_clang_argument
_parser = None

def get_test_command(module_name):
    f = _test_file.format(module_name)
    cmd = copy.copy(_cmdline)
    cmd[1] = f
    return cmd

def get_sexp_filename(module_name):
    f = _test_file.format(module_name)
    f = re.sub(".cpp$", "_hdl.txt", f)
    return f

def get_parser():
    global _parser
    _parser = argparse.ArgumentParser(description='LLNL example verilog conversion driver, this might leave the repo in inconsistent state, be careful')
    _parser.add_argument('--keep-sexp', dest='keep_sexp', action='store_true', help='Keep *_hdl.txt file for debug purpose in current directory')
    _parser.add_argument('--keep-v', dest='keep_v', action='store_true', help='Keep *_hdl.txt.v file for debug purpose')
    _parser.set_defaults(keep_sexp=False, keep_v=False)
    return _parser

def do_diff(golden, generated):
    diff = difflib.ndiff(golden.splitlines(keepends=True), generated.splitlines(keepends=True))
    # diff = filter(lambda x: x[0] == '+' or x[0] == '-', diff)
    diff = list(diff)
    print(''.join(diff))
    return len(diff) == True

def run_verilog_matched_test(module_name):
    parser = get_parser()
    args = parser.parse_args()
    keep_sexp = args.keep_sexp
    keep_v = args.keep_v
    golden_standard_file = '{}.v'.format(module_name)
    with open(golden_standard_file, 'r') as f:
        golden = f.read()
    sexp = get_sexp_filename(module_name)
    try:
        cmdline = get_test_command(module_name)
        with open(os.devnull, 'wb') as null:
            # suppressing output, assuming we are sucessfull
            subprocess.run(' '.join(cmdline), shell=True, stdout=null, stderr=null)

            subprocess.run('mv {} .'.format(sexp), shell=True)
            res = subprocess.check_output('python convert.small.py {}_hdl.txt'.format(module_name), shell=True, stderr=null)
        res = res.decode('utf-8')
        return do_diff(golden, res)
    finally:
        pass
        subprocess.run('rm -rf {}'.format(sexp), shell=True)
        if not keep_sexp:
            subprocess.run('rm -rf ./{}_hdl.txt'.format(module_name), shell=True)
        if not keep_v:
            subprocess.run('rm -rf ./{}_hdl.txt.v'.format(module_name), shell=True)
