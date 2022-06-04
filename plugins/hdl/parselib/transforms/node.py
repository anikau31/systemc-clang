from lark import Tree
from copy import deepcopy
from ..utils import dprint, is_tree_type, get_ids_in_tree, alternate_ids, set_ids_in_tree_dfs


class Node:
    def __init__(self):
        pass


class Statement(Tree):
    def __init__(self):
        pass


class Expression(Tree):
    def __init__(self):
        pass


class Always(Tree):
    def __init__(self, tree):
        super().__init__(tree.data, tree.children, tree.meta)


class ArrayDeref(Tree):
    def __init__(self, tree):
        super().__init__(tree.data, tree.children, tree.meta)


class TypeDefType:
    def __init__(self, name, type_param_names, aliases, fields):
        self.name = name
        self.type_param_names = type_param_names
        self.aliases = aliases
        self.fields = fields

    def _instantiate(self, field, params, types):
        """
        Instantiate a field in a type, given params map between the template parameter and argument,
        and fields to instantiate
        Note: nested type is not supported
        """
        if not isinstance(field, Tree):
            return field
        if isinstance(field, Tree):
            if is_tree_type(field, 'htype'):
                if field.children[0] in types:
                    raise NotImplementedError('Nested type support is WIP')
                new_children = []
                for x in field.children:
                    if x in params:
                        new_children.append(params[x])
                    else:
                        new_children.append(self._instantiate(x, params, types))
                field.children = new_children
                # if len(field.children) == 1 and not isinstance(field.children[0], Tree):
                #     return field.children[0]
                return field
            elif is_tree_type(field, 'hdeptype'):
                # Check for local hTypeAlias
                type_name = field.children[0]
                if type_name in self.aliases:
                    original_type = self.aliases[type_name]
                    return self._instantiate(original_type, params, types)
                else:
                    # query the global scope
                    # e.g. FP::expo_t
                    path_to_type = type_name.split('::')
                    assert len(path_to_type) == 2, "Only two levels of dependent type is supported (e.g. FP::expo_t is supported but FP::X::expo_t is not)"
                    for component in path_to_type[:-1]:
                        # maps parameter to argument (e.g. FP -> fp_t)
                        assert component in params, "{} not found when processing dependent type {}".format(component, type_name)
                        t = params[component]  # get fp_t
                        assert t.data == 'htype', "Dependent type {} should be an htype (e.g. in FP::expo_t, FP must be htype)".format(component)
                        tname, tparams = t.children[0], t.children[1:]
                        aliases = types[t.children[0]].get_alias_type_with_instantiation(tparams, types)
                    last_path = path_to_type[-1]
                    res = aliases[last_path]
                    # returning some types
                    return self._instantiate(res, params, types)
            else:
                # dprint(field)
                assert field.data == 'htypefield', "Generated field should be of type hTypeField, got {}".format(field.data)
                field.children = [self._instantiate(x, params, types) if isinstance(x, Tree) else x for x in field.children]
                return field

    def get_fields_with_instantiation(self, params, types):
        """
        NOTE:
            types is the global collected type
            The type of hTypeField is searched in three scopes if including alias type
            current type,
            type parameter's scope,
            global scope
        """
        param_maps = dict(zip(self.type_param_names, params))
        instantiated_fields = deepcopy(self.fields)
        # dprint(instantiated_fields)
        instantiated_fields = list(map(lambda x: self._instantiate(x, param_maps, types), instantiated_fields))
        # dprint(instantiated_fields)
        res = [[f.children[0], f.children[1]] for f in instantiated_fields]
        # dprint(res)
        return res

    def get_alias_type_with_instantiation(self, params, types):
        param_maps = dict(zip(self.type_param_names, params))
        aliases = dict(map(lambda x: (x[0], self._instantiate(x[1], param_maps, types)), deepcopy(self.aliases).items()))
        return aliases
