from parselib.grammar import lark_grammar
from parselib.utils import tidify
from parselib.transforms import VerilogTranslator
import sys
import traceback
import logging
import argparse
import pathlib
logging.basicConfig(level=logging.DEBUG)

from lark import logger
from pprint import pprint

logger.setLevel(logging.DEBUG)

__file_input = None

def _handle_exception_and_exit(e, retcode=3):
    exc_type, exc_value, exc_traceback = sys.exc_info()
    print("***** print_exception:")
    # exc_type below is ignored on 3.5 and later
    traceback.print_exception(exc_type, exc_value, exc_traceback, file=sys.stdout, limit=-60)

    stack_summary = list(traceback.walk_tb(exc_traceback))
    if stack_summary:
        f, lineno = stack_summary[-1]
        if 'tree' in f.f_locals:
            print("While processing the following tree node (the format might differ from hNode representation).")
            if __file_input:
                print("When parsing file: ", __file_input)
            print("Line: ", f.f_locals['tree'].meta.line, ", Column: ", f.f_locals['tree'].meta.column)
            print(f.f_locals['tree'].pretty())
            print(f.f_locals['tree'].meta)
    exit(retcode)


def translate_text(file_content):
    t = lark_grammar.parse(file_content)

    try:
        x = VerilogTranslator
        return x.translate(t)
    except Exception as e:
        _handle_exception_and_exit(e)


def main():
    global __file_input

    parser = argparse.ArgumentParser()
    parser.add_argument('input', type=str, help='Input file name (normally the _hdl.txt file)')
    parser.add_argument('--output', type=str, help='The outpuf filename')
    parser.add_argument('-f', '--force', action="store_true",
                        help='Whether the script overwrites resultant file')
    args = parser.parse_args()
    filename = args.input
    __file_input = filename
    outputname = filename + ".sv"
    if args.output is not None:
        outputname = args.output
    if not args.force and pathlib.Path(outputname).exists():
        raise FileExistsError("File {} exists".format(outputname))
    with open(filename, 'r') as f:
        file_content = f.read()
    try:
        t = lark_grammar.parse(file_content)
    except Exception as e:
        print(e)
        exit(2)  # This code has specific meanings, should be tested in the verilog tests
    try:
        x = VerilogTranslator
        res = x.translate(t)
    except Exception as e:
        _handle_exception_and_exit(e)

    with open(outputname, 'w+') as f:
        f.writelines(res)
        f.write("\n\n")
    # print('Result: ', res)


if __name__ == '__main__':
    main()
    # l.parse('')
