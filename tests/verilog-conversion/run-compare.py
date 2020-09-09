import argparse, os, datetime
import pathlib
from driver import *
from util.vparser import VerilogParser
from util.conf import TestingConfigurations
from util.sexpdiff import sexpdiff

parser = argparse.ArgumentParser("A tool for running and comparing against a golden standard")
parser.add_argument('action', choices=[ 'cpp-to-hdl', 'hdl-to-v', 'cpp-to-v'], help='Action to perfrom')
parser.add_argument('--cpp', help='The SystemC C++ file to convert')
parser.add_argument('--hdl', help='The _hdl.txt file to convert')
parser.add_argument('--golden-intermediate', help='The _hdl.txt file to compare against')
# parser.add_argument('--verilog', help='The Verilog file to compare against')
parser.add_argument('--golden-verilog', help='The Verilog file to compare against')
parser.add_argument('--include-path', nargs='*', help='Include path (-I) option for the systemc-clang command')
parser.add_argument('--output-dir', help='The output folder to store the results. '
                                         'Within the folder, a timestamped subfolder will be created')
parser.add_argument('--verbose', default=False, action='store_true', help='Whether show the output of the called tools (systemc-clang and hcode2verilog.py)')
parser.add_argument('--no-ts', default=False, action='store_true', help='Do not create time-stamped folder')
parser.add_argument('--force', default=False, action='store_true', help='Overwrite content if necessary')
args = parser.parse_args()

# shared variables
conf = TestingConfigurations(root_folder=[], golden_folder=[], 
        header_folders=args.include_path if args.include_path else [])
driver = SystemCClangDriver(conf=conf)

def get_output_folder():
    """get the timestamped output folder name"""
    if args.no_ts:
        dir_name = os.path.join(args.output_dir, '')
    else:
        dir_name = os.path.join(args.output_dir, datetime.datetime.now().strftime('%Y-%m-%d_%H-%M-%S'))
    return dir_name

def cpp_to_hdl(args, output_folder=None, rmdir=True):
    assert args.cpp, 'should provide c++ (--cpp)'
    filename = args.cpp
    if not output_folder:
        output_folder = get_output_folder() + '/'
        pathlib.Path(output_folder).mkdir(exist_ok=args.force)
    res = False
    path = None
    try:
        res, path = driver.generate_sexp(
                path=os.path.join(filename), 
                output_folder=output_folder,
                keep_sexp=True,
                verbose=args.verbose,
                force=args.force)
        if res.xlat_run and args.golden_intermediate:
            if not os.path.isfile(args.golden_intermediate):
                raise RuntimeError("Golden file {} does not exists".format(args.golden_intermediate))
            is_diff, diff_str = sexpdiff(path, args.golden_intermediate)
            if is_diff:
                print('Differences between {} and {}:'.format(path, args.golden_intermediate))
                print(diff_str)
                diff_file = output_folder + '/diff'
                print('diff stored in: {}'.format(diff_file))
                with open(diff_file, 'w+') as f:
                    f.write(diff_str)
            else:
                print('{} and {} has the same content'.format(path, args.golden_intermediate))
    except RuntimeError as e:
        print('****** Error *****')
        print(e)
    finally:
        if not res.xlat_run:
            print('Conversion failed (cpp to hdl.txt), please check program output')
            if rmdir:
                pathlib.Path(output_folder).rmdir()
        else:
            print('The _hdl.txt file is written to: {}'.format(path))
        return res, path

def hdl_to_v(args, hdl=None, output_folder=None, rmdir=True):
    if hdl:
        filename = hdl
    else:
        assert args.hdl, 'should provide _hdl.txt (--hdl)'
        filename = args.hdl
    if not output_folder:
        output_folder = get_output_folder() + '/'
        pathlib.Path(output_folder).mkdir(exist_ok=False)
    res = False
    path = None
    try:
        res, path = driver.generate_verilog_from_sexp(
                path=filename,
                output_folder=output_folder,
                verbose=args.verbose,
                keep_v=True,
                force=args.force)

        if res.convert_run and args.golden_verilog:
            diff_info = VerilogParser.diff(
                    path,
                    args.golden_verilog)
            if diff_info is None:
                print('{} and {} has the same Verilog AST'.format(path, args.golden_verilog))
            else:
                print('Diff information of {} and {}:'.format(path, args.golden_verilog))
                print(str(diff_info))
                diff_file = output_folder + '/verilog.ast.diffinfo'
                print('diff stored in: {}'.format(diff_file))
                with open(diff_file, 'w+') as f:
                    f.write(str(diff_info))
    except:
        raise
    finally:
        if not res.convert_run or not res.convert_syntax or not res.convert_transform:
            print('Conversion failed, please check program output (with --verbose option)')
            print('Syntax: {}'.format(res.convert_syntax))
            print('Transform: {}'.format(res.convert_transform))
            raise
        else:
            print('The .v file is written to {}'.format(path))
        return res, path

def cpp_to_v(args):
    assert args.cpp, 'should provide c++ (--cpp)'
    filename = args.cpp
    output_folder = get_output_folder() + '/'
    pathlib.Path(output_folder).mkdir(exist_ok=args.force)
    res, path = cpp_to_hdl(args, output_folder=output_folder, rmdir=False)
    if not res.xlat_run:
        print('Conversion to _hdl.txt failed')
        raise
    res, path = hdl_to_v(args, hdl=path, output_folder=output_folder, rmdir=False)


def main():

    if args.action == 'cpp-to-hdl':
        cpp_to_hdl(args)
    elif args.action == 'hdl-to-v':
        hdl_to_v(args)
    elif args.action == 'cpp-to-v':
        cpp_to_v(args)

    print('Using systemc-clang binary: {}'.format(SystemCClangDriver.SYSTEMC_CLANG_BIN_PATH))
    print('Using hcode2verilog.py command: {}'.format(SystemCClangDriver.PYTHON_CONVERT_TEMPLATE))

if __name__ == '__main__':
    main()
