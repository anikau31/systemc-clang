import pytest
import os
import subprocess
from pathlib import Path
from pytest_steps import test_steps

sysc_clang = __import__('systemc-clang')


# Handy paths
examples = Path(os.environ['SYSTEMC_CLANG']) / 'examples'
testdata = Path(os.environ['SYSTEMC_CLANG']) / 'tests' / 'verilog-conversion' / 'data'
zfpsynth = examples / 'llnl-examples' / 'zfpsynth' 
zfpshared2 = zfpsynth / 'shared2'


# Helper functions
def load_file(path):
    with open(path, 'r') as f:
        return f.read()


def load_module(mod_name):
    raise NotImplementedError


@pytest.fixture
def default_params():
    return ['-x', 'c++', '-w', '-c', '-std=c++14', '_-D__STDC_CONSTANT_MACROS',  '-D__STDC_LIMIT_MACROS', '-DRVD']


@pytest.fixture
def vivado_tcl_template():
    return (
            "create_project -in_memory -part xc7z020-clg400-1\n"
            "read_verilog -sv {}\n"
            "synth_design -flatten none -mode out_of_context -top _sc_module_0\n"
            "exit"
            )


test_data = [
    ('add',    load_file(testdata / 'add.cpp'), None),
    # ('z3test', load_file(zfpsynth / 'zfp3/z3test.cpp'), ['-I', zfpshared2.stem, '-I', zfpsynth / 'zfp3']),
    # ('z4test', load_file(zfpsynth / 'zfp4/z4test.cpp'), ['-I', zfpshared2.stem, '-I', zfpsynth / 'zfp4']),
    # ('z5test', load_file(zfpsynth / 'zfp5/z5test.cpp'), ['-I', zfpshared2.stem, '-I', zfpsynth / 'zfp5'])
]


@test_steps('hcode', 'translation', 'synthesis')
@pytest.mark.parametrize("name,content,extra_args", test_data, ids=[x[0] for x in test_data])
def test_translation(tmp_path, name, content, extra_args, default_params, vivado_tcl_template):
    # move files to the target directory
    target_path = tmp_path / '{}.cpp'.format(name)
    with open(target_path, 'w') as f:
        f.writelines(content)

    # step: hcode
    if extra_args is None:
        extra_args = []
    target = sysc_clang.invoke_sysc([str(target_path), '--debug', '--'] + default_params + extra_args)
    yield

    # step: translation
    sysc_clang.invoke_translation(target, [])
    yield

    # synthesis: synthesis
    tcl_path = tmp_path / '{}.tcl'.format(name)
    with open(tcl_path, 'w') as f:
        f.writelines(vivado_tcl_template.format(name + '_hdl.txt.v'))  # TODO: change this to customized suffix
    synth_res = subprocess.run(
            ['vivado', '-mode', 'batch', '-source',  str(tmp_path / '{}.tcl'.format(name))], 
            stdout=subprocess.PIPE, 
            stderr=subprocess.PIPE,
            cwd=tmp_path)
    assert synth_res.returncode == 0
    yield
