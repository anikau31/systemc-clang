from contextlib import contextmanager
from lark.visitors import Interpreter
# visit_children_decor first visit the child and then execute the current function


class VariableBindings:
    variable_binding_stack = list()

    @staticmethod
    def get_type(name):
        for var_dict in reversed(VariableBindings.variable_binding_stack):
            if name in var_dict:
                return var_dict[name]
        return None


@contextmanager
def some_vars(var_dict):
    VariableBindings.variable_binding_stack.append(var_dict)
    yield
    VariableBindings.variable_binding_stack.pop()


class TopDown(Interpreter):
    def __default__(self, t):
        self.__push_up(t)
        return t

    def __push_up(self, current_node):
        current_node.children = self.visit_children(current_node)

    def __init__(self):
        pass

