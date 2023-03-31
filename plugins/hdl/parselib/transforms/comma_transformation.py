from lark import Tree, Token
from parselib.utils import dprint
from parselib.transforms import TopDown
from ..utils import dprint, is_tree_types


class CommaTransformation(TopDown):
    """
    The comma transformation is aimed to handle the case such as
        k = (i++, j++)
    in the code.
    This type of operator is not supported in Verilog and thus we need to break
    the code into sequence of code such as:
        i++
        k = j++
    Note: k = j++ will be handled by another pass that gets j and j++ correctly
    To handle this, we break a hBinop , [ lhs rhs ] node into two.
    lhs will be appended to the parent and rhs remains
    """
    def __init__(self):
        super().__init__()
        self.broken_down_ops = []  # stores the operations broken down by ,
        self.has_comma = False
        self.lifted = set()
        self.nesting_assign = False

    def hcomma(self, tree):
        self.__push_up(tree)
        self.has_comma = True
        lhs, rhs = tree.children
        self.broken_down_ops.append(lhs)
        return rhs

    def blkassign(self, tree):
        if self.nesting_assign:
            self.__push_up(tree)
            lhs, rhs = tree.children
            self.broken_down_ops.append(tree)
            return lhs
        else:
            self.nesting_assign = True
            self.__push_up(tree)
            return tree

    def hbinop(self, tree):
        op = tree.children[0]
        if op in ['+=', '-=', '*=', '/=']:
            self.nesting_assign = True
        self.__push_up(tree)
        op, lhs, rhs = tree.children
        if op in ['+=', '-=', '*=', '/=']:
            self.broken_down_ops.append(tree)
            return lhs
        else:
            return tree

    def hnsbinop(self, tree):
        op = tree.children[0]
        if op in ['+=', '-=', '*=', '/=']:
            self.nesting_assign = True
        self.__push_up(tree)
        op, lhs, rhs = tree.children
        if op in ['+=', '-=', '*=', '/=']:
            self.broken_down_ops.append(tree)
            return lhs
        else:
            return tree

    def stmts(self, tree):
        # TODO: this only covers a small case, and does not support for cases like
        # for(...) a += b++;
        new_children = []
        for ch in tree.children:
            self.broken_down_ops = []
            self.nesting_assign = False
            if isinstance(ch, Tree) and is_tree_types(ch, ["hbinop", "hnsbinop", "blkassign"]):
                ch_new = self.visit(ch)
                new_children.extend(map(lambda x: Tree('stmt', [x]), self.broken_down_ops))
                new_children.append(ch_new)
            else:
                self.__push_up(ch)
                new_children.append(ch)
        tree.children = new_children
        return tree