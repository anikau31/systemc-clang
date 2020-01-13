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
    def generate_verilog_from_sexp(self, path, output_folder, keep_v=False, verbose=False):
        if not path.endswith('_hdl.txt'):
            raise RuntimeError('Filename should end with _hdl.txt')
        v_loc = path + '.v'
        v_loc = os.path.abspath(v_loc)
        if os.path.isfile(v_loc):
            raise RuntimeError('File to generate: {} exists'.format(v_loc))

        v_filename = os.path.basename(v_loc)
        output_filename = '{}/{}'.format(output_folder, v_filename)
        output_filename = os.path.abspath(output_filename)
        if os.path.isfile(output_filename):
            raise RuntimeError('File to generate: {} exists'.format(v_loc))

        path = os.path.abspath(path)
        if not os.path.isfile(path):
            raise RuntimeError('Sexp file {} does not exist'.format(path))

        if verbose:
            print('path: ', path)
            print('v_loc: ', os.path.normpath(v_loc))
            print('v_filename: ', v_filename)
            print('output_filename: ', os.path.normpath(output_filename))

        cmdline = ' '.join([
            SystemCClangDriver.PYTHON_CONVERT_TEMPLATE,
            path
        ])
        if verbose:
            print('cmdline', cmdline)
        try:
            if verbose:
                subprocess.run(cmdline, shell=True)
            else:
                with open(os.devnull, 'wb') as null:
                    subprocess.run(cmdline)
            return True, output_filename
        except:
            raise
        finally:
            if not keep_v:
                subprocess.run('rm {}'.format(output_filename), shell=True)
            if keep_v and os.path.normpath(v_loc) != os.path.normpath(output_filename):
                subprocess.run('rm -f {}'.format(output_filename), shell=True)

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
        ok = True
        try:
            if verbose:
                print('cmd: ', ' '.join(cmdline))
                subprocess.run(' '.join(cmdline), shell=True)
            else:
                with open(os.devnull, 'wb') as null:
                    subprocess.run(' '.join(cmdline), shell=True, 
                            stdout=null, 
                            stderr=null)
            subprocess.run('mv {} {}'.format(sexp_loc, output_folder), shell=True)
            return True, output_filename
        except:
            raise
        finally:
            # we remove the generated file anyway
            if not keep_sexp:
                subprocess.run('rm {}'.format(output_filename), shell=True)
            # Currently XLAT will generate the file in a fixed location
            if keep_sexp and os.path.normpath(sexp_loc) != os.path.normpath(output_filename):
                subprocess.run('rm -f {}'.format(sexp_loc), shell=True)

        return ok, output_filename

    """
    Takes .cpp as input, generate .v
    If any step fails, an exeption will be thrown
    """
    def generate_verilog(self, path, output_folder, verbose):
        succ_sexp, sexp_filename = self.generate_sexp(path, output_folder, keep_sexp=True, verbose=verbose)
        if succ_sexp:
            assert os.path.isfile(sexp_filename), 'Cannot find generated sexp_filename: {}'.format(sexp_filename)
            succ_v, v_filename = self.generate_verilog_from_sexp(sexp_filename, output_folder, keep_v=True, verbose=verbose)
            return succ_v, v_filename
        else:
            return False, None

