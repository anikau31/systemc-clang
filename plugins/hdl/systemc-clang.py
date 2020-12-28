"""
systemc-clang driver, a commandline utility for calling systemc-clang end-to-end from c++ to Verilog

Usage:
    python /path/to/systemc-clang.py SYSTEMC_CLANG_PARAMS --- [options]
    options include:
    -o, --output: specify Verilog output folder

Example:
    python ../llnl/systemc-clang.py ~/working/systemc-clang/examples/llnl-examples/zfpsynth/zfp3/z3test.cpp \
        --debug -- -I ../systemc/include/ -I ../systemc-clang/examples/llnl-examples/zfpsynth/shared2/ -x c++ -w -c  \
        -std=c++14 -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DRVD \
        --- -o ./z3test_hdl.txt.v
"""
import os
import sys
import re
import subprocess
import logging

from argparse import ArgumentParser
from hcode2verilog import translate_text

parser = ArgumentParser()
parser.add_argument("-o", "--output", help="The output file name")
parser.add_argument(
    "-k",
    "--keep",
    action="store_true",
    default=False,
    help="keeps the intermediate _hdl.txt file, currently this option is not used",
)


class UnconfiguredEnvironment(Exception):
    """raised when an environment variable is not set"""

    def __init__(self, env):
        super().__init__("Environmental variable {} not found.".format(env))


class MissingSystemCClang(Exception):
    """raised when the systemc-clang binary is not found"""


class InvalidCommandlineFormat(Exception):
    """raised when the number of --- in the commandline option is greater than or equal to 2"""


class SystemCClangFatalError(Exception):
    """raised when running of systemc-clang triggers a fatal error"""


class SystemCClang:
    """A class that encapsulate the execution of systemc-clang binary"""

    def __check_systemc_clang_executable_exists(self):
        if not os.path.isfile(self.systemc_clang_binary):
            raise MissingSystemCClang

    def __init__(
        self, systemc_clang_build_dir=None, llvm_install_path=None, systemc_path=None
    ):
        if systemc_clang_build_dir:
            self._systemc_clang_build_dir = systemc_clang_build_dir
        else:
            self._systemc_clang_build_dir = os.environ["SYSTEMC_CLANG_BUILD_DIR"]

        if llvm_install_path:
            self._llvm_install_path = llvm_install_path
        else:
            self._llvm_install_path = os.environ["LLVM_INSTALL_DIR"]

        if systemc_path:
            self._systemc_path = systemc_path
        else:
            self._systemc_path = os.environ["SYSTEMC"]

        self.__check_systemc_clang_executable_exists()

    @property
    def systemc_clang_build_dir(self):
        return self._systemc_clang_build_dir

    @property
    def systemc_clang_binary(self):
        executable_path = os.path.join(self.systemc_clang_build_dir, "systemc-clang")
        return executable_path

    @property
    def llvm_install_path(self):
        return self._llvm_install_path

    @property
    def llvm_inc_dir(self):
        """
        returns the include directory necessary for systemc-clang to locate headers.
        Currently, it only supports version 10.0.0
        """
        return os.path.join(self.llvm_install_path, "lib/clang/10.0.0/include")

    @property
    def systemc_path(self):
        return self._systemc_path

    @property
    def systemc_inc_dir(self):
        return os.path.join(self.systemc_path, "include")

    def execute(self, args):
        """
        executes systemc-clang as if it is on the commandline
        """
        bin_path = self.systemc_clang_binary
        args_to_sysc = args + ["-I", self.llvm_inc_dir, "-I", self.systemc_inc_dir]
        result = subprocess.run(
            [bin_path, *args_to_sysc], stdout=subprocess.PIPE, stderr=subprocess.PIPE
        )  # in case the command fail, an exception will be raised
        err = result.stderr.decode("utf-8")
        res = list(re.finditer(r"^.*fatal error.*$", err, re.MULTILINE))
        if res:
            msg = "\n".join(x.group() for x in res)
            raise SystemCClangFatalError(
                """Fatal error captured in systemc-clang stderr, try run systemc-clang separately to identify the error:\n"""
                """{}""".format(msg)
            )

        return SystemCClang.__get_systemc_clang_output_files(args)

    @staticmethod
    def __get_systemc_clang_output_files(argv):
        sources = SystemCClang.__get_sources_from_args(argv)
        target = [x.replace(".cpp", "_hdl.txt") for x in sources]
        return target

    @staticmethod
    def __get_sources_from_args(argv):
        sources = []
        for arg in argv:
            if not arg.startswith("--"):
                sources.append(arg)
            else:
                break
        return sources


def environment_check():
    check_list = [
        "LLVM_INSTALL_DIR",  # determines the correct location and headers for systemc-clang
        "SYSTEMC",  # determines the correct include directory for SystemC,
        "SYSTEMC_CLANG_BUILD_DIR",  # determines the build directory for systemc-clang binary,
        "SYSTEMC_CLANG",  # determines the python script location
    ]
    for env in check_list:
        if env not in os.environ:
            raise UnconfiguredEnvironment(env)


def get_argv():
    """
    returns the source file paths to translate, arguments for systemc-clang and argument for this script
    returns: sources, argv_sysc, argv_python
    """
    argv = sys.argv[1:]  # we don't care the script name

    if argv.count("---") == 1:
        sep_idx = argv.index("---")
        argv_sysc = argv[:sep_idx]
        argv_python = argv[sep_idx + 1:]
    elif argv.count("---") == 0:
        argv_sysc = argv
        argv_python = []
    else:
        raise InvalidCommandlineFormat
    return argv_sysc, argv_python


def invoke_sysc(argv_sysc):
    wrapper = SystemCClang()
    targets = wrapper.execute(argv_sysc)
    if len(targets) > 1:
        logging.warning("Up to 1 target is translated currently")
    target = targets[0]
    return target


def invoke_translation(target, argv_python):
    args = parser.parse_args(argv_python)
    with open(target, "r") as fin:
        res = translate_text(fin.read())

    outfile = args.output
    if not outfile:
        outfile = target + ".v"
    with open(outfile, "w") as fout:
        fout.writelines(res)


def main(argv_sysc, argv_python):
    target = invoke_sysc(argv_sysc)
    invoke_translation(target, argv_python)


if __name__ == "__main__":
    environment_check()
    main(*get_argv())
