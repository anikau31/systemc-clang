from lark import Tree, Token
from parselib.utils import dprint
from parselib.transforms import TopDown
from ..utils import ContextManager, is_tree_type
from dataclasses import dataclass
import typing

@dataclass(frozen=True)
class PortDecl:
    name: str
    type: ...
    direction: str

@dataclass(frozen=True)
class Interface:
    module_name: str
    interface_name: str  # we might need this for user to provide extra information
    interfaces: typing.List[PortDecl]

    @staticmethod
    def generate_instance_name(instance_name, is_array) -> str:
        if is_array:
            return 'itf'
        else:
            return f'{instance_name}_itf'

    def generate_interface_decl_name(self) -> str:
        return f'itf'

class InterfaceGeneration(TopDown):
    """
    This module generate interface definitions, and put them in the start of the nodes
    """
    def __init__(self):
        self.ctx = ContextManager()
        self.interface_meta_data = {}
        super().__init__()

    def modulelist(self, tree):
        with self.ctx.add_values(interfaces=[]):
            self.__push_up(tree)
            interfaces_tree_node = Tree(
                'interfaces', []
            )
            for interface in self.ctx.interfaces:
                interface_tree_node = Tree(
                    'interface', [
                        interface.interface_name,
                    ] + interface.interfaces
                )
                if interface.interfaces:
                    interfaces_tree_node.children.append(interface_tree_node)
            if interfaces_tree_node.children:
                tree.children.insert(0, interfaces_tree_node)

            for interface in self.ctx.interfaces:
                if interface.interfaces:
                    self.interface_meta_data[interface.module_name] = interface
            return tree

    def hmodule(self, tree):
        with self.ctx.add_values(current_ports=[]):
            self.__push_up(tree)
            self.ctx.interfaces.append(
                Interface(
                    module_name=tree.children[0].value,
                    interface_name=tree.children[0].value + '_interface',
                    interfaces=self.ctx.current_ports
                )
            )
            return tree

    def htypeinfo(self, tree):
        self.ctx.htype = tree.children[0]
        return tree

    def inportdecl(self, tree):
        self.ctx.name = tree.children[0].value
        self.ctx.direction = 'input'
        return tree

    def outportdecl(self, tree):
        self.ctx.name = tree.children[0].value
        self.ctx.direction = 'output'
        return tree

    def portdecltype(self, tree):
        with self.ctx.add_values(htype=None, name=None, direction=None):
            self.__push_up(tree)
            self.ctx.current_ports.append(
                PortDecl(
                    name=self.ctx.name,
                    type=self.ctx.htype,
                    direction=self.ctx.direction,
                )
            )
            return tree


    def _portdecltype_to_sigdecltype(self, tree):
        assert tree.data == 'portdecltype', "Expecting portdecltype"
        new_tree = Tree('sigdecltype', tree.children, meta=tree.meta)
        new_tree.children[0] = Tree('sigdecl', [tree.children[0].children[0]])
        new_tree.meta.direction = tree.direction
        return new_tree

    def _modportsiglist(self, tree):
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

    def _portdecltype(self, tree):
        with self.ctx.add_values(current_port_decl_node=tree):
            self.__push_up(tree)
            return tree

    def _htype(self, tree):
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


class InterfaceReplacement(TopDown):
    """Now we replace any interface port reference with the actual interface"""

    def __init__(self, port_meta):
        self.port_meta = port_meta
        self.ctx = ContextManager()
        super().__init__()

    def hmodule(self, tree):
        with self.ctx.add_values(current_module=tree.children[0].value):
            self.__push_up(tree)
            return tree
    
    # def hportbinding(self, tree):
    #    # we need to handle LHS case and RHS case
    #    # LHS should be the signal of the module,
    #    # RHS could be from
    #    # 1. the parent module
    #    # 2. internal signals
    #    pass
    def genbindinglist(self, tree):
        with self.ctx.add_values(is_in_genbindinglist=True):
            self.__push_up(tree)
            return tree

    def portbindinglist(self, tree):
        # this node is not automatically enumerated
        port_bindings: list = tree.children[1]
        for binding in port_bindings:
            assert len(binding.children) == 3, "Internal error, length of port binding should be 3"
            # only goes into right hand side
            assert is_tree_type(binding.children[2], 'hvarref'), "Internal error, only hvarref is supported"
            binding.children[2] = self.hvarref(binding.children[2])
        return tree
    
    # def portbinding(self, tree):
    #     if self.ctx.is_in_genbindinglist:
    #         if hasattr(tree, 'swap_for_for_loop'):
    #             if is_tree_type(tree.children[1], 'hvarref'):
    #                 tree.children[1] = self.hvarref(tree.children[1])
    #             else:
    #                 self.__push_up(tree.children[1])
    #         else:
    #             if is_tree_type(tree.children[1], 'hvarref'):
    #                 tree.children[1] = self.hvarref(tree.children[1])
    #             else:
    #                 self.__push_up(tree.children[1])

    #             tree.children[2] = self.hvarref(tree.children[2])
    #         return tree
    #     else:
    #         self.__push_up(tree)
    #         return tree

    def hvarref(self, tree):
        self.__push_up(tree)
        assert len(tree.children) == 1, "Internal error, hvarref should only have one child"
        cur_mod = self.ctx.current_module
        if cur_mod not in self.port_meta:
            return tree

        # TODO: this checks whether a varref is a port of current module
        # we could refactor this to be a function call such as _is_port_of_current_module()
        interface = self.port_meta[cur_mod]
        port_decls = interface.interfaces
        for port_decl in port_decls:
            # PortDecl
            if port_decl.name == tree.children[0]:
                # if this vardef is a local port
                new_token = Token('INTF_ID', value=f'{interface.generate_interface_decl_name()}.{port_decl.name}', )
                tree.children[0] = new_token
                break
        return tree

    def hprocess(self, tree):
        self.__push_up(tree)

        if hasattr(tree, 'force_sensevar'):
            self.__push_up(tree.force_sensevar)

        return tree
