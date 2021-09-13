import pytest
from conftest import testdata, zfpsynth, construct_hcode
from parselib.transforms import TopDown

sysc_clang = __import__('systemc-clang')

class SubmoduleInfoCollector(TopDown):
    def __init__(self):
        super().__init__()

class ChildModuleChecker(TopDown):
    def __init__(self, a):
        super().__init__()


@pytest.mark.parametrize("cpp_design_path, extra_args", [
    (testdata / 'test_child_module_binding.cpp', []),
])
def test_child_module_port_binding(tmp_path, cpp_design_path, extra_args, default_params, clang_args_params):
    t, hcode_target_path = construct_hcode(cpp_design_path, tmp_path, default_params, clang_args_params, extra_args)
    child_mod_info = SubmoduleInfoCollector()
    child_mod_info.visit(t)

    checker = ChildModuleChecker()
    checker.visit(t)
