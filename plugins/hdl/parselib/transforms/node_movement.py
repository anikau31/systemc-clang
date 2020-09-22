from lark import Tree, Token
from parselib.utils import dprint
from parselib.transforms import TopDown


class NodeMovement(TopDown):
    def __init__(self):
        super().__init__()
        # record function per module
        self.functions = dict()
        self.current_module = None

    def hmodule(self, tree):
        self.current_module = tree.children[0]
        self.__push_up(tree)
        functions = self.functions.get(self.current_module)
        if functions:
            for f in functions:
                tree.children.append(f)
        self.current_module = None
        return tree

    def __add_functions(self, functions):
        res = self.functions.setdefault(self.current_module, [])
        res.extend(functions)

    def hprocess(self, tree):
        functions = list(filter(lambda x: isinstance(x, Tree) and x.data == 'hfunction', tree.children))
        tree.children = list(filter(lambda x: not (isinstance(x, Tree) and x.data == 'hfunction'), tree.children))
        self.__add_functions(functions)
        return tree