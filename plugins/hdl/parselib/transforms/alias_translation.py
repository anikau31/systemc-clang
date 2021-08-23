from lark import Tree, Token
from parselib.utils import dprint
from parselib.transforms import TopDown


class AliasTranslation(TopDown):
    """
    This pass merges separate nodes that are created for easy recognition for grammar,
    but actually shares the same semantics.
    TODO: this pass does almost the same thing as the pass NodeMerging Pass, and
    TODO: the 2 classes should be merged together
    """
    COMPOUND_ASSIGN = ["*=", "+=", "-=", "/=", "%=", "|=", "&=", "^=", "<<=", ">>="]

    def _get_op_token_from_compound_assign(self, op):
        assert op in AliasTranslation.COMPOUND_ASSIGN
        new_op = op[:-1]
        if new_op in ["+", "-", "*", "/", "^"]:
            return Token('ARITHOP', new_op)
        else:
            return Token('NONSUBBINOP', new_op)

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

    def hbinop(self, tree):
        """
        Convert a compound assignment into assignment of binary operator
        Nested compound assignment is not supported
        """
        op = tree.children[0]
        if op in AliasTranslation.COMPOUND_ASSIGN:
            lhs, rhs = tree.children[1:]
            # Convert this node into a block assignment node with binary op
            new_op = self._get_op_token_from_compound_assign(op)
            # Create a new treenode
            meta = tree.meta
            binop_node = Tree('hbinop', [new_op] + tree.children, meta)
            assign = Tree('blkassign', [lhs, binop_node], meta)
            return assign
        return tree

    def blkassign(self, tree):
        # we should detect blocking assignment in a different way
        tree.must_block = True
        if tree.children[0].data == 'vassign':
            tree.children = tree.children[0].children
            tree.must_block = True
        elif tree.children[0].data == 'nblkassign':
            tree.children = tree.children[0].children
            tree.must_block = False
        elif tree.children[0].data == 'hmodassign':
            self.__push_up(tree)
            op, l, r = tree.children[0].children
            tree.children = [l, Tree('hbinop', [Token('BINOP', op), l, r], meta=tree.meta)]
            tree.must_block = False
        return tree

    def htype(self, tree):
        is_array = isinstance(tree.children[0], Tree) and tree.children[0].data == 'htypearray'
        self.__push_up(tree)
        if is_array:
            tree.children = tree.children[0]
        return tree

    def arraydimlength(self, tree):
        self.__push_up(tree)
        dim = int(tree.children[0])
        if len(tree.children) == 1:  # the last dim
            return [dim]
        else:
            return [dim] + tree.children[1]

    def htypearray(self, tree):
        self.__push_up(tree)
        sz, tpe = tree.children
        res = ['array', tpe, sz]
        return res

    def stmt(self, tree):
        """filters out noop"""
        self.__push_up(tree)
        tree.children = list(filter(lambda x: x.data != 'hnoop', tree.children))
        return tree

    def hunop(self, tree):
        # self.__push_up(tree)
        if isinstance(tree.children[0], Tree) and tree.children[0].data == 'hunopdec':
            tree.children = [Token('UNOP_NON_DEC', '--'), tree.children[0].children[0]]
        return tree