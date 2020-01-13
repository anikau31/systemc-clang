from driver import *
from util.vparser import VerilogParser
from util.conf import LLNLExampleTestingConfigurations
import tempfile

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

