import pytest
import os
import subprocess
from pathlib import Path
from pytest_steps import test_steps
from conftest import test_data

sysc_clang = __import__('systemc-clang')


@pytest.fixture
def vivado_tcl_template():
    return (
            "create_project -in_memory -part xc7z020-clg400-1\n"
            "read_verilog -sv {}\n"
            "synth_design -flatten none -mode out_of_context -top _sc_module_0\n"
            "exit"
            )


@test_steps('hcode', 'translation', 'synthesis')
@pytest.mark.parametrize("name,content,extra_args", test_data, ids=[x[0] for x in test_data])
def test_translation(tmp_path, name, content, extra_args, default_params, vivado_tcl_template, has_vivado):
    # move files to the target directory
    target_path = tmp_path / '{}.cpp'.format(name)
    hcode_target_path = tmp_path / '{}_hdl.txt'.format(name)
    with open(target_path, 'w') as f:
        f.writelines(content)

    # step: hcode
    if extra_args is None:
        extra_args = []
    target = sysc_clang.invoke_sysc([str(target_path), '--debug', '--'] + default_params + extra_args)
    assert hcode_target_path.exists(), 'hCode txt should be present'
    yield

    # step: translation
    sysc_clang.invoke_translation(target, [])
    yield

    # synthesis: synthesis
    if has_vivado:
        tcl_path = tmp_path / '{}.tcl'.format(name)
        with open(tcl_path, 'w') as f:
            f.writelines(vivado_tcl_template.format(name + '_hdl.txt.v'))  # TODO: change this to customized suffix
        synth_res = subprocess.run(
                ['vivado', '-mode', 'batch', '-source',  str(tmp_path / '{}.tcl'.format(name))], 
                stdout=subprocess.PIPE, 
                stderr=subprocess.PIPE,
                cwd=tmp_path)
        assert synth_res.returncode == 0
    else:
        pytest.skip('Vivado is not found on this host, synthesis test is skipped')
    yield
