import pytest
from util.sexpdiff import sexpdiff
from util.vparser import VerilogParser
from shutil import copy
import os

f = pytest.sc_log

@pytest.mark.verilog
def test_zfp2_verilog(request, tmpdir, llnldriver, tool_output):
    testname = request.node.name
    conf = llnldriver.conf
    filename = conf.get_module_name('zfpsynth/zfp2/test.cpp')
    print(filename)
    output_folder = tmpdir
    res, filename = llnldriver.generate_verilog(
        path=filename,
        output_folder=output_folder,
        verbose=tool_output
    )
    f.writelines(f'{testname},xlat_run,{res.xlat_run}\n')
    f.writelines(f'{testname},convert_run,{res.convert_run}\n')
    f.writelines(f'{testname},convert_syntax,{res.convert_syntax}\n')
    f.writelines(f'{testname},convert_transform,{res.convert_transform}\n')
    assert res.convert_run and res.xlat_run, "should convert to Verilog from cpp"
    diff_info = VerilogParser.diff(
        filename,
        conf.get_golden_verilog_name('sreg_hdl.txt.v')
    )
    print(str(diff_info))
    assert diff_info is None, 'should be no diff in Verilog'
