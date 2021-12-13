"""transform functions such that have separate the parameters where input and output are separate"""
from lark import Token

from parselib.transforms import TopDown
from ..utils import dprint, is_tree_type
from ..grammar import UnexpectedHCodeStructureError
from .name_stub import *
import copy


class FunctionTransformationPass(TopDown):
    """
    Traverse all usage of function calls, convert function names into stubs (for duplicating functions)
    And add prologue and epilogue for function calls
    """
    def __init__(self):
        super().__init__()
        self.__current_functions = []
        self.__current_function = None
        self.__current_process = None
        self.__func_param_stubs = None
        self.__in_module_scope = False
        self.__current_module_scope_vars = None
        self.__current_module_sense_list = None
        self.__sense_list_proc_name = None

    def hsenslist(self, tree):
        proc_name = tree.children[0]
        self.__sense_list_proc_name = proc_name
        self.__current_module_sense_list[proc_name] = set()
        self.__push_up(tree)
        self.__sense_list_proc_name = None
        return tree

    def hsensvar(self, tree):
        self.__current_module_sense_list[self.__sense_list_proc_name].add(tree.children[0])
        return tree

    @property
    def current_module_function_nodes(self):
        return self.__current_functions

    @current_module_function_nodes.setter
    def current_module_function_nodes(self, val):
        self.__current_functions = val

    def __add_func_name_stub_to_current_process(self, func):
        self.__current_process.func_name_stubs.append(FuncNameStub(func))

    def __search_current_function(self, func_name):
        for f in self.current_module_function_nodes:
            if f.children[0] == func_name:
                return f
        # try fuzzy search
        for f in reversed(self.current_module_function_nodes):
            # dprint(f.children[0][:-1] == func_name[:-1])
            if f.children[0][:-1] == func_name[:-1]:
                return f
        raise ValueError(f'Function {func_name} not found')

    def __get_current_process_stub(self, id):
        if self.__current_process:
            if id in self.__current_process.name_stub:
                return self.__current_process.name_stub[id]
            else:
                return None

    def modportsiglist(self, tree):
        self.__in_module_scope = True
        self.__push_up(tree)
        self.__in_module_scope = False
        return tree

    def sigdecltype(self, tree):
        if self.__in_module_scope:
            if self.__in_module_scope:
                var = tree.children[0].children[0]
                self.__current_module_scope_vars[var] = True
        return tree

    def vardeclinit(self, tree):
        if self.__in_module_scope:
            if self.__in_module_scope:
                var = tree.children[0]
                self.__current_module_scope_vars[var] = True
        return tree

    def portdecltype(self, tree):
        if self.__in_module_scope:
            if self.__in_module_scope:
                var = tree.children[0].children[0]
                self.__current_module_scope_vars[var] = True
        return tree

    def hmodule(self, tree):
        # for t in tree.children:
        #     if isinstance(t, Tree):
        #         dprint(t.data)
        self.__current_module = tree
        self.__current_module_scope_vars = dict()
        self.__current_module_sense_list = dict()
        self.current_module_function_nodes = tree.function_nodes
        self.__push_up(tree)
        self.__current_module_scope_vars = None
        self.__current_module_sense_list = None
        self.__current_module = None
        return tree

    def hprocess(self, tree):
        self.__current_process = tree  # note down the process so that we can add temporary variables later
        tree.func_name_stubs = []
        self.__push_up(tree)
        self.__current_process = None

        # generate prolog
        body = None
        for ch in tree.children:
            if is_tree_type(ch, 'hcstmt'):
                body = ch
                break
        if len(body.children) == 0:
            body.children.append(Tree('stmts', children=[]))
        body = body.children[0]
        for name, stub in tree.name_stub.items():
            blkassign = Tree('blkassign', children=[
                stub.children[0],
                name
            ])
            body.children.insert(0, Tree('stmt', children=[blkassign]))

        # generate epilog
        for name, stub in tree.name_stub.items():
            blkassign = Tree('blkassign', children=[
                name,
                stub.children[0]
            ])
            body.children.append(Tree('stmt', children=[blkassign]))
        return tree

    def hfunctionlocalvars(self, tree):
        stub_decl = []
        for name, stub in self.__current_function.name_stub.items():
            stub_decl.append(
                Tree('vardeclinit', children=[stub.children[0], stub.children[1]])
            )
        for name, tpe in self.__current_function.phantom_vars.items():
            t = self.strip_io_or_default(tpe)
            stub_decl.append(
                Tree('vardeclinit', children=[name, t])
            )
        tree.children.extend(stub_decl)
        return tree

    def strip_io_or_default(self, tpe):
        # htypeinfo htype '__func_inout' *args
        # htypeinfo htype *args
        t = tpe
        if tpe.children[0].children[0] in ['__func_inout', 'sc_in']:
            t = copy.deepcopy(tpe)
            t.children = t.children[0].children[1:]
        return t


    def prevardecl(self, tree):
        stub_decl = []
        for name, stub in self.__current_process.name_stub.items():
            stub_decl.append(
                Tree('vardecl', children=[
                    Tree('vardeclinit', children=[ stub.children[0], stub.children[1] ])
                ]))
        for name, tpe in self.__current_process.phantom_vars.items():
            t = self.strip_io_or_default(tpe)
            stub_decl.append(
                Tree('vardecl', children=[
                    Tree('vardeclinit', children=[name, t])
                ]))
        tree.children.extend(stub_decl)
        return tree

    def __extract_func_def(self, tree):
        func_name = tree.children[0]
        ret_type = None
        func_params = None
        local_vars = None
        func_body = None
        for child in tree.children[1:]:
            if child.data == 'hfunctionparams':
                func_params = child
            elif child.data == 'hfunctionrettype':
                ret_type = child
            elif child.data == 'hfunctionlocalvars':
                local_vars = child
            elif child.data == 'hfunctionbody':
                func_body = child
        return func_name, ret_type, func_params, local_vars, func_body

    def __extract_id_from_func_arg(self, tree):
        if hasattr(tree, 'phantom_var'):
            return tree.phantom_var
        elif isinstance(tree, str):
            return tree
        return tree.children[0]

    @property
    def current_scope_object(self):
        if self.__current_process:
            return self.__current_process
        elif self.__current_function:
            return self.__current_function
        else:
            return None

    def add_func_name_stub_to_current_scope(self, func):
        self.current_scope_object.func_name_stubs.append(FuncNameStub(func))

    def check_blocking(self, var_name):
        in_global_scope = var_name in self.__current_module_scope_vars
        proc_name = self.__current_process.children[0]
        in_sens_list = var_name in self.__current_module_sense_list[proc_name]
        if not in_global_scope or (in_global_scope and in_sens_list):
            return True
        else:
            return False



    def hmethodcall(self, tree):
        # we translate methods manually
        # dprint(tree.pretty())
        orig_len = len(tree.children)


        # tree.children = [
        #     self.hmethodcall(x) if is_tree_type(x, 'hmethodcall') else x for x in tree.children
        # ]  # for other vars, we will need to process the stubs
        # self.__push_up(tree)
        # dprint(tree.pretty())
        func_name = tree.children[0]
        self.add_func_name_stub_to_current_scope(func_name)
        orig_func_name = func_name
        # dprint('Before: ', func_name)
        func_node = self.__search_current_function(func_name)
        func_name, ret_type, func_params, local_vars, func_body = self.__extract_func_def(func_node)
        # dprint('After: ', func_name)
        extra_args = []
        func_args = tree.children[1:]
        if func_params is not None:
            for idx, _ in func_params.io_params:
                arg = self.__extract_id_from_func_arg(func_args[idx])
                stub = self.current_scope_object.name_stub[arg]
                extra_args.append(Tree('hvarref', children=[stub.children[0]]))
        tree.children.extend(extra_args)
        # fuzzy
        if orig_func_name != func_name:
            tree.children[0] = func_name
        for idx in range(orig_len):
            arg = tree.children[idx]
            if idx == 0:
                continue
            if is_tree_type(arg, 'hmethodcall'):
                self.hmethodcall(arg)
            elif isinstance(arg, str) and self.check_blocking(arg):
                stub = self.__get_current_process_stub(arg)
                if stub:
                    tree.children[idx] = stub.children[0]
        # dprint(tree.pretty())
        return tree

    def hfunctionparams(self, tree):
        self.__push_up(tree)
        return tree

    def __change_type_to_funcinput(self, tree):
        tree.children[1].children[0].children[0] = 'sc_in'

    def __get_func_param_stub(self, id):
        return self.__func_param_stubs[id]

    def hvarref(self, tree):
        if hasattr(tree, 'func_repl_id'):
            tree.children[0] = self.__get_func_param_stub(tree.children[0])
        else:  # check for stub in process
            stub = self.__get_current_process_stub(tree.children[0])
            if stub:
                tree.children[0] = stub.children[0]

        return tree

    def harrayref(self, tree):
        self.__push_up(tree)
        return tree

    def hfunction(self, tree):
        tree.func_name_stubs = []
        self.__current_function = tree
        func_name, ret_type, func_params, local_vars, func_body = self.__extract_func_def(tree)
        extra_params = []
        self.__func_param_stubs = dict()
        if func_params is not None:
            for idx, par in func_params.io_params:
                tpe = func_params.children[idx].children[1]
                # dprint(func_params.children[idx].pretty())
                stub = FuncParamNameStub(par, copy.deepcopy(tpe))
                extra_params.append(stub)
                self.__change_type_to_funcinput(func_params.children[idx])
                self.__func_param_stubs[par] = stub
        if extra_params:
            # NOTE: this condition must be present as it implies that func_params is not None
            func_params.children.extend(extra_params)
        for assignment in tree.assignments:
            pass
            # dprint(assignment.pretty())
        self.__push_up(tree)
        self.__current_function = None
        return tree

