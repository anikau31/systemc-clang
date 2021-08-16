from lark import Tree, Token
from parselib.utils import dprint
from parselib.transforms import TopDown


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
    """
    def __init__(self):
        super().__init__()

    def hbinop(self, tree):
        dprint(tree)
        pass