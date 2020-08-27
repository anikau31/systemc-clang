from lark import Tree
from copy import deepcopy

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
    def __init__(self, name, type_param_names, fields):
        self.name = name
        self.type_param_names = type_param_names
        self.fields = fields

    def _instantiate(self, field, params, types):
        """Note: nested type is not supported"""
        if isinstance(field, Tree):
            if field.data == 'htype':
                if field.children[0] in types:
                    raise NotImplementedError('Nested type support is WIP')
                new_children = []
                for x in field.children:
                    if x in params:
                        new_children.append(params[x])
                    else:
                        new_children.append(self._instantiate(x, params, types))
                field.children = new_children
                return field
            else:
                field.children = [self._instantiate(x, params, types) if isinstance(x, Tree) else x for x in field.children]
                return field
        return field

    def get_fields_with_instantiation(self, params, types):
        param_maps = dict(zip(self.type_param_names, params))
        instantiated_fields = deepcopy(self.fields)
        instantiated_fields = list(map(lambda x: self._instantiate(x, param_maps, types), instantiated_fields))
        res = [[f.children[0], f.children[1]] for f in instantiated_fields]
        return res
