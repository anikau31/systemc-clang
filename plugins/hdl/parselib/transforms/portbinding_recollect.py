
from lark import Tree
from parselib.transforms import TopDown
from parselib.transforms.node import TypeDefType
from ..utils import dprint, is_tree_type, get_ids_in_tree_types, ContextManager, get_tree_types
from pprint import pprint
from copy import deepcopy
import logging


class PortbindingPrecheck(TopDown):
    def portbindinglist(self, tree):
        assert False, "portbindinglist should not be present in the tree"


class PortbindingRecollect(TopDown):
    """
    This pass collects portbinding nodes and transform them into a portbindinglist node.
    We have this pass because portbindings are scattered in hModinitblock
    These blocks cannot be directly translated to verilog uniformly
    """
    def __init__(self, ports):
        # pre checks
        super().__init__()
        self.ctx = ContextManager()
        self.ports = ports
        

    def hmodule(self, tree):
        with self.ctx.add_values(current_module=tree.children[0].value, module_type={}):
            self.__push_up(tree)
            return tree

    def forinit(self, tree):
        with self.ctx.add_values(is_in_for_init=True):
            self.__push_up(tree)
            return tree

    def hvarref(self, tree):
        if self.ctx.is_in_for_init and self.ctx.is_in_initblock:
            self.ctx.for_var_catcher.add(tree.children[0])
        return tree

    def forstmt(self, tree):
        if not self.ctx.is_in_initblock:
            return tree

        # assert: self.is_in_initblock
        with self.ctx.add_values(
            is_in_for=True, 
            for_has_portbinding=False,
            has_non_portbinding_stmt=False,
            ):
            self.__push_up(tree)

            # allow some level of nesting
            assert not (self.ctx.for_has_portbinding and self.ctx.has_non_portbinding_stmt), \
                "portbinding and non-portbinding stmts should not be mixed in a for block"

            # if this is the outermost for block, collect it for portbinding
            if self.ctx.is_in_initblock and  self.ctx.for_has_portbinding and self.ctx.search_key_in_outer_context("is_in_for") is None:
                self.ctx.for_port_bindings.append(tree)
                return None
            else:
                return tree

    def stmt(self, tree):
        # self.__push_up(tree)
        # if self.is_in_initblock and self.is_in_for:
        #     if tree.children[0].data != 'portbinding':
        #         self.has_non_portbinding_stmt = True 
        if self.ctx.is_in_initblock and self.ctx.is_in_for:
            if tree.children[0].data not in ['portbinding', 'hcstmt', 'forstmt']:
                self.ctx.has_non_portbinding_stmt = True
        
        self.__push_up(tree)
        if len(tree.children) == 1 and tree.children[0] is None:
            return None
        else:
            return tree

    def stmts(self, tree):
        self.__push_up(tree)
        if self.ctx.is_in_initblock:
            tree.children = list(filter(lambda x: x is not None, tree.children))
        return tree

    def hmodinitblock(self, tree):
        with self.ctx.add_values(
            is_in_initblock=True,
            simple_port_bindings=[],
            for_port_bindings=[],
            for_var_catcher=set()
        ):
            self.__push_up(tree)

            portbindinglist_node = Tree('portbindinglist', [
                None,
                [*self.ctx.simple_port_bindings],
            ])
            forbindinglist_node = Tree('genbindinglist', [
                Tree('genvardecl', list(self.ctx.for_var_catcher)),
                Tree('genfor', self.ctx.for_port_bindings)
            ])
            # dprint(portbindinglist_node.pretty())

            tree.children.append(portbindinglist_node)
            if len(self.ctx.for_port_bindings) > 0:
                tree.children.append(forbindinglist_node)
            return tree

    def moduleinst(self, tree):
        self.__push_up(tree)
        module_inst_name = str(tree.children[0])
        module_type_name = tree.children[1].children[0].children[0].value
        self.ctx.module_type[module_inst_name] = module_type_name
        return tree

    def modulearrayinst(self, tree):
        self.__push_up(tree)
        module_inst_name = str(tree.children[0])
        module_type_name = tree.children[1].children[0].children[1].children[0].value
        self.ctx.module_type[module_inst_name] = module_type_name
        return tree



    def get_module_name_in_portbinding(self, tree):
        normal_name = tree.children[0].value
        # TODO: change this to something that cannot be confused with a module name
        if 'NONAME' in normal_name:
            module_name = get_ids_in_tree_types(tree.children[1])[0].value
            return module_name
        return normal_name

    def get_port_name_in_portbinding(self, tree):
        normal_name = tree.children[0].value
        if 'NONAME' in normal_name:
            # This corrsponds to the case where the port name is behind a dot
            # node = tree.children[1]
            node = get_tree_types(tree, ['hfieldaccess'])[0]
            if node.data == 'hfieldaccess':
                return node.children[1].children[0].value
            else:
                dprint(tree.pretty())
                assert False, "Unknown case"
        else:
            return get_ids_in_tree_types(tree.children[1])[0].value


    def portbinding(self, tree):
        # self.collected_portbinding.append(tree)
        # self.for_has_portbinding = True
        if self.ctx.is_in_for:
            self.ctx.for_has_portbinding = True
            module_orig = tree.children[0].value
            module = self.get_module_name_in_portbinding(tree)
            if module not in self.ctx.module_type:
                logging.warn(f"TODO: Module {module} not found in moduleinst, future fix is required")
                return tree
            module_type = self.ctx.module_type[module]
            
            # print(self.ports[module_type])
            # children[1] is always the port name ref
            # port_name = get_ids_in_tree_types(tree.children[1])[0].value
            port_name = self.get_port_name_in_portbinding(tree)
            port_dir = self.ports[module_type][port_name]
            # default order is children[0] <- children[1] (in Verilog)
            # if it is an output port, we need to reverse them.
            if port_dir == PortDirectionCollector.OUTPUT: 
                tree.children[1:3] = tree.children[2:0:-1]
                tree.swap_for_for_loop = True
                # This is an indication so that at a later pass
                # the dot syntax can be inserted properly
            return tree
        else:
            self.ctx.simple_port_bindings.append(tree)
            return None
        

