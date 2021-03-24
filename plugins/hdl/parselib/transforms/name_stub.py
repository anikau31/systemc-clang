from lark import Tree
import random
from types import LambdaType

class FuncNameStub(Tree):
    """Name stub for function"""
    def __init__(self, func_name, meta=None):
        super().__init__('func_name_stub', [func_name], meta)


class FuncParamNameStub(Tree):
    """Name stub for function parameter"""
    def __init__(self, param_name, tpe, meta=None):
        super().__init__('func_param_name_stub', [param_name, tpe], meta)

    def insert_name(self, name_list):
        id = 0
        pn = self.children[0]
        rn = pn + f'__ref_{id}'
        while rn in name_list:
            id += 1
            rn = pn + f'__ref_{id}'
        self.children[0] = rn


class ProcessVarNameStub(Tree):
    """Name stub for variables in a process"""
    def __init__(self, var_name, tpe, meta=None):
        super().__init__('process_var_name_stub', [var_name, tpe], meta)
        self.orig_name = var_name

    def insert_name(self, name_check):
        id = 0
        pn = self.children[0]
        rn = pn + f'__ref_{id}'
        if isinstance(name_check, LambdaType):
            while name_check(rn):
                id += 1
                rn = pn + f'__ref_{id}'
        else:
            raise ValueError('Unsupported name generation check for ProcessVarNameStub')
        self.children[0] = rn