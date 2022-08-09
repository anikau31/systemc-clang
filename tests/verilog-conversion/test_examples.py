import pytest
import os
import subprocess
import difflib
import shutil
from pathlib import Path
from pytest_steps import test_steps
from conftest import test_data

sysc_clang = __import__('systemc-clang')


def check_text_same(data, golden, target_path):
    """check if data is the same as golden, if not, raise an error and print relative information"""
    golden_lines = golden.splitlines(keepends=True)
    target_lines = data.splitlines(keepends=True)
    seq_matcher = difflib.SequenceMatcher(lambda x: x in " \t\n\r", golden, data)
    ops = list(seq_matcher.get_opcodes())
    cond = len(ops) == 1 and ops[0][0] == 'equal'
    if not cond:
        differ = difflib.Differ(charjunk=lambda x: x in " \t\n\r")
        diff_res = list(differ.compare(golden_lines, target_lines))
        print("OPS to correct the results: ")
        for tag, i1, i2, j1, j2 in ops:
            print('{:7}   a[{}:{}] --> b[{}:{}] {!r:>8} --> {!r}'.format( 
                tag, i1, i2, j1, j2, golden[i1:i2], data[j1:j2]))
            assert False, '\nTranslated file: \n' + str(target_path) + '\nTranslated file and golden file mismatch, diff result:\n' + ''.join(diff_res)


@pytest.fixture
def vivado_tcl_template():
    return (
            "create_project -in_memory -part xczu3eg-sbva484-1-i\n"
            "read_verilog -sv {}\n"
            "synth_design -flatten none -mode out_of_context -top {}\n"
            "exit"
            )


@test_steps('hcode', 'translation', 'translation-match-check', 'synthesis')
@pytest.mark.parametrize("name,content,extra_args,golden,golden_hcode,copy_back_path,top_name", test_data, ids=[x[0] for x in test_data])
def test_translation(tmp_path, name, content, extra_args, golden, golden_hcode, copy_back_path, default_params, clang_args_params, vivado_tcl_template, has_vivado, top_name):
    # move files to the target directory
    target_path = tmp_path / '{}.cpp'.format(name)
    hcode_target_path = tmp_path / '{}_hdl.txt'.format(name)
    verilog_target_path = tmp_path / '{}_hdl.txt.v'.format(name)
    with open(target_path, 'w') as f:
        f.writelines(content)

    # step: hcode
    if extra_args is None:
        extra_args = []
    args = [str(target_path), '--debug', '--'] + list(default_params) + list(clang_args_params) + list(map(str, extra_args))
    print(' '.join(args))
    target = sysc_clang.invoke_sysc(args)
    assert hcode_target_path.exists(), 'hCode txt should be present'
    # We need to copy this back to the build directory
    if copy_back_path:
        shutil.copy(hcode_target_path, copy_back_path)


    if golden_hcode is not None:
        # check for the code
        data = Path(hcode_target_path).read_text()
        check_text_same(data, golden_hcode, hcode_target_path)
    yield

    # step: translation
    print("Generated _hdl.txt file at: {}".format(target))
    sysc_clang.invoke_translation(target, [])
    assert verilog_target_path.exists(), 'Verilog should be present'
    if copy_back_path:
        shutil.copy(verilog_target_path, copy_back_path)
    yield


    # step: translation-match (checks whether the translation matches a golden file, if exists)
    if golden is None:
        yield
        # pytest.skip('Golden Verilog file is not included, translation-match test is skipped')
    else:
        with open(verilog_target_path, 'r') as f:
            data = f.read()
        golden = golden.strip()
        data = data.strip()
        golden_lines = golden.splitlines(keepends=True)
        target_lines = data.splitlines(keepends=True)
        seq_matcher = difflib.SequenceMatcher(lambda x: x in " \t\n\r", golden, data)
        ops = list(seq_matcher.get_opcodes())
        cond = len(ops) == 1 and ops[0][0] == 'equal'
        if not cond:
            differ = difflib.Differ(charjunk=lambda x: x in " \t\n\r")
            diff_res = list(differ.compare(golden_lines, target_lines))
            print("OPS to correct the results: ")
            for tag, i1, i2, j1, j2 in ops:
                print('{:7}   a[{}:{}] --> b[{}:{}] {!r:>8} --> {!r}'.format( 
                    tag, i1, i2, j1, j2, golden[i1:i2], data[j1:j2]))
                assert False, '\nTranslated file: \n' + str(verilog_target_path) + '\nTranslated file and golden file mismatch, diff result:\n' + ''.join(diff_res)
        yield


    # synthesis: synthesis
    if has_vivado and top_name is not None:
        tcl_path = tmp_path / '{}.tcl'.format(name)
        with open(tcl_path, 'w') as f:
            f.writelines(vivado_tcl_template.format(name + '_hdl.txt.v', top_name))  # TODO: change this to customized suffix
        synth_res = subprocess.run(
                ['vivado', '-mode', 'batch', '-source',  str(tmp_path / '{}.tcl'.format(name))], 
                stdout=subprocess.PIPE, 
                stderr=subprocess.PIPE,
                cwd=tmp_path)
        assert synth_res.returncode == 0
    else:
        pytest.skip('Vivado is not found on this host, synthesis test is skipped')
    yield