class PortDirectionCollector(TopDown):
    INPUT = 0
    OUTPUT = 1

    def __init__(self):
        self.ports= { }
        self.ctx = ContextManager()

    def hmodule(self, tree):
        with self.ctx.add_values(current_module=tree.children[0].value):
            self.ports[self.ctx.current_module] = {}
            self.__push_up(tree)
            return tree
    
    def inportdecl(self, tree):
        self.ports[self.ctx.current_module][tree.children[0].value] = PortDirectionCollector.INPUT
        return tree

    def outportdecl(self, tree):
        self.ports[self.ctx.current_module][tree.children[0].value] = PortDirectionCollector.OUTPUT
        return tree

    def sigdecltype(self, tree):
        
        if hasattr(tree.meta, 'direction'):
            d = tree.meta.direction
            if d == 'input':
                self.ports[self.ctx.current_module][tree.children[0].children[0].value] = PortDirectionCollector.INPUT
            elif d == 'output':
                self.ports[self.ctx.current_module][tree.children[0].children[0].value] = PortDirectionCollector.OUTPUT
            else:
                raise ValueError(f"Unknown direction {d}")
        return tree


class LowerComplexPort(TopDown):
    """
    This pass simply lowers the the field access to a simple portbinding
    """
    def __init__(self):
        self.ctx = ContextManager()
        pass

    def genbindinglist(self, tree):
        with self.ctx.add_values(is_in_genbindinglist=True):
            self.__push_up(tree)
            return tree

    def hvarref(self, tree):
        if not self.ctx.is_in_genbindinglist:
            return tree
        return tree.children[0]
        
    def harrayref(self, tree):
        if not self.ctx.is_in_genbindinglist:
            return tree
        self.ctx.cur_depth += 1
        self.__push_up(tree)
        self.ctx.cur_depth -= 1
        if self.ctx.cur_depth == 0:
            return '{}[{}]'.format(tree.children[0], tree.children[1])
        else:
            return '{}[{}].dim'.format(tree.children[0], tree.children[1])

    def numlit(self, tree):
        if not self.ctx.is_in_genbindinglist:
            return tree
        return tree.children[0]

    def hfieldname(self, tree):
        if not self.ctx.is_in_genbindinglist:
            return tree
        return tree.children[0]
    def hfieldaccess(self, tree):
        if self.ctx.is_in_genbindinglist:
            with self.ctx.add_values(cur_depth=0):
                self.__push_up(tree)
                return tree.children[0] + ".mod." + tree.children[1]
        return tree
