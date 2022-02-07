"""collect function declaration information and invocation information in processes"""
from lark import Token

from parselib.transforms import TopDown
from ..utils import dprint, is_tree_type, is_tree_types
from ..grammar import UnexpectedHCodeStructureError
from .name_stub import  ProcessVarNameStub
from warnings import warn
from pprint import pprint
from copy import deepcopy


class FunctionInfoPass(TopDown):
    """
    This pass tags inout IDs assigned within the function parameters and marks the position of the inout params.
    These information is useful for the next pass which translates the functions and their invocations
    """
    def __init__(self):
        super().__init__()
        self.__current_module = None
        self.__local_output_ids = set()
        self.__in_function = False
        self.function_nodes = dict()  # record the function nodes per modules
        self.__current_process = None
        self.__current_function = None

    @property
    def current_module_function_nodes(self):
        return self.function_nodes[self.__current_module]

    @current_module_function_nodes.setter
    def current_module_function_nodes(self, val):
        self.function_nodes[self.__current_module] = val

    @property
    def current_process_function_invocations(self):
        return self.__current_process.function_invocations

    @current_process_function_invocations.setter
    def current_process_function_invocations(self, val):
        self.__current_process.function_invocations = val

    @property
    def current_function_assignments(self):
        return self.__current_function.assignments

    @property
    def current_function_function_invocations(self):
        return self.__current_function.function_invocations

    @property
    def current_scope_function_invocations(self):
        if self.__current_function:
            return self.__current_function.function_invocations
        elif self.__current_process:
            return self.__current_process.function_invocations
        else:
            raise ValueError('Current scope is not set')

    @current_function_assignments.setter
    def current_function_assignments(self, val):
        self.__current_function.assignments = val

    def hprocess(self, tree):
        self.__current_process = tree
        tree.function_invocations = []
        self.current_process_function_invocations = tree.function_invocations
        self.__push_up(tree)
        self.__current_process = None
        return tree

    def hmodule(self, tree):
        self.__current_module = tree.children[0]
        self.current_module_function_nodes = []
        self.__push_up(tree)
        tree.function_nodes = self.current_module_function_nodes
        return tree


    def hfunctionparams(self, tree):
        tree.io_params = []
        tree.func_param_var_name = []
        for idx, param in enumerate(tree.children):
            var_name = param.children[0]
            if param.data == 'funcparamio':  # filter io params
                self.__local_output_ids.add(var_name)
                tree.io_params.append((idx, var_name))
            tree.func_param_var_name.append(var_name)
        return tree

    def hfunctionlocalvars(self, tree):
        tree.local_var_name = []
        for var_decl in tree.children:
            tree.local_var_name.append(var_decl.children[0])
        return tree

    def hfunctionbody(self, tree):
        self.__push_back(tree)
        return tree

    def hvarref(self, tree):
        if hasattr(tree, 'func_repl_id'):
            if tree.children[0] in self.__local_output_ids:
                # variable references where they should be changed to a different name
                # dprint(tree)
                pass
            else:
                # remove repl_id tags for local variables
                delattr(tree, 'func_repl_id')
        return tree

    def harrayref(self, tree):
        # we only expands the first parameter
        array_ref = tree.children[0]
        if self.__in_function and hasattr(tree, 'push_down'):
            array_ref.func_repl_id = True
            delattr(tree, 'push_down')
        self.__push_up(tree)
        return tree

    def hslice(self, tree):
        array_ref = tree.children[0]
        if self.__in_function:
            array_ref.func_repl_id = True
        self.__push_up(tree)
        return tree

    def blkassign(self, tree):
        """
        func_repl_id being true means that this ID should be replaced by another name,
        because the ID crosses the boundary of parameters and need special handlings for
        blocking/non-blocking assignments
        """
        lhs = tree.children[0]
        if lhs.data == 'hvarref' and self.__in_function:
            lhs.func_repl_id = True
        elif lhs.data == 'harrayref' and self.__in_function:
            lhs.push_down = True
        self.__push_up(tree)
        if self.__in_function:
            self.current_function_assignments.append(tree)
        return tree

    def hfunction(self, tree):
        self.__in_function = True
        self.__current_function = tree
        tree.function_invocations = []
        self.current_function_assignments = []
        self.__push_up(tree)
        self.__in_function = False
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
        self.current_module_function_nodes.append(tree)
        self.__current_function = None
        return tree

    def hmethodcall(self, tree):
        self.__push_up(tree)
        self.current_scope_function_invocations.append(tree)
        return tree


