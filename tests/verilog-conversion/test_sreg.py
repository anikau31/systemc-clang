import pytest
from util.sexpdiff import sexpdiff
from util.vparser import VerilogParser
from shutil import copy
import os


def test_sreg_sexp(tmpdir, llnldriver, tool_output):
    conf = llnldriver.conf
    filename = conf.get_module_name('sreg.cpp')
    output_folder = tmpdir
    copy(filename, output_folder)
    cpp_path = str(output_folder) + '/' + 'sreg.cpp'
    assert os.path.isfile(cpp_path)
    res, filename = llnldriver.generate_sexp(
        path=cpp_path,
        output_folder=output_folder,
        verbose=tool_output,
        keep_sexp=True
    )
    assert res, "should convert to sexpression"
    diff_res, diff_str = sexpdiff(
        filename,
        conf.get_golden_sexp_name('sreg_hdl.txt')
    )
    if diff_res:
        print(diff_str)
    assert not diff_res, 'should match golden standard'


def test_sreg_verilog(tmpdir, llnldriver, tool_output):
    conf = llnldriver.conf
    filename = conf.get_module_name('sreg.cpp')
    output_folder = tmpdir
    res, filename = llnldriver.generate_verilog(
        path=filename,
        output_folder=output_folder,
        verbose=tool_output
    )
    assert res, "should convert to Verilog from cpp"
    diff_info = VerilogParser.diff(
        filename,
        conf.get_golden_verilog_name('sreg_hdl.txt.v')
    )
    print(str(diff_info))
    assert diff_info is None, 'should be no diff in Verilog'


def test_sreg_sexp_to_verilog(tmpdir, llnldriver, tool_output):
    conf = llnldriver.conf
    filename = conf.get_golden_sexp_name('sreg_hdl.txt')
    output_folder = tmpdir
    copy(filename, str(output_folder) + '/')
    res, filename = llnldriver.generate_verilog_from_sexp(
        path=str(output_folder) + '/sreg_hdl.txt',
        output_folder=output_folder,
        keep_v=True,
        verbose=tool_output
    )
    assert res, "should convert to Verilog from sexp"
    print('filename: ', filename)
    print('golden: ', conf.get_golden_verilog_name('sreg_hdl.txt.v'))
    diff_info = VerilogParser.diff(
        filename,
        conf.get_golden_verilog_name('sreg_hdl.txt.v')
    )
    print(str(diff_info))
    assert diff_info is None, 'should be no diff in Verilog'

if __name__ == '__main__':
    test_sreg_verilog()
    test_sreg_sexp()
