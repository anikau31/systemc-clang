import os
import sys
import argparse
import datetime
import shutil
import difflib
from itertools import chain
from pathlib import Path
sys.path.append(str(Path(os.environ['SYSTEMC_CLANG']) / 'plugins' / 'hdl'))
sysc_clang = __import__('systemc-clang')

# time stamp is set when the script is loaded
__time_stamp = datetime.datetime.now().strftime('%Y-%m-%d_%H-%M-%S')


def get_output_folder(args):
    """get the timestamped output folder name"""
    output_dir = args.output_dir if args.output_dir is not None else os.getcwd()
    if args.no_ts:
        dir_name = os.path.abspath(os.path.join(args.output_dir, ''))
    else:
        dir_name = os.path.abspath(os.path.join(args.output_dir, __time_stamp))
        os.makedirs(dir_name, exist_ok=True)
    return dir_name


def default_options():
    """default options to systemc-clang after the --"""
    return ['-x', 'c++', '-w', '-c', '-std=c++14', '_-D__STDC_CONSTANT_MACROS',  '-D__STDC_LIMIT_MACROS', '-DRVD']


def validate_args(args):
    use_cpp = args.action in ['cpp-to-hdl', 'cpp-to-v']
    has_cpp = args.cpp is not None

    use_hdl = args.action in ['hdl-to-v']
    has_hdl = args.hdl is not None
    gen_hdl = args.action in ['cpp-to-hdl', 'cpp-to-v']

    gen_verilog = args.action in ['cpp-to-v', 'hdl-to-v']

    has_golden_interm = args.golden_intermediate is not None
    has_golden_verilog = args.golden_verilog is not None

    if use_cpp ^ has_cpp:
        raise ValueError("Action '{}' and --cpp is not compatible, --cpp must be present exactly when the action is one of cpp-to-v or cpp-to-hdl".format(args.action))
    if use_hdl ^ has_hdl:
        raise ValueError("Action '{}' and --hdl is not compatible, --hdl must be present exactly when the action is hdl-to-v".format(args.action))

    if has_golden_interm and not gen_hdl:
        raise ValueError("Action '{}' and --golden-intermediate is not compatible, --golden-intermediate may only be present when the action is one of cpp-to-v or cpp-to-hdl".format(args.action))
    if has_golden_verilog and not gen_verilog:
        raise ValueError("Action '{}' and --golden-verilog is not compatible, --golden-verilog may only be present when the action is one of cpp-to-v or hdl-to-v".format(args.action))


def cpp_to_hdl(args):
    # setup necessary parameters
    cpp_path = args.cpp
    cpp_name = os.path.basename(cpp_path)
    target_name = cpp_name.replace('.cpp', '_hdl')
    if args.verbose:
        print('path: {}'.format(cpp_path))
    extra_options = list(chain.from_iterable(('-I', inc_path) for inc_path in args.include_path))
    output_dir = get_output_folder(args)
    print('cmd: ')
    target = sysc_clang.invoke_sysc(['-hdl-file-out', os.path.join(output_dir, target_name), cpp_name, '--debug', '--'] + default_options() + extra_options)
    if args.verbose:
        print('sexp_loc: {}'.format(target))
        print('sexp_filename: {}'.format(target))
    fn = os.path.basename(target)
    destination = os.path.join(output_dir, fn)
    if args.verbose:
        print('The _hdl.txt file is written to: {}'.format(os.path.abspath(destination)))
    args.hdl = destination  # when cpp_to_hdl is called, hdl should be none


def hdl_to_v(args):
    source = args.hdl
    output_dir = get_output_folder(args)
    target = os.path.basename(source) + '.v'
    destination = os.path.join(output_dir, target)
    sysc_clang.invoke_translation(source, ['-o', destination])
    args.verilog = destination


def has_diff(diff_delta):
    for l in diff_delta:
        if l[0] in '+-?':
            return True
    return False


def diff_file(f_a, f_b):
    with open(f_a, 'r') as f:
        s_a = f.read()
    with open(f_b, 'r') as f:
        s_b = f.read()
    s_a_lines = s_a.strip().splitlines(keepends=True)
    s_b_lines = s_b.strip().splitlines(keepends=True)
    delta = list(difflib.ndiff(s_a_lines, s_b_lines, charjunk=lambda x: x in ' \t'))
    return delta


def check_hdl(args):
    golden_f = os.path.abspath(args.golden_intermediate)
    hdl_f = args.hdl
    if args.verbose:
        print('Comparing: {} and {}'.format(golden_f, hdl_f))
    delta = diff_file(golden_f, hdl_f)
    is_diff = has_diff(delta)
    if is_diff:
        print(''.join(delta))
    else:
        print('* HDL is the same as specified golden input {}'.format(args.golden_intermediate))
    output_dir = get_output_folder(args)
    if is_diff:
        with open(os.path.join(output_dir, 'diff.hdl'), 'w') as f:
            f.writelines(delta)


def check_verilog(args):
    golden_f = os.path.abspath(args.golden_verilog)
    verilog_f = args.verilog
    if args.verbose:
        print('Comparing: {} and {}'.format(golden_f, verilog_f))
    delta = diff_file(golden_f, verilog_f)
    is_diff = has_diff(delta)
    if is_diff:
        print(''.join(delta))
    else:
        print('* Verilog is the same as specified golden input {}'.format(args.golden_verilog))
    output_dir = get_output_folder(args)
    if is_diff:
        with open(os.path.join(output_dir, 'diff.verilog'), 'w') as f:
            f.writelines(delta)
    pass


def run(args, flow_flags):
    for flow_name, flag in flow_flags.items():
        if flag:
            flow_name(args)


def main():
    parser = argparse.ArgumentParser("A tool for running and comparing against a golden standard")
    parser.add_argument('action', choices=[ 'cpp-to-hdl', 'hdl-to-v', 'cpp-to-v'], help='Action to perfrom')
    parser.add_argument('--cpp', help='The SystemC C++ file to convert')
    parser.add_argument('--hdl', help='The _hdl.txt file to convert')
    parser.add_argument('--golden-intermediate', help='The _hdl.txt file to compare against')
    # parser.add_argument('--verilog', help='The Verilog file to compare against')
    parser.add_argument('--golden-verilog', help='The Verilog file to compare against')
    parser.add_argument('--include-path', default=list(), nargs='*', help='Include path (-I) option for the systemc-clang command')
    parser.add_argument('--output-dir', help='The output folder to store the results. '
                                             'Within the folder, a timestamped subfolder will be created.'
                                             'If this option is not provided, current working directory will be used.')
    parser.add_argument('--verbose', default=False, action='store_true', help='Whether show the output of the called tools (systemc-clang and hcode2verilog.py)')
    parser.add_argument('--no-ts', default=False, action='store_true', help='Do not create time-stamped folder')
    parser.add_argument('--force', default=False, action='store_true', help='Overwrite content if necessary')

    args = parser.parse_args()
    validate_args(args)

    flow_flags = {
        cpp_to_hdl: args.action in ['cpp-to-hdl', 'cpp-to-v'],
        hdl_to_v: args.action == 'hdl-to-v',
        check_hdl: args.action in ['cpp-to-v', 'cpp-to-hdl'] and args.golden_intermediate is not None,
        check_verilog: args.action in ['cpp-to-v', 'hdl-to-v'] and args.golden_verilog is not None,
    }  # the order of dict elements are enforced 
    run(args, flow_flags)


if __name__ == '__main__':
    main()
