from parselib.transforms import TopDown
from parselib.transforms.node import TypeDefType


class TypeDefFilter(TopDown):
    """This pass recognizes the custom typedef type and remove it from the grammar tree"""
    def __init__(self):
        super().__init__()
        self.types = dict()
        self.current_params = None

    def htypedef(self, tree):
        type_name = tree.children[0]
        type_params = list(map(lambda x: x.children[0], tree.find_data('htypetemplateparam')))

        self.current_params = type_params
        self.__push_up(tree)
        self.current_params = None

        type_fields = list(tree.find_data('htypefield'))
        t = TypeDefType(type_name, type_params, type_fields)
        self.types[type_name] = t
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