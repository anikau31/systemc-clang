import pytest
from conftest import testdata, zfpsynth, construct_hcode
from parselib.transforms import TopDown


sysc_clang = __import__('systemc-clang')


class FunctionCollector(TopDown):
    """
    This class maintains the functions within a scope (module)
    """
    def __init__(self):
        super().__init__()
        self.current_module = None
        self.functions = {}

    def hmodule(self, tree):
        self.current_module = tree.children[0]
        self.__push_up(tree)
        self.current_module = None
        return tree

    def hfunction(self, tree):
        """
        we shall not have nested function declarations
        """
        self.functions.setdefault(self.current_module, []).append(tree.children[0])
        return tree


class MethodCallChecker(TopDown):
    def __init__(self, functions, path):
        super().__init__()
        self.functions = functions
        self.path = path

    def hmodule(self, tree):
        self.current_module = tree.children[0]
        self.__push_up(tree)
        self.current_module = None
        return tree

    def hmethodcall(self, tree):
        self.__push_up(tree)
        assert tree.children[0] in self.functions[self.current_module], \
                'Expected method {} not found in module {}, line: {}, file: {}'.format(
                        tree.children[0], 
                        self.current_module, 
                        str(tree.meta.line), 
                        self.path)
        return tree


@pytest.mark.parametrize("cpp_design_path, extra_args", [
    (testdata / 'test_binary_iscs.cpp', []),
    # (zfpsynth / 'zfp3/z3test.cpp', ["-I{}".format(zfpsynth/"zfp3")]) 
])
def test_hmethod_call(tmp_path, cpp_design_path, extra_args, default_params, clang_args_params):
    """
    hMethodCall should be formatted correctly and only calls method that is present in module scope
    """
    t, hcode_target_path = construct_hcode(cpp_design_path, tmp_path, default_params, clang_args_params, extra_args)

    # collect functions
    fc = FunctionCollector()
    fc.visit(t)
    functions = fc.functions

    # check function/method calls
    checker = MethodCallChecker(functions, path=hcode_target_path)
    checker.visit(t)

