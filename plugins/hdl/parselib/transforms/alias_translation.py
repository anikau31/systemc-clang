from lark import Tree, Token

from parselib.transforms import TopDown


class AliasTranslation(TopDown):
    """Expands integer literal into int"""
    def __init__(self):
        super().__init__()

    def hsensedge(self, tree):
        self.__push_up(tree)
        tree.data = 'hsensvar'
        return tree

    def syscread(self, tree):
        """handles the syscread for array ref"""
        # dprint(tree)
        # if len(tree.children) == 1 and tree.children[0].data == 'hbinop' and tree.children[0].children[0] == 'ARRAYSUBSCRIPT':
        #     pass
        #     # assert False
        return tree

    def hsubassign(self, tree):
        return '-'

    def haddassign(self, tree):
        return '+'

    def blkassign(self, tree):
        tree.must_block = False
        if tree.children[0].data == 'vassign':
            tree.children = tree.children[0].children
            tree.must_block = True
        elif tree.children[0].data == 'hmodassign':
            self.__push_up(tree)
            op, l, r = tree.children[0].children
            tree.children = [l, Tree('hbinop', [Token('BINOP', op), l, r], meta=tree.meta)]
        return tree

    def htype(self, tree):
        is_array = isinstance(tree.children[0], Tree) and tree.children[0].data == 'htypearray'
        self.__push_up(tree)
        if is_array:
            tree.children = tree.children[0]
        return tree

    def htypearray(self, tree):
        sz, tpe = tree.children
        res = ['array', tpe, int(sz)]
        return res