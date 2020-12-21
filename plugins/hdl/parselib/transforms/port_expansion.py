from .top_down import TopDown
from lark import Token, Tree
from ..utils import dprint
import copy
import warnings


class PortExpansion(TopDown):
    def __init__(self):
        """
        Expands mixed port objects such as sc_rvd into in-port and out-port
        This pass should come **before** typedef expansion
        """
        super().__init__()
        self.expanded = list()

    # The port expansion for sc_rvd_in and sc_rvd_out
    def create_inport(self, t, meta):
        return Tree('htype', [Token('TYPESTR', 'sc_in'), t], meta)

    def create_outport(self, t, meta):
        return Tree('htype', [Token('TYPESTR', 'sc_out'), t], meta)

    def create_channel(self, t, meta):
        return Tree('htype', [Token('TYPESTR', 'sc_signal'), t], meta)

    def create_inport_decl(self, name, meta):
        return Tree('inportdecl', [Token('ID', name)], meta)

    def create_outport_decl(self, name, meta):
        return Tree('outportdecl', [Token('ID', name)], meta)

    def create_channel_decl(self, name, meta):
        return Token('ID', name, meta)

    def __expand_sc_rvd(self, tree, inout=None):
        port_name, port_type = tree.children
        port_type_meta = port_type.meta
        if inout == 'channel':
            port_name = port_name
        elif inout in ['in', 'out']:
            port_name = port_name.children[0].value

        data_port = copy.deepcopy(port_type)
        valid_port = copy.deepcopy(port_type)
        ready_port = copy.deepcopy(port_type)

        if inout == 'in':
            data_valid_port_func = self.create_inport
            data_valid_port_decl_func = self.create_inport_decl
            ready_port_func = self.create_outport
            ready_port_decl_func = self.create_outport_decl
        elif inout == 'out':
            data_valid_port_func = self.create_outport
            data_valid_port_decl_func = self.create_outport_decl
            ready_port_func = self.create_inport
            ready_port_decl_func = self.create_inport_decl
        elif inout == 'channel':
            data_valid_port_func = self.create_channel
            data_valid_port_decl_func = self.create_channel_decl
            ready_port_func = self.create_channel
            ready_port_decl_func = self.create_channel_decl

        meta = data_port.children[0].meta

        data_type = data_port.children[0].children[1]
        data_port.children[0] = data_valid_port_func(data_type, meta)

        signal_type = Tree('htype', [Token('TYPESTR', 'sc_uint'), 1])
        valid_port.children[0] = data_valid_port_func(copy.deepcopy(signal_type), meta)
        ready_port.children[0] = ready_port_func(copy.deepcopy(signal_type), meta)

        meta = tree.meta

        data_port_decl = copy.copy(tree)
        valid_port_decl = copy.copy(tree)
        ready_port_decl = copy.copy(tree)

        data_port_decl.children = [data_valid_port_decl_func(port_name + '_data', meta), data_port]
        valid_port_decl.children = [data_valid_port_decl_func(port_name + '_valid', meta), valid_port]
        ready_port_decl.children = [ready_port_decl_func(port_name + '_ready', meta), ready_port]
        return [data_port_decl, valid_port_decl, ready_port_decl]

    def __expand_sc_rvd_binding_pair(self, tree):
        _, sub, par = tree.children
        par_name, par_type = self.__get_expanded(par.children[0])
        if par_type:
            assert par_type in ['sc_rvd', 'sc_rvd_in', 'sc_rvd_out']
            new_trees = [copy.deepcopy(tree) for _ in range(3)]
            sub_fields = ['data', 'valid', 'ready']
            res = []
            for t, f in zip(new_trees, sub_fields):
                _, sub, par = t.children
                sub_v = sub.children[0]
                par_v = par.children[0]
                t.children[0 + 1].children[0].value = sub_v + '_' + f
                t.children[1 + 1].children[0].value = par_v + '_' + f
                res.append(t)
            return res
        else:
            return [tree]

    def __expand_port(self, tree):
        port_name, port_type = tree.children
        port_to_expand = ['sc_rvd_in', 'sc_rvd_out']
        port_type_name = port_type.children[0].children[0]
        if port_type_name in port_to_expand:
            self.__set_expanded(port_name.children[0], port_type_name)
            if port_type_name == 'sc_rvd_in':
                return self.__expand_sc_rvd(tree, 'in')
            elif port_type_name == 'sc_rvd_out':
                return self.__expand_sc_rvd(tree, 'out')
            else:
                assert False
        else:
            return [tree]

    def __expand_vardecl(self, tree):
        if len(tree.children) == 3:
            warnings.warn('Expansion of sc_rvd with initial value is not supported')
            return [tree]
        bundle_name, bundle_type = tree.children
        bundle_to_expand = ['sc_rvd']
        bundle_type_name = bundle_type.children[0].children[0]
        if bundle_type_name in bundle_to_expand:
            self.__set_expanded(bundle_name, bundle_type_name)
            return self.__expand_sc_rvd(tree, 'channel')
        else:
            return [tree]

    def __set_expanded(self, var_name, bundle_type_name):
        if var_name in self.expanded[-1]:
            assert False, "Duplicate bundle decl"
        self.expanded[-1][var_name] = bundle_type_name

    def __get_expanded(self, var_name):
        if var_name in self.expanded[-1]:
            return var_name, self.expanded[-1][var_name]
        else:
            return None, None


    def modportsiglist(self, tree):
        self.__push_up(tree)
        new_children = []
        for ch in tree.children:
            if ch.data == 'portdecltype':
                res = self.__expand_port(ch)
                new_children.extend(res)
            elif ch.data == 'vardeclinit':
                res = self.__expand_vardecl(ch)
                new_children.extend(res)
            else:
                new_children.append(ch)
        tree.children = new_children
        return tree


    def portbindinglist(self, tree):
        module_name, *bindings = tree.children
        new_children = []
        for binding in bindings:
            res = self.__expand_sc_rvd_binding_pair(binding)
            new_children.extend(res)
        tree.children = module_name, *new_children
        return tree

    def hmodule(self, tree):
        self.expanded.append(dict())
        self.__push_up(tree)
        self.expanded.pop()
        return tree
