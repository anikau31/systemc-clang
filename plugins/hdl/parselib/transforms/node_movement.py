from lark import Tree, Token
from parselib.utils import dprint
from parselib.transforms import TopDown
from ..utils import ContextManager


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


class ArrayPortMovement(TopDown):
    def __init__(self):
        self.ctx = ContextManager()
        super().__init__()

    def portdecltype_to_sigdecltype(self, tree):
        assert tree.data == 'portdecltype', "Expecting portdecltype"
        new_tree = Tree('sigdecltype', tree.children, meta=tree.meta)
        new_tree.children[0] = Tree('sigdecl', [tree.children[0].children[0]])
        new_tree.meta.direction = tree.direction
        return new_tree

    def modportsiglist(self, tree):
        with self.ctx.add_values(port_decl_to_remove=[]):
            self.__push_up(tree)
            if self.ctx.port_decl_to_remove:
                tree.children = list(filter(lambda x: x not in self.ctx.port_decl_to_remove, tree.children))
                # move it to varlist
                insertion_pos = -1
                # find the last sigdecltype, or last portdecltype if no sigdecltype exists
                for i in range(len(tree.children) - 1, -1, -1):
                    if tree.children[i].data == 'sigdecltype':
                        insertion_pos = i
                        break
                    elif tree.children[i].data == 'portdecltype':
                        insertion_pos = i
                        break
                new_sig_decls = list(map(self.portdecltype_to_sigdecltype, self.ctx.port_decl_to_remove))
                tree.children = tree.children[:insertion_pos + 1] + new_sig_decls + tree.children[insertion_pos + 1:]

                # for port in self.ctx.port_decl_to_remove:
                #     tree.children.append(
                #         self.portdecltype_to_sigdecltype(port)
                #     )

        return tree

    def portdecltype(self, tree):
        with self.ctx.add_values(current_port_decl_node=tree):
            self.__push_up(tree)
            return tree

    def htype(self, tree):
        # If this is a port array
        if self.ctx.current_port_decl_node and tree.children[0] == 'array':
            if self.ctx.current_port_decl_node.children[0].data == 'inportdecl':
                with self.ctx.add_values(port_erasure=True):
                    self.__push_up(tree)
                    self.ctx.current_port_decl_node.direction = 'input'
                    self.ctx.port_decl_to_remove.append(self.ctx.current_port_decl_node)
            elif self.ctx.current_port_decl_node.children[0].data == 'outportdecl':
                self.ctx.current_port_decl_node.direction = 'output'
            else:
                raise ValueError("Unknown port type")
        elif self.ctx.port_erasure:
            if tree.children[0] in ['sc_in', 'sc_out']:
                return tree.children[1]
            return tree
        return tree

