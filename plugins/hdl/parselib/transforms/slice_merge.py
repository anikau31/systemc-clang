from lark import Tree, Token

from parselib.transforms import TopDown
from parselib.utils import dprint


class SliceMerge(TopDown):
    """recognize slice patterns and convert slicing into synthesizable index part select"""
    def __init__(self):
        super().__init__()

    def __to_seq(self, tree):
        return list(map(lambda x: x.children[0], tree.iter_subtrees_topdown()))

    def __is_literal_node(self, t):
        return isinstance(t, Tree) and t.data == 'hliteral'

    def __check_part_select_left(self, tr):
        """check if the left hand side is width * (ID + 1) - 1
        returns result, width, ID
        """
        failed = False, None, None
        if not isinstance(tr, Tree) or len(tr.children) != 3:
            return failed
        is_offset_1 = tr.children[0] == '-' and isinstance(tr.children[2], Tree) and tr.children[2].children[0] == 1
        if not is_offset_1:
            dprint(tr)
            return failed
        mult_part = tr.children[1]
        if not isinstance(mult_part, Tree) or mult_part.data != 'hbinop' or mult_part.children[0] != '*':
            return failed
        op0, op1 = mult_part.children[1:]
        if self.__is_literal_node(op0):
            width = op0.children[0]
            idx = op1
        elif self.__is_literal_node(op1):
            width = op1.children[0]
            idx = op0
        else:
            return failed
        if not isinstance(idx, Tree) or idx.data != 'hbinop' or idx.children[0] != '+':
            return failed
        op0, op1 = idx.children[1:]
        if self.__is_literal_node(op0) and op0.children[0] == 1 and len(op1.children) == 1 and op1.data == 'hvarref':
            id = op1.children[0]
            return True, width, id
        elif self.__is_literal_node(op1) and op1.children[0] == 1 and len(op0.children) == 1 and op0.data == 'hvarref':
            id = op0.children[0]
            return True, width, id
        else:
            return failed


    def __check_part_select_right(self, tr):
        """check if the right hand-side is width * ID
        returns result, width, ID
        """
        lst = self.__to_seq(tr)
        is_mult = lst[0] == '*'
        if not is_mult:
            return False, None, None
        p1, p2 = lst[1:]

        if isinstance(p1, int) and isinstance(p2, Token):
            return True, p1, p2
        elif isinstance(p2, int) and isinstance(p1, Token):
            return True, p2, p1
        else:
            return False, None, None

    def harrayref(self, tree):
        if tree.children[0].data == 'hslice':
            self.__push_up(tree)
            if isinstance(tree.children[0], tuple):
                tree.children = tree.children[0]
            return tree
        else:
            return tree

    def hslice(self, tree):
        self.__push_up(tree)
        var_name, l, r = tree.children
        lOk, lWidth, lID = self.__check_part_select_left(l)
        rOk, rWidth, rID = self.__check_part_select_right(r)
        if lOk and rOk and lWidth == rWidth and lID == rID:
            mult = Tree(data='hbinop', children=['*', lID, lWidth])
            index_part = Tree(data='hbinop', children=['+:', mult, lWidth])
            return var_name, index_part
        else:
            return tree