from lark import Tree
from parselib.transforms import TopDown
from parselib.transforms.node import TypeDefType
from ..utils import dprint, is_tree_type, get_ids_in_tree, alternate_ids, set_ids_in_tree_dfs


class TypeDefFilter(TopDown):
    """This pass recognizes the custom typedef type and remove it from the grammar tree"""
    def __init__(self):
        super().__init__()
        self.types = dict()
        self.current_params = None

    @staticmethod
    def _get_type_name(tree):
        return tree.children[0]

    @staticmethod
    def _get_type_params(tree):
        return list(map(lambda x: x.children[0], tree.find_data('htypetemplateparam')))

    @staticmethod
    def _get_type_aliases(tree):
        return dict(map(lambda x: (x.children[0], x.children[1]), tree.find_data('htypealias')))

    def htypedef(self, tree):
        type_name = TypeDefFilter._get_type_name(tree)
        type_params = TypeDefFilter._get_type_params(tree)
        type_aliases = TypeDefFilter._get_type_aliases(tree)

        self.current_params = type_params
        self.__push_up(tree)
        self.current_params = None

        type_fields = list(tree.find_data('htypefield'))
        t = TypeDefType(type_name, type_params, type_aliases, type_fields)
        self.types[type_name] = t
        return tree

    def hdeptype(self, tree):
        return tree

    def htypealias(self, tree):
        dprint(tree)
        return tree

    def htype(self, tree):
        # extract type parameters
        if len(tree.children) == 1 and self.current_params and tree.children[0] in self.current_params:
            return tree.children[0]
        else:
            self.__push_up(tree)
            return tree

    def start(self, tree):
        tree.children = self.visit_children(tree)
        # remove typedefs from the syntax tree
        tree.children = list(filter(lambda x: x.data != 'typelist', tree.children))
        return tree


class TypeDefCleanup(TopDown):
    """This pass recognizes the custom typedef type and remove it from the grammar tree"""
    def __init__(self):
        super().__init__()

    def htype(self, tree):
        self.__push_up(tree)
        if is_tree_type(tree, 'htype') and isinstance(tree.children[0], int):
            return tree.children[0]
        return tree
