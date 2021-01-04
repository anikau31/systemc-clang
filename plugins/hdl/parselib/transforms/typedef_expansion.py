import itertools
from .top_down import TopDown
from ..primitives import Primitive
from .node import TypeDefType
from ..compound import aggregate
from lark import Tree, Token
import copy
import warnings
from ..utils import dprint, is_tree_type

class TypedefExpansion(TopDown):
    """Expands block assignment of custom types into primitive types"""
    def __init__(self, types):
        super().__init__()
        self.types = types
        # expanded is the variable stack
        self.expanded = [dict()]
        self.current_module = ''

    def __expand_htype(self, htype):
        """expand non-primitive htype into primitive sub-field type"""
        # Note: expansion will stop at custom type boundary,
        # futher nested type expansion will be handled in the TypeDefType expansion
        assert htype.data == 'htype'
        # depth first search, use non-recursive version to better handle copy of the node
        stack = list()
        stack.append(htype)
        expandable = 0
        while stack:
            stack_top = stack.pop()
            assert stack_top.data == 'htype'
            type_name, *type_params = stack_top.children
            if type_name in self.types:
                expandable += 1
                fields = self.types[type_name].get_fields_with_instantiation(type_params, self.types)
            else:
                # expand other htype fields
                for child in type_params:
                    if isinstance(child, Tree):
                        stack.append(child)

        if expandable == 0:
            return [('', htype)]  # no expansion required
        elif expandable == 1:
            res = []
            for k, v in fields:
                new_type = copy.deepcopy(htype)
                stack.append(new_type)
                while stack:
                    stack_top = stack.pop()
                    type_name, *type_params = stack_top.children
                    if type_name in self.types:  # this is the only type that gets expanded
                        stack_top.children = v.children
                    else:
                        for child in type_params:
                            if isinstance(child, Tree):
                                stack.append(child)
                res.append((k, new_type))
            return res
        else:
            assert False, 'Cannot expand more than 1 custom type'

    def __expand_typeinfo(self, typeinfo):
        assert typeinfo.data == 'htypeinfo'
        assert len(typeinfo.children) == 1
        res = self.__expand_htype(typeinfo.children[0])
        # reconstruct typeinfo
        return [(x[0], Tree(typeinfo.data, [x[1]], typeinfo.meta)) for x in res]

    def __expand_helper(self, portdecl, typeinfo):
        """portdecl is the name:
        Tree(outportdecl, [Token(ID, 'dout')])
        typeinfo is the real type info:
        Tree(htypeinfo, [Tree(htype, [Token(TYPESTR, 'sc_out'), Tree(htype, [Token(TYPESTR, 'fp_t'), 52, 11])])])
        """
        res = self.__expand_typeinfo(typeinfo)
        res_types = []
        for suffix, tpe in res:
            children = []
            for x in portdecl.children:
                # keep to newly generated name to be a token
                new_token = copy.deepcopy(x)
                new_token = new_token.update(value=new_token.value + ('_' + suffix if suffix else ''))
                children.append(new_token)
            port = Tree(portdecl.data, children, portdecl.meta)
            res_types.append([port, tpe])
        return res_types

    def __expand_portdecltype(self, tree):
        """The expansion is achieved by first traverse the type subtree to detect the expandable type,
        then, any parent path is duplicated
        An example of tree:
        Tree(portdecltype, [Tree(inportdecl, [Token(ID, 'clk')]), Tree(htypeinfo,
            [Tree(htype, [Token(TYPESTR, 'sc_in'), Tree(htype, [Token(TYPESTR, '_Bool')])])])])
        """
        assert len(tree.children) == 2
        expanded_ports = self.__expand_helper(tree.children[0], tree.children[1])
        res = [Tree(tree.data, x, tree.meta) for x in expanded_ports]
        return res

    def __expand_vardecltype(self, tree):
        assert tree.data == 'vardeclinit'
        if len(tree.children) == 2:
            # the case without initial value
            expanded_ports = self.__expand_helper(Tree('stub', [tree.children[0]]), tree.children[1])
            expanded_ports = [[var.children[0], tpe] for var, tpe in expanded_ports]
            res = [Tree(tree.data, x, tree.meta) for x in expanded_ports]
            return res
        elif len(tree.children) == 3:
            # This case expands the vardecltype with initial value
            expanded_type = self.__expanded_type(tree.children[0])
            if expanded_type is None:
                return [tree]
            # the case with initial value, and it might be expanded
            # this is similar to blkassign
            new_node = copy.copy(tree)
            new_node.children = tree.children[0:3:2]
            # a hack to re-use the __expand_blkassign and __expand_helper
            # a formal way would be to abstract this into a assign-like top class
            new_node.children[0] = Tree('hliteral', children=[new_node.children[0]])
            # print(new_node)
            res = self.__expand_blkassign(new_node)
            decls = self.__expand_helper(new_node.children[0], Tree(data='htypeinfo', children=[expanded_type]))
            for x in zip(decls, res):
                var_name_decl, var_type = x[0]
                var_name, init_val = x[1].children
                # print('Testing: ')
                # print('var_name_decl: ', var_name_decl)
                assert var_name_decl == var_name
                x[1].children[0] = x[1].children[0].children[0]
                x[1].children.insert(1, var_type)
            # print(res)
            return res
        else:
            assert False, 'vardeclinit should contain 2 or 3 sub-trees'

    def __expand_sigdecltype(self, tree):
        return self.__expand_portdecltype(tree)

    def hsensvars(self, tree):
        """expand identifiers in sensitivity list with fields"""
        self.__push_up(tree)
        new_children = []
        for sense_var in tree.children:
            var_type = self.__expanded_type(sense_var)
            # dprint(sense_var, self.__expanded_type(sense_var))
            if var_type:
                var_type = self.__get_expandable_type_from_htype(var_type)
                type_name = var_type.children[0]
                type_params = var_type.children[1:]
                tpe = self.types[type_name]
                fields = tpe.get_fields_with_instantiation(type_params, self.types)
                for field_name, _ in fields:
                    new_children.append(sense_var + '_' + field_name)
            else:
                new_children.append(sense_var)
        tree.children = new_children
        return tree

    def modportsiglist(self, tree):
        self.__push_up(tree)
        new_children = []
        # print('Mod Port Sig List')
        for node in tree.children:
            # print(node)
            if node.data == 'portdecltype':
                var_name = node.children[0].children[0]
                var_type = node.children[1].children[0]
                var_type_name = var_type.children[1].children[0]
                if var_type_name in self.types:
                    self.__set_expanded(var_name, var_type)
                new_children.extend(self.__expand_portdecltype(node))
            elif node.data == 'sigdecltype':
                var_name = node.children[0].children[0]
                var_type = node.children[1].children[0]
                # print(var_name)
                # print(var_type)
                var_tokens = map(lambda x:
                                 filter(lambda y: isinstance(y, str), x.children),
                                 var_type.iter_subtrees_topdown())
                for var_type_name in itertools.chain.from_iterable(var_tokens):
                    if var_type_name in self.types:  # detect the first type that is in the typedef list
                        self.__set_expanded(var_name, var_type)
                        break
                new_children.extend(self.__expand_sigdecltype(node))
            else:
                # for vardeclinit, the structure is slightly different
                # no expansion for now
                var_name = node.children[0]
                var_type = node.children[1].children[0]
                var_tokens = map(lambda x:
                                 filter(lambda y: isinstance(y, str), x.children),
                                 var_type.iter_subtrees_topdown())
                type_name = var_type.children[0]
                if not Primitive.get_primitive(type_name) and not type_name in self.types:
                    # module instantiate
                    new_children.append(Tree('moduleinst', node.children, node.meta))
                    # dprint(new_children[-1])
                    # assert False
                    continue
                if type_name == 'array':
                    # array of module instantiations
                    sub_type_name = var_type.children[1].children[0]
                    if not Primitive.get_primitive(sub_type_name) and not type_name in self.types:
                        # inst_name, module_name, array_size
                        inst_arr_name = node.children[0]
                        n_inst = var_type.children[2][0]
                        inst_type = Tree('htypeinfo', children=[var_type.children[1]])
                        for i in range(n_inst):
                            inst_name = inst_arr_name + '#' + str(i)
                            new_children.append(Tree('moduleinst', [inst_name, inst_type], node.meta))
                        continue
                for var_type_name in itertools.chain.from_iterable(var_tokens):
                    if var_type_name in self.types:  # detect the first type that is in the typedef list
                        self.__set_expanded(var_name, var_type)
                        break
                res = self.__expand_vardecltype(node)
                new_children.extend(res)
            # original type
            # print('Var Name: ', var_name)
            # print('Var Type: ', var_type)
            # print()
        tree.children = new_children
        # print('---')
        # for node in tree.children:
        #     print(node)
        return tree

    def __get_expandable_type_from_htype(self, htype):
        # returns the first expandable sub-tree
        for subtree in htype.iter_subtrees_topdown():
            if isinstance(subtree, Tree) and subtree.data == 'htype' and subtree.children[0] in self.types:
                return subtree
        return None

    def __get_expandable_var_from_tree(self, tree):
        assert isinstance(tree, Tree)
        if tree.data in ['hliteral', 'hvarref']:
            var_name = tree.children[0]
            if self.__expanded_type(var_name):
                return var_name
        elif tree.data == 'harrayref':
            var_name = tree.children[0].children[0]
            if self.__expanded_type(var_name):
                return var_name
        elif tree.data == 'hvarref':
            var_name = tree.children[0]
            if self.__expanded_type(var_name):
                return var_name
        elif tree.data == 'syscread':
            # this is only used in statement vardeclinit
            assert tree.children[1].data in ['hliteral', 'hvarref'], f'Actual: {tree.children[1].data} ({tree})'
            var_name = tree.children[1].children[0]
            if self.__expanded_type(var_name):
                return var_name
        return None

    def __append_to_expandable_var_to_tree(self, tree, field_name):
        """append the field_name to the expandable variable in tree"""
        assert isinstance(tree, Tree)
        if tree.data in ['hliteral', 'hvarref']:
            var_name = tree.children[0]
            if self.__expanded_type(var_name):
                tree.children[0] = var_name + '_' + field_name
        elif tree.data == 'harrayref':
            var_name = tree.children[0].children[0]
            if self.__expanded_type(var_name):
                tree.children[0].children[0] = var_name + '_' + field_name
        elif tree.data == 'syscread':
            assert tree.children[1].data in ['hliteral', 'hvarref']
            var_name = tree.children[1].children[0]
            if self.__expanded_type(var_name):
                tree.children[1].children[0] = var_name + '_' + field_name


    def __expand_blkassign(self, tree):
        """detects the expandable variable on lhs and rhs and
        expand them with the fields"""
        # Note: we only need fields here, and we don't need the actual type
        lhs, rhs = tree.children
        # dprint('LHS ', lhs)
        # dprint('RHS ', rhs)
        lhs_var = self.__get_expandable_var_from_tree(lhs)
        rhs_var = self.__get_expandable_var_from_tree(rhs)
        # dprint('LHS var ', lhs_var)
        # dprint('RHS var ', rhs_var)
        if lhs_var is not None and (rhs_var is not None or rhs.data == 'hliteral'):
            lhs_expanded_type = self.__expanded_type(lhs_var)
            assert lhs_expanded_type is not None, '{} should have expanded type'.format(lhs_var)
            lhs_type = self.__get_expandable_type_from_htype(lhs_expanded_type)
            if rhs.data != 'hliteral':
                rhs_type = self.__get_expandable_type_from_htype(self.__expanded_type(rhs_var))
                # dprint(rhs_type)
                if lhs_type.children[0] != rhs_type.children[0]:
                    raise RuntimeError('Type does not match between LHS and RHS')
            else:
                warnings.warn('Treating CXXDefaultArgExpr as 0')
                assert rhs.data == 'hliteral'
            type_name = lhs_type.children[0]
            type_params = lhs_type.children[1:]
            tpe = self.types[type_name]
            fields = tpe.get_fields_with_instantiation(type_params, self.types)
            res = []
            for field_member, _ in fields:
                new_assign = copy.deepcopy(tree)
                if tree.data == 'blkassign':
                    new_assign.must_block = tree.must_block
                    assert type(new_assign.must_block) == type(False)
                new_lhs, new_rhs = new_assign.children
                self.__append_to_expandable_var_to_tree(new_lhs, field_member)
                if rhs.data == 'hliteral':
                    new_assign.children[1] = Tree('hliteral', [0], meta=rhs.meta)
                else:
                    self.__append_to_expandable_var_to_tree(new_rhs, field_member)
                res.append(new_assign)
            return res
        elif lhs_var is None and rhs_var is None:
            return [tree]
        elif lhs_var is not None and rhs_var is None:
            return [tree]
        else:
            raise RuntimeError('Error while expanding blkassign, LHS and RHS expandability does not match')


    def stmt(self, tree):
        # TODO: expand blkassign for aggregated types
        self.__push_up(tree)
        new_children = []
        for ch in tree.children:
            if ch.data == 'blkassign':
                res = self.__expand_blkassign(ch)
                new_children.extend(res)
            else:
                new_children.append(ch)
        tree.children = new_children
        return tree

    def __expanded_type(self, var_name):
        for d in reversed(self.expanded):
            if var_name in d:
                return d[var_name]
        return None

    def __set_expanded(self, var_name, var_type):
        if self.__expanded_type(var_name):
            raise RuntimeError('Duplicate variable ', var_name)
        self.expanded[-1][var_name] = var_type

    def hprocess(self, tree):
        """add another scope for a process"""
        self.expanded.append(dict())
        self.__push_up(tree)
        self.expanded.pop()
        return tree


    def hfunctionparams(self, tree):
        self.expanded.append(dict())
        self.__push_up(tree)
        tree.children = self.__expand_vardecl_in_tree_children(tree)
        self.expanded.pop()
        return tree

    def hmethodcall(self, tree):
        self.__push_up(tree)
        new_children = []
        for sense_var in tree.children[1:]:
            var_name = sense_var.children[0]
            var_type = self.__expanded_type(var_name)
            # dprint(var_name, self.__expanded_type(var_name))
            if var_type:
                var_type = self.__get_expandable_type_from_htype(var_type)
                type_name = var_type.children[0]
                type_params = var_type.children[1:]
                tpe = self.types[type_name]
                fields = tpe.get_fields_with_instantiation(type_params, self.types)
                for field_name, _ in fields:
                    new_children.append(var_name + '_' + field_name)
            else:
                new_children.append(sense_var)
        tree.children[1:] = new_children
        return tree

    def vardecl(self, tree):
        """for variable expansion in statement"""
        self.__push_up(tree)
        tree.children = self.__expand_vardecl_in_tree_children(tree)
        return tree

    def hfunctionlocalvars(self, tree):
        self.__push_up(tree)
        tree.children = self.__expand_vardecl_in_tree_children(tree)
        return tree

    def __expand_vardecl_in_tree_children(self, tree):
        new_children = []
        for node in tree.children:
            if node.data == 'vardeclinit':
                var_name = node.children[0]
                var_type = node.children[1].children[0]
                var_tokens = map(lambda x:
                                 filter(lambda y: isinstance(y, str), x.children),
                                 var_type.iter_subtrees_topdown())
                type_name = var_type.children[0]
                if not Primitive.get_primitive(type_name) and not type_name in self.types:
                    # module instantiate
                    assert False, 'module instantiation cannot reside in process: {}, {}'.format(var_name, type_name)
                for var_type_name in itertools.chain.from_iterable(var_tokens):
                    if var_type_name in self.types:  # detect the first type that is in the typedef list
                        self.__set_expanded(var_name, var_type)
                        break
                res = self.__expand_vardecltype(node)
                new_children.extend(res)
            else:
                new_children.append(node)
        return new_children

    def hmodinitblock(self, tree):
        """
        expands the hmodinitblock
        hmodinitblock includes a initialization block and portdecl block, both of which can include
        aggregated types
        """
        self.__push_up(tree)
        return tree

    def portbindinglist(self, tree):
        module_name, *bindings = tree.children
        new_bindings = []
        for binding in bindings:
            mod_name, sub, par = binding.children
            sub_v = sub.children[0]
            if is_tree_type(par, 'hbindingarrayref'):
                par_v = par.children[0].children[0]
            else:
                par_v = par.children[0]
            typeinfo = self.__expanded_type(par_v.value)
            if typeinfo:
                type_name = self.__get_expandable_type_from_htype(typeinfo).children[0]
                tpe = self.types[type_name]
                b = []
                for field in tpe.fields:
                    new_sub = copy.deepcopy(sub)
                    new_par = copy.deepcopy(par)
                    new_sub.children[0].value += '_' + field.children[0]
                    new_par.children[0].value += '_' + field.children[0]
                    new_binding = copy.copy(binding)
                    new_binding.children = [mod_name, new_sub, new_par]
                    b.append(new_binding)
                new_bindings.extend(b)
            else:
                new_bindings.append(binding)
        tree.children = [module_name, new_bindings]
        return tree

    def hmodule(self, tree):
        """add another scope for a module"""
        self.current_module = tree.children[0]
        self.expanded.append(dict())
        self.__push_up(tree)
        self.expanded.pop()
        return tree