class FunctionInfoPass2(TopDown):
    """Similar to FunctionInfoPass, but this pass collects invocation information"""
    def __init__(self):
        super().__init__()
        self.__current_module = None
        self.__current_process = None
        self.__current_function = None
        self.__scope_stack = []
        self.__phantom_id = 0

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

    @property
    def current_function_nodes(self):
        return self.__current_module.function_nodes

    def __search_current_function(self, func_name):
        for f in self.current_function_nodes:
            if f.children[0] == func_name:
                return f
        # try fuzzy search
        for f in self.current_function_nodes:
            # dprint(f.children[0][:-1] == func_name[:-1])
            if f.children[0][:-1] == func_name[:-1]:
                return f
        raise ValueError(f'Function {func_name} not found')

    def push_scope(self):
        self.__scope_stack.append(dict())

    def search_id_def(self, id, allow_failure=False):
        for stk in reversed(self.__scope_stack):
            if id in stk:
                return stk[id]
        if not allow_failure:
            raise ValueError(f'ID {id} not found in current scope')
        return None

    def copy_and_strip_io(self, tp):
        tpe = deepcopy(tp)
        return tpe

    def add_id_type(self, id, tpe):
        if id in self.__scope_stack[-1]:
            raise ValueError('Redefinition of variable')
        self.__scope_stack[-1][id] = self.copy_and_strip_io(tpe)

    def portdecltype(self, tree):
        self.add_id_type(tree.children[0].children[0], tree.children[1])
        return tree

    def sigdecltype(self, tree):
        self.add_id_type(tree.children[0].children[0], tree.children[1])
        return tree

    def vardeclinit(self, tree):
        # dprint(self.__current_function.children[0] if self.__current_function else None)
        # dprint(self.__current_process.children[0] if self.__current_process else None)
        # dprint(self.__current_module.children[0] if self.__current_module else None)
        self.add_id_type(tree.children[0], tree.children[1])
        return tree

    def funcparamio(self, tree):
        self.add_id_type(tree.children[0], tree.children[1])
        return tree

    def funcparami(self, tree):
        self.add_id_type(tree.children[0], tree.children[1])
        return tree

    def pop_scope(self):
        self.__scope_stack.pop(-1)

    def hmodule(self, tree):
        self.__current_module = tree
        self.push_scope()
        self.__push_up(tree)
        self.pop_scope()
        self.__current_module = None
        return tree

    @property
    def current_phantom_name(self):
        return '__phantom_var_{}'.format(self.__phantom_id)

    def new_phantom_var_name(self, tpe):
        while self.search_id_def(self.current_phantom_name, True) is not None:
            self.__phantom_id += 1
        phantom_var = self.current_phantom_name
        self.add_id_type(phantom_var, tpe)
        return phantom_var

    def __extract_name_from_method_args(self, arg_node, arg_tpe):
        if is_tree_type(arg_node, 'hvarref'):
            return arg_node.children[0]
        elif is_tree_types(arg_node, ['hliteral', 'hbinop', 'hmethodcall', 'syscread']):
            warn('Complex expression or constant used for in/out argument, '
                 'this will cause phantom argument to be created and the output result will be discarded. '
                 'Consider using non-reference or const-reference instead')
            phantom_var = self.new_phantom_var_name(arg_tpe)
            arg_node.phantom_var = phantom_var
            return phantom_var
        elif isinstance(arg_node, str):  # strings can be treated as an ID
            return arg_node
        raise ValueError('Unsupported form of function argument')

    def augment_name_stub(self, tree):
        tree.name_stub = dict()
        tree.phantom_vars = dict()
        names_to_stub = set()


        for invoc in tree.function_invocations:
            invoc_params = invoc.children[1:]
            f_name = invoc.children[0]
            func_node = self.__search_current_function(f_name)
            func_name, ret_type, func_params, local_vars, func_body = self.__extract_func_def(func_node)
            if func_params is not None:
                for idx, param in func_params.io_params:
                    arg_node = invoc_params[idx]
                    tpe = func_params.children[idx].children[1]
                    name = self.__extract_name_from_method_args(arg_node, tpe)
                    if hasattr(arg_node, 'phantom_var'):
                        tree.phantom_vars[name] = tpe
                    names_to_stub.add(name)
        # dprint(tree.phantom_vars)
        # actually, we can finalized the name of the stub here
        for nm in names_to_stub:
            stub = ProcessVarNameStub(nm, self.search_id_def(nm))
            stub.insert_name(lambda x: self.search_id_def(x, True))
            tree.name_stub[nm] = stub

    def hprocess(self, tree):
        # dprint(tree.children[0], tree.pretty())
        self.__current_process = tree
        self.push_scope()
        self.__push_up(tree)
        self.augment_name_stub(tree)
        self.pop_scope()
        self.__current_process = None
        return tree

    def hfunction(self, tree):
        self.__current_function = tree
        self.push_scope()
        self.__push_up(tree)

        # func_name, ret_type, func_params, local_vars, func_body = self.__extract_func_def(tree)
        # dprint(func_params.pretty())

        self.augment_name_stub(tree)
        self.pop_scope()
        self.__current_function = None
        return tree
