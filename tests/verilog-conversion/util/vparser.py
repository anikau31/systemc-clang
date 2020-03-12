"""Testing utilities"""
import pyverilog.utils.version
from util.vdiff import VerilogASTDiff
from pyverilog.vparser.parser import parse

class VerilogParser(object):
    """parses Verilog code into AST, useful for comparing verilogs"""

    @staticmethod
    def parse(verilog_filename, include_list=None, define_list=None):
        """
        parses verilog code
        """
        if include_list is None:
            include_list = []
        if define_list is None:
            define_list = []
        ast, _ = parse([ verilog_filename ],
                preprocess_include=include_list,
                preprocess_define=define_list)
        return ast

    @staticmethod
    def diff(this_verilog_filename, that_verilog_filename, 
            include_list=None, define_list=None):
        """
        parses verilog_a and verilog_b and outputs their diff
        """
        this_ast = VerilogParser.parse(this_verilog_filename, include_list, define_list)
        that_ast = VerilogParser.parse(that_verilog_filename, include_list, define_list)

        # show the diff
        diff_info = VerilogASTDiff.diff_info(this_ast, that_ast)
        return diff_info

