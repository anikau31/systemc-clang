from lark import Tree, Token

from parselib.transforms import TopDown
from parselib.utils import dprint

class ReorderModInitBlock(TopDown):
    """
    reorders the mod init block, so that hmodinitblock (sensitivity list) is available
    when the processes are being translated
    """
    def __init__(self):
        super().__init__()

    def hmodule(self, tree):
        key = {
            'hmodinitblock': 0,
            'processlist': 1
        }  # only supported elements
        tree.children = sorted(tree.children, key=lambda x: key[x.data] if isinstance(x, Tree) and x.data in key else -1)
        return tree
