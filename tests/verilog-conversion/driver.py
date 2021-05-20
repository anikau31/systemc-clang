"""Driver for using systemc-clang tool in python"""
import os
import re
import subprocess
from pathlib import Path
from shutil import move

class Result(object):
    def __init__(self):
        self.xlat_run = False
        self.xlat_match = False
        self.convert_run = False
        self.convert_syntax = False  # is syntax ok?
        self.convert_transform = False # is transform ok?
        self.convert_match = False

class SystemCClangDriver(object):
    """A clang driver class for running systemc-clang binaries
    """
    SYSTEMC_CLANG_BIN_PATH = os.environ['LLVM_INSTALL_DIR'] + "/bin/systemc-clang"
    PYTHON_CONVERT_TEMPLATE = 'python {}{}'.format(
        os.environ['SYSTEMC_CLANG'],
        "/plugins/hdl/hcode2verilog.py"
    )
    SYSTEMC_CLANG_ARGUMENTS = [
        "-I", "{}/include/".format(os.environ['SYSTEMC']),
        "-std=c++17",
        "-I", "/usr/include/",
        "-D__STDC_CONSTANT_MACROS",
        "-D__STDC_LIMIT_MACROS",
        "-x c++ -w -c"
    ]

    def systemc_clang_commandline(self, filename):
        """
        Extra args are positional and non positional arguments, for example,
        extra header etc
        """
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
        self.conf = conf

    def generate_verilog_from_sexp(self, path, output_folder, keep_v=False, verbose=False, keep_log=True, force=False):
        """
        Takes _hdl.txt as input, generate .v
        """
        result = Result()
        if not path.endswith('_hdl.txt'):
            raise RuntimeError('Filename should end with _hdl.txt')
        v_loc = path + '.v'
        # v_loc = os.path.abspath(v_loc)
        # if os.path.isfile(v_loc):
        #    raise RuntimeError('File to generate: {} exists'.format(v_loc))

        v_filename = os.path.basename(v_loc)
        output_filename = '{}/{}'.format(output_folder, v_filename)
        output_filename = os.path.abspath(output_filename)
        if os.path.isfile(output_filename) and not force:
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
            path,
            '--output',
            output_filename
        ])
        if verbose:
            print('cmdline', cmdline)
        try:
            res = subprocess.run(cmdline, 
                    stdout=subprocess.PIPE,  stderr=subprocess.PIPE,
                    shell=True)
            if res.returncode == 2:
                result.convert_run = True
            elif res.returncode == 3:
                result.convert_run = True
                result.convert_syntax = True
            elif res.returncode == 0:
                result.convert_run = True
                result.convert_syntax = True
                result.convert_transform = True
            if keep_log:
                with open(output_folder + '/hcode2verilog.py.stdout', 'wb') as f:
                    f.write(res.stdout)
                with open(output_folder + '/hcode2verilog.py.stderr', 'wb') as f:
                    f.write(res.stderr)
            if os.path.isfile(output_filename):
                result.convert_run = True
                return result, output_filename
            else:
                return result, None
        except:
            raise
        finally:
            if not keep_v:
                subprocess.run('rm {}'.format(output_filename), shell=True)
            # if keep_v and os.path.normpath(v_loc) != os.path.normpath(output_filename):
            #     subprocess.run('rm -f {}'.format(output_filename), shell=True)

    """
    Takes .cpp as input, generate _hdl.txt
    """
    def generate_sexp(self, path, output_folder, keep_sexp=False, verbose=False, keep_log=True, force=False):
        cmdline = self.systemc_clang_commandline(filename=path)
        result = Result()
        if path.endswith('.cpp'):
            sexp_loc = re.sub(".cpp$", "_hdl.txt", path)
        elif path.endswith('.hpp'):
            sexp_loc = re.sub(".hpp$", "_hdl.txt", path)
        else:
            raise RuntimeError('Filename should end with .cpp or .hpp')

        if os.path.isfile(sexp_loc) and not force:
            raise RuntimeError('File to generate: {} exists'.format(sexp_loc))

        sexp_filename = os.path.basename(sexp_loc)
        output_filename = os.path.abspath('{}/{}'.format(output_folder, sexp_filename))
        if os.path.isfile(output_filename) and not force:
            raise RuntimeError('File to generate: {} exists'.format(output_filename))

        if verbose:
            print('path: ', path)
            print('sexp_loc: ', sexp_loc)
            print('sexp_filename: ', sexp_filename)
            print('output_filename: ', output_filename)
        try:
            if verbose:
                print('cmd: ', ' '.join(cmdline))
            res = subprocess.run(' '.join(cmdline), shell=True, 
                    stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE)
            if keep_log:
                with open(output_folder + '/systemc-clang.stdout', 'wb') as f:
                    f.write(res.stdout)
                with open(output_folder + '/systemc-clang.stderr', 'wb') as f:
                    f.write(res.stderr)
            if res.returncode != 0:
                return result, None
            move_required = os.path.normpath(sexp_loc) != os.path.normpath(output_filename)
            if os.path.isfile(sexp_loc):
                if move_required:
                    move(sexp_loc, output_filename)
                # subprocess.run('mv {} {}'.format(sexp_loc, output_folder), shell=True)
                result.xlat_run = True
                return result, output_filename
            else:
                print("Failed Path 1")
                return result, None
        except:
            raise
        finally:
            # we remove the generated file anyway
            if not keep_sexp:
                subprocess.run('rm {}'.format(output_filename), shell=True)
            # Currently XLAT will generate the file in a fixed location
            if keep_sexp and os.path.normpath(sexp_loc) != os.path.normpath(output_filename):
                subprocess.run('rm -f {}'.format(sexp_loc), shell=True)

        return result, None

    """
    Takes .cpp as input, generate .v
    If any step fails, an exeption will be thrown
    """
    def generate_verilog(self, path, output_folder, verbose, force=False):
        result, sexp_filename = self.generate_sexp(path, output_folder, keep_sexp=True, verbose=verbose, force=force)
        if result.xlat_run:
            assert os.path.isfile(sexp_filename), 'Cannot find generated sexp_filename: {}'.format(sexp_filename)
            result_verilog, v_filename = self.generate_verilog_from_sexp(sexp_filename, output_folder, keep_v=True, verbose=verbose, force=force)
            result.convert_run = result_verilog.convert_run
            result.convert_syntax = result_verilog.convert_syntax
            result.convert_transform = result_verilog.convert_transform
            return result, v_filename
        else:
            return result, None

