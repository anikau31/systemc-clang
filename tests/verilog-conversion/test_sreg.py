import os
from llnl import run_verilog_matched_test, run_sexp_matched_test

def test_sreg_sexp():
    assert run_sexp_matched_test('sreg'), "sreg sexp should match golden standard"

def test_sreg_verilog():
    assert run_verilog_matched_test('sreg'), "sreg verilog should match golden standard"

if __name__ == '__main__':
    test_sreg_verilog()
    test_sreg_sexp()
