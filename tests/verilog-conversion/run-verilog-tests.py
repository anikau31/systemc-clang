import py
import argparse
import os

parser = argparse.ArgumentParser('Helper script for running Verilog tests')
parser.add_argument('-o', '--only', dest='only', type=str, help='Run only one test')
parser.add_argument('-v', '--verbose', dest='verbose', action='store_true', default=False)
parser.add_argument('-c', '--collect-only', dest='collect_only', action='store_true', default=False)

SYSTEMC_CLANG_PATH = os.environ['SYSTEMC_CLANG']

args = parser.parse_args()


def test_path():
    return '{}/tests/verilog-conversion/'.format(SYSTEMC_CLANG_PATH)


def additional_options():
    return '{}'.format('-v' if args.verbose else '')


def all_tests_args():
    return '{} {}'.format(test_path(), additional_options()).split(' ')


def one_test_args(name):
    return '{} {} -k {}'.format(test_path(), additional_options(), name).split(' ')


def collect_only_args():
    return '{} {} -s --collect-only -qq'.format(
                test_path(),
                additional_options()
            ).split(' ')


def main():

    if args.collect_only:
        py.test.cmdline.main(collect_only_args())
    elif args.only:
        py.test.cmdline.main(one_test_args(args.only))
    else:  # all tests
        py.test.cmdline.main(all_tests_args())


if __name__ == '__main__':
    main()
