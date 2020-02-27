"""Sanity tests for confirming that the tools are running"""
import pytest
from util.sexpdiff import sexpdiff
from util.vparser import VerilogParser
from shutil import copy
import os

def test_custom_sexp(tmpdir, customdriver, tool_output):
    conf = customdriver.conf
    test_name = conf.test_name
    filename = conf.get_module_name('{}.cpp'.format(test_name))
    output_folder = tmpdir
    copy(filename, output_folder)
    cpp_path = str(output_folder) + '/' + '{}.cpp'.format(test_name)
    assert os.path.isfile(cpp_path)
    res, filename = customdriver.generate_sexp(
        path=cpp_path,
        output_folder=output_folder,
        verbose=tool_output,
        keep_sexp=True
    )
    assert res, "should convert to sexpression"
    diff_res, diff_str = sexpdiff(
        filename,
        conf.get_golden_sexp_name('{}_hdl.txt'.format(test_name))
    )
    if diff_res:
        print(diff_str)
    assert not diff_res, 'should match golden standard'


def test_custom_verilog(tmpdir, customdriver, tool_output):
    conf = customdriver.conf
    test_name = conf.test_name
    filename = conf.get_module_name('{}.cpp'.format(test_name))
    output_folder = tmpdir
    res, filename = customdriver.generate_verilog(
        path=filename,
        output_folder=output_folder,
        verbose=tool_output
    )
    assert res, "should convert to Verilog from cpp"
    diff_info = VerilogParser.diff(
        filename,
        conf.get_golden_verilog_name('{}_hdl.txt.v'.format(test_name))
    )
    print(str(diff_info))
    assert diff_info is None, 'should be no diff in Verilog'


def test_custom_sexp_to_verilog(tmpdir, customdriver, tool_output):
    conf = customdriver.conf
    test_name = conf.test_name
    filename = conf.get_golden_sexp_name('{}_hdl.txt'.format(test_name))
    output_folder = tmpdir
    copy(filename, str(output_folder) + '/')
    res, filename = customdriver.generate_verilog_from_sexp(
        path=str(output_folder) + '/{}_hdl.txt'.format(test_name),
        output_folder=output_folder,
        keep_v=True,
        verbose=tool_output
    )
    assert res, "should convert to Verilog from sexp"
    print('filename: ', filename)
    print('golden: ', conf.get_golden_verilog_name('{}_hdl.txt.v'.format(test_name)))
    diff_info = VerilogParser.diff(
        filename,
        conf.get_golden_verilog_name('{}_hdl.txt.v'.format(test_name))
    )
    print(str(diff_info))
    assert diff_info is None, 'should be no diff in Verilog'

