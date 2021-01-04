import pytest
from conftest import test_data

sysc_clang = __import__('systemc-clang')

@pytest.fixture
def syntax_error_cpp():
    return (
            '#include "systmc.h"\n'  # the header is not found
            'int main() {\n'
            '  return 0;\n'
            '}'
            )


def test_syntax_error(tmp_path, syntax_error_cpp, default_params):
    target_path = tmp_path / 'simple.cpp'
    with open(target_path, 'w') as f:
        f.writelines(syntax_error_cpp)

    with pytest.raises(sysc_clang.SystemCClangFatalError):
        target = sysc_clang.invoke_sysc([str(target_path), '--debug', '--'] + default_params)


def test_generate_file(tmp_path, simple_add_cpp, default_params):
    target_path = tmp_path / 'simple.cpp'
    with open(target_path, 'w') as f:
        f.writelines(simple_add_cpp)

    target = sysc_clang.invoke_sysc([str(target_path), '--debug', '--'] + default_params)
    assert Path(target).exist()


