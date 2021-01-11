import warnings

from lark import Token

from parselib.transforms import TopDown
from ..utils import dprint, is_tree_type
from ..grammar import UnexpectedHCodeStructureError


class LiteralExpansion(TopDown):
    """Expands integer literal into int"""
    def __init__(self):
        super().__init__()

    def hvarref(self, tree):
        if '##' in tree.children[0]:
            orig_token = tree.children[0]
            new_val = tree.children[0].value.replace('##', '_')
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
        if len(tree.children) != 2:
            raise UnexpectedHCodeStructureError('hSensvar node should have 2 children')
        return tree

    def npa(self, tree):
        return tree.children[0]