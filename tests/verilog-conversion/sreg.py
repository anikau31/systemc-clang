import os
from llnl import run_verilog_matched_test

assert run_verilog_matched_test('sreg'), "sreg verilog should match golden standard"
