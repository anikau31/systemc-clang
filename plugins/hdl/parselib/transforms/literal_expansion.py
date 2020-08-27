import warnings

from lark import Token

from parselib.transforms import TopDown


class LiteralExpansion(TopDown):
    """Expands integer literal into int"""
    def __init__(self):
        super().__init__()

    def hvarref(self, tree):
        if '##' in tree.children[0]:
            orig_token = tree.children[0]
            new_val = tree.children[0].value.replace('##', '_')
            new_token = Token('ID', new_val)
            tree.children[0] = new_token
        return tree

    def idlit(self, tree):
        str_literal = tree.children[0]
        return str_literal

    def numlit(self, tree):
        num_literal = int(tree.children[0])
        return num_literal

    def numlitwidth(self, tree):
        self.__push_up(tree)
        tree.children[0] = int(tree.children[0])  # only supports decimal literals and primitive types for now
        return tree

    def htypeint(self, tree):
        return int(tree.children[0])

    def htype(self, tree):
        self.__push_up(tree)
        if len(tree.children) == 1 and isinstance(tree.children[0], int):
            return tree.children[0]
        else:
            return tree

    def hsensvar(self, tree):
        self.__push_up(tree)
        if len(tree.children) == 1:
            return tree.children[0]
        else:
            if len(tree.children) > 2:
                warnings.warn('more than two children are present in sensitivity list, taking only the first one')
                tree.children = tree.children[:2]
            var, edge = tree.children
            if edge:
                if edge == 'pos':
                    return 'posedge ' + var
                elif edge == 'neg':
                    return 'negedge ' + var
                else:
                    return var + '_' + edge
            return var

    def npa(self, tree):
        return tree.children[0]