import os
import sys
from llnl import run_verilog_matched_test, run_sexp_matched_test

def test_sreg_sexp(args=None):
    assert run_sexp_matched_test('sreg', args), "sreg sexp should match golden standard"

def test_sreg_verilog(args=None):
    assert run_verilog_matched_test('sreg', args), "sreg verilog should match golden standard"

if __name__ == '__main__':
    test_sreg_verilog(args)
    test_sreg_sexp(args)
