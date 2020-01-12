"""Driver for using systemc-clang tool in python"""
import os
import re
import subprocess
from pathlib import Path


class SystemCClangDriver(object):
    """A clang driver class for running systemc-clang binaries
    """
    SYSTEMC_CLANG_BIN_PATH = os.environ['LLVM_INSTALL_DIR'] + "/bin/systemc-clang"
    PYTHON_CONVERT_TEMPLATE = 'python {}'.format(os.environ['SYSTEMC_CLANG_BUILD_DIR'] + "../systemc-clang/plugins/xlat/convert.py")
    SYSTEMC_CLANG_ARGUMENTS = [
        "-I", "{}/include/".format(os.environ['SYSTEMC']),
        "-std=c++14",
        "-I", "/usr/include/",
        "-D__STDC_CONSTANT_MACROS",
        "-D__STDC_LIMIT_MACROS",
        "-x c++ -w -c"
    ]



    """
    Extra args are positional and non positional arguments, for example,
    extra header etc
    """
    def systemc_clang_commandline(self, filename):
        args = SystemCClangDriver.SYSTEMC_CLANG_ARGUMENTS + self.conf.positional_arguments
        return [
            SystemCClangDriver.SYSTEMC_CLANG_BIN_PATH,
            filename,
            '--'
        ] + args + [
            # for the containing folder of the cpp/hpp
            "-I", str(Path(filename).parent)
        ]

    def __init__(self, conf, *args, **kwargs):
        self.conf=conf
        pass

    """
    Takes _hdl.txt as input, generate .v
    """
    def generate_verilog_from_sexp(self, path):
        pass

    """
    Takes .cpp as input, generate _hdl.txt
    """
    def generate_sexp(self, path, output_folder, keep_sexp=False, verbose=False):
        cmdline = self.systemc_clang_commandline(filename=path)
        if path.endswith('.cpp'):
            sexp_loc = re.sub(".cpp$", "_hdl.txt", path)
        elif path.endswith('.hpp'):
            sexp_loc = re.sub(".hpp$", "_hdl.txt", path)
        else:
            raise RuntimeError('Filename should end with .cpp or .hpp')

        if os.path.isfile(sexp_loc):
            raise RuntimeError('File to generate: {} exists'.format(sexp_loc))

        sexp_filename = os.path.basename(sexp_loc)
        output_filename = '{}/{}'.format(output_folder, sexp_filename)
        if os.path.isfile(output_filename):
            raise RuntimeError('File to generate: {} exists'.format(output_filename))

        if verbose:
            print('path: ', path)
            print('sexp_loc: ', sexp_loc)
            print('sexp_filename: ', sexp_filename)
            print('output_filename: ', output_filename)

        try:
            if verbose:
                subprocess.run(' '.join(cmdline), shell=True)
            else:
                with open(os.devnull, 'wb') as null:
                    subprocess.run(' '.join(cmdline), shell=True, 
                            stdout=null, 
                            stderr=null)
            subprocess.run('mv {} {}'.format(sexp_loc, output_folder), shell=True)
        finally:
            # we remove the generated file anyway
            if not keep_sexp:
                subprocess.run('rm {}'.format(output_filename), shell=True)
            # Currently XLAT will generate the file in a fixed location
            if keep_sexp and os.path.normpath(sexp_loc) != os.path.normpath(output_filename):
                subprocess.run('rm -f {}'.format(sexp_loc), shell=True)

    """
    Takes .cpp as input, generate .v
    """
    def generate_verilog(self, path, output_folder):
        generate_sexp(path)
        pass
