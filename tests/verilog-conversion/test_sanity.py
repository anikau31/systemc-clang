"""Sanity tests for confirming that the tools are running"""
import pytest
from util.sexpdiff import sexpdiff
from util.vparser import VerilogParser
from shutil import copy
import os

def test_sanity_add_sexp(tmpdir, sanitydriver, tool_output):
    conf = sanitydriver.conf
    filename = conf.get_module_name('module1.cpp')
    output_folder = tmpdir
    copy(filename, output_folder)
    cpp_path = str(output_folder) + '/' + 'module1.cpp'
    assert os.path.isfile(cpp_path)
    res, filename = sanitydriver.generate_sexp(
        path=cpp_path,
        output_folder=output_folder,
        verbose=tool_output,
        keep_sexp=True
    )
    assert res, "should convert to sexpression"
    diff_res, diff_str = sexpdiff(
        filename,
        conf.get_golden_sexp_name('module1_hdl.txt')
    )
    if diff_res:
        print(diff_str)
    assert not diff_res, 'should match golden standard'


def test_sanity_add_verilog(tmpdir, sanitydriver, tool_output):
    conf = sanitydriver.conf
    filename = conf.get_module_name('module1.cpp')
    output_folder = tmpdir
    res, filename = sanitydriver.generate_verilog(
        path=filename,
        output_folder=output_folder,
        verbose=tool_output
    )
    assert res, "should convert to Verilog from cpp"
    diff_info = VerilogParser.diff(
        filename,
        conf.get_golden_verilog_name('module1_hdl.txt.v')
    )
    print(str(diff_info))
    assert diff_info is None, 'should be no diff in Verilog'


def test_sanity_add_sexp_to_verilog(tmpdir, sanitydriver, tool_output):
    conf = sanitydriver.conf
    filename = conf.get_golden_sexp_name('module1_hdl.txt')
    output_folder = tmpdir
    copy(filename, str(output_folder) + '/')
    res, filename = sanitydriver.generate_verilog_from_sexp(
        path=str(output_folder) + '/module1_hdl.txt',
        output_folder=output_folder,
        keep_v=True,
        verbose=tool_output
    )
    assert res, "should convert to Verilog from sexp"
    print('filename: ', filename)
    print('golden: ', conf.get_golden_verilog_name('module1_hdl.txt.v'))
    diff_info = VerilogParser.diff(
        filename,
        conf.get_golden_verilog_name('module1_hdl.txt.v')
    )
    print(str(diff_info))
    assert diff_info is None, 'should be no diff in Verilog'

