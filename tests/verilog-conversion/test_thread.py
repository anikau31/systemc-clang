import pytest
from conftest import sysctestdata, testdata, zfpsynth, construct_hcode
from parselib.transforms import TopDown


sysc_clang = __import__('systemc-clang')


@pytest.mark.parametrize("cpp_design_path,extra_args", [ 
    (sysctestdata / 'thread-single-wait.cpp', [])
    ])
def test_thread_simple_wait(tmp_path, cpp_design_path, extra_args, default_params, clang_args_params):
    t, hcode_target_path = construct_hcode(cpp_design_path, tmp_path, default_params, clang_args_params, extra_args)

