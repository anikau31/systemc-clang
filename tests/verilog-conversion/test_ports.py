import pytest
from conftest import testdata, zfpsynth, construct_hcode
from parselib.transforms import TopDown


sysc_clang = __import__('systemc-clang')

class PortLocationChecker(TopDown):
    """
    This class maintains the functions within a scope (module)
    """
    def __init__(self):
        super().__init__()

    def hvardecl(self, tree):
        self.is_in_vardecl = True
        self.__push_up(tree)
        self.is_in_vardecl = False

        return tree

    def htype(self, tree):
        self.__push_up(tree)
        assert tree.children[0] not in ['sc_in', 'sc_out'], 'Input/Output port should only be present in hPortin/hPortout node and not hVardecl node'

@pytest.mark.parametrize("cpp_design_path, extra_args", [
    (testdata / 'test_port_location.cpp', []),
])
def test_port_location(tmp_path, cpp_design_path, extra_args, default_params, clang_args_params):
    # pointers to port should reside in port list

    t, hcode_target_path = construct_hcode(cpp_design_path, tmp_path, default_params, clang_args_params, extra_args)
    checker = PortLocationChecker()
    checker.visit(t)
