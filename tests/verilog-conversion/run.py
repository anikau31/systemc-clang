from driver import *
from util.vparser import VerilogParser
import tempfile
import contextlib

class TestingConfigurations(object):
    """parameters in this configuration might change"""

    @property
    def extra_header_folders(self):
        # SystemC Clang build llnl-examples
        return self.header_folders

    @property
    def positional_arguments(self):
        headers = []
        extra_header_folders = self.extra_header_folders
        for h in extra_header_folders:
            headers.extend(["-I", h])
        return headers

    def __init__(self, header_folders):
        self.header_folders = header_folders

class LLNLExampleTestingConfigurations(TestingConfigurations):

    def __init__(self, header_folders=None):
        if header_folders is None:
            header_folders = []
        this_folders = header_folders + [
            '/home/allen/working/systemc-clang/examples/llnl-examples/'
        ]
        super(LLNLExampleTestingConfigurations, self).__init__(this_folders)

@contextlib.contextmanager
def make_temp_directory():
    temp_dir = tempfile.mkdtemp()
    try:
        yield temp_dir
    finally:
        shutil.rmtree(temp_dir)

def main():
    conf = LLNLExampleTestingConfigurations()
    driver = SystemCClangDriver(conf=conf)
    filename='~/working/systemc-clang/tests/data/verilog-conversion/llnl-examples/sreg.cpp'
    # print(' '.join(driver.systemc_clang_commandline(filename=filename)))
    # tempfolder = tempfile.mkdtemp()
    # driver.generate_sexp(path=filename, output_folder='.', verbose=True, keep_sexp=True)
    # driver.generate_verilog_from_sexp(path='./sreg_hdl.txt', output_folder='.', verbose=True, keep_v=True)

    res = VerilogParser.diff('./sreg_hdl.txt.v', './sreg_hdl.txt.new.v')
    print('Are they same? ', res)

if __name__ == '__main__':
    main()

