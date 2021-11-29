import warnings

from lark import Token

from parselib.transforms import TopDown
from ..utils import dprint, is_tree_type
from ..grammar import UnexpectedHCodeStructureError


class LiteralExpansion(TopDown):
    """Expands integer literal into int"""
    def __init__(self, structure):
        super().__init__()
        self.structure = structure
        self.is_port_binding = False
        self.port_binding_module = None

    def portbinding(self, tree):
        self.is_port_binding = True
        self.port_binding_module = str(self.structure[self.current_module][tree.children[0]])
        self.__push_up(tree)
        self.is_port_binding = False
        self.port_binding_module = None
        return tree


    def hvarref(self, tree):
        if '##' in tree.children[0]:
            orig_token = tree.children[0]
            new_val = ''
            parts = orig_token.split('##')
            start = self.current_module
            if self.is_port_binding:  # if we are in port binding, we shall take the ids directly
                start = self.port_binding_module
            if start in self.structure:
                m = self.structure[start]
                end_point = False
            else:
                m = {}
                end_point = True
            for part in parts[:-1]:
                new_val += part
                if not end_point:
                    if part in m:
                        delim = '.'
                        m = self.structure[str(m[part])]
                    else:
                        end_point = True
                        delim = '_'
                else:
                    delim = '_'
                new_val += delim
            new_val += parts[-1]

            # new_val = tree.children[0].value.replace('##', '_')
            new_token = Token('ID', new_val)
            tree.children[0] = new_token
        return tree

    def idlit(self, tree):
        str_literal = tree.children[0]
        return str_literal

    def numlit(self, tree):
        num_literal = int(tree.children[0])
        return num_literal

    def numlitwidth(self, tree):
        self.__push_up(tree)
        tree.children[0] = int(tree.children[0])  # only supports decimal literals and primitive types for now
        return tree

    def htypeint(self, tree):
        return int(tree.children[0])

    def htype(self, tree):
        self.__push_up(tree)
        if len(tree.children) == 1 and isinstance(tree.children[0], int):
            return tree.children[0]
        else:
            return tree

    def hsensvar(self, tree):
        self.__push_up(tree)
        if len(tree.children) == 1 and is_tree_type(tree.children[0], 'hasync'):
            tree.children = tree.children[0].children
        elif len(tree.children) != 2:
            raise UnexpectedHCodeStructureError('hSensvar node should have 2 children')
        return tree

    def hasync(self, tree):
        self.__push_up(tree)
        assert len(tree.children) == 2
        if tree.children[1].children[0] == 0:  # negedge
            tree.children[1] = Token('NEG', 'neg')
        elif tree.children[1].children[0] == 1:
            tree.children[1] = Token('POS', 'pos')
        return tree

    def npa(self, tree):
        return tree.children[0]

    def hmodule(self, tree):
        self.current_module = tree.children[0]
        self.__push_up(tree)
        self.current_module = None
        return tree