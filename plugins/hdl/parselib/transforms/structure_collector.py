import warnings

from lark import Token

from parselib.transforms import TopDown
from ..utils import dprint, is_tree_type
from ..grammar import UnexpectedHCodeStructureError
from ..utils import dprint


class StructureCollector(TopDown):
    """Collect structural information, such as:
    module_name: list of port name

    We could leverage this to facilitate generation of assign of ports
    """
    def __init__(self):
        super().__init__()
        self.hier = {}
        self.current = self.hier

    def moddecl(self, tree):
        # we can safely assume that moddecl only contains module types
        self.__push_up(tree)
        mod_name = tree.children[0]
        mod_type = tree.children[1].children[0].children[0]
        self.current[mod_name] = mod_type
        return tree

    def start(self, tree):
        self.__push_up(tree)
        return tree

    def hmodule(self, tree):
        d = {}
        mod_name = tree.children[0]
        orig = self.current
        orig[mod_name] = d
        self.current = d

        self.__push_up(tree)

        # restore
        self.current = orig
        return tree
