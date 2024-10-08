import warnings

import lark
from lark import Token

from parselib.transforms import TopDown
from ..utils import dprint, is_tree_type, get_ids_in_tree
from ..grammar import UnexpectedHCodeStructureError


class LiteralExpansion(TopDown):
    """Expands integer literal into int"""
    def __init__(self, structure):
        super().__init__()
        self.structure = structure
        self.is_port_binding = False
        self.port_binding_module = None
        self.field_access = None

    def _get_port_binding_moduel(self, tree):
        """
        Get the name of the module that corresponds
        to this port-binding
        """
        if type(tree) == lark.Tree:
            if tree.children[0] != 'NONAME':
                res = str(self.structure[self.current_module][tree.children[0]])
            else:
                ids = get_ids_in_tree(tree)
                res = str(self.structure[self.current_module][ids[0]])
        else:
            res = str(self.structure[self.current_module][tree.children[0]])
        return res

    def portbinding(self, tree):
        # For now we just move port binding as is
        # self.is_port_binding = True
        # port_binding_module = self._get_port_binding_moduel(tree)
        # self.__push_up(tree)
        # self.is_port_binding = False
        # self.port_binding_module = None
        return tree

    def hfieldaccess(self, tree):
        """
              hFieldaccess  NONAME [
                hBinop ARRAYSUBSCRIPT [
                  hBinop ARRAYSUBSCRIPT [
                    hVarref pa_scclang_global_15 NOLIST
                    hVarref sig_scclang_global_1 NOLIST
                  ]
                  hVarref sig_scclang_global_1 NOLIST
                ]
                hField x NOLIST
              ]
        """
        # Field access is dedicated to struct, so no need to worry about the module
        field_name = tree.children[1].children[0]
        if self.field_access is not None:
            self.field_access.append(field_name)
        else:
            self.field_access = [field_name]
        self.__push_up(tree)
        return tree.children[0]


    def hvarref(self, tree):
        if '##' in tree.children[0]:
            orig_token = tree.children[0]
            new_val = ''
            parts = orig_token.split('##')
            start = self.current_module
            # if self.is_port_binding:  # if we are in port binding, we shall take the ids directly
            #     start = self.port_binding_module
            if start in self.structure:
                m = self.structure[start]
                end_point = False
            else:
                m = {}
                end_point = True
            for part in parts[:-1]:
                new_val += part
                if not end_point:
                    if part in m:
                        delim = '.'
                        m = self.structure[str(m[part])]
                    else:
                        end_point = True
                        delim = '_'
                else:
                    delim = '_'
                new_val += delim
            new_val += parts[-1]

            # new_val = tree.children[0].value.replace('##', '_')
            new_token = Token('ID', new_val)
            tree.children[0] = new_token
        if self.field_access is not None:
            tree.children[0] += '_' + '_'.join(self.field_access)
            self.field_access = None
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
        if len(tree.children) == 1 and is_tree_type(tree.children[0], 'hasync'):
            tree.children = tree.children[0].children
        elif len(tree.children) != 2:
            raise UnexpectedHCodeStructureError('hSensvar node should have 2 children')
        return tree

    def hasync(self, tree):
        self.__push_up(tree)
        assert len(tree.children) == 2
        if tree.children[1].children[0] == 0:  # negedge
            tree.children[1] = Token('NEG', 'neg')
        elif tree.children[1].children[0] == 1:
            tree.children[1] = Token('POS', 'pos')
        return tree

    def npa(self, tree):
        return tree.children[0]

    def hmodule(self, tree):
        self.current_module = tree.children[0]
        self.__push_up(tree)
        self.current_module = None
        return tree


class LiteralExpansion2(TopDown):
    def __init__(self):
        super().__init__() 
    
    def hvarref(self, tree):
        if isinstance(tree.children[0], Token) and '##' in tree.children[0].value:
            tree.children[0] = Token('ID', tree.children[0].value.replace('##', '_'))
        return tree

    def handle_list(self, lst):
        for idx in range(len(lst)):
            if isinstance(lst[idx], lark.Tree):
                self.__push_up(lst[idx])

    def portbindinglist(self, tree):
        for c in tree.children:
            if isinstance(c, list):
                self.handle_list(c)
        return tree