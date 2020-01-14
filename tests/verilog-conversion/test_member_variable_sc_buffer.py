"""test for test_member_variable_sc_buffer.cpp"""
import os
from shutil import copy


def test_member_variable_sc_buffer_verilog(tmpdir, testfolderdriver, tool_output):
    """testing the conversion from cpp to sexp"""
    conf = testfolderdriver.conf
    output_folder = tmpdir
    # copy the file to tmpfolder
    copy(conf.get_module_name('member-variable-sc-buffer.cpp'), output_folder)

    filename = str(output_folder) + '/' + 'member-variable-sc-buffer.cpp'

    res, filename = testfolderdriver.generate_sexp(
        path=filename,
        output_folder=output_folder,
        verbose=tool_output,
        keep_sexp=True
    )

    with open(filename, 'r') as f:
        print(''.join(f.readlines()))

    assert res, 'should convert to sexpression'

    res, filename = testfolderdriver.generate_verilog_from_sexp(
        path=filename,  # sexp name
        output_folder=output_folder,
        verbose=tool_output,
        keep_v=True
    )

    assert res, 'should convert from sexpression to verilog'
