import re, warnings
from lark import Lark, Transformer, Visitor

from parselib.utils import p
from .type_node import TypeNode


class TypeCollector(Transformer):
    def __init__(self, skip=None, *args, **kwargs):
        self.custom_types = dict()
        self.module_types = dict()
        self.current_bindings = []
        pass

    def is_module_type(self, name):
        for key, _ in self.module_types.items():
            if key.startswith(str(name)):
                return key
        return None

    def get_port_bindings(self, name, parent=None):
        # TODO: fix the portbinding hack
        # list of port bindings
        return self.module_types[name]

    def is_custom_type(self, name):
        return name in self.custom_types

    def get_custom_type(self, name):
        return self.custom_types[name]

    def htype(self, args):
        if len(args) == 1:
            if re.match(r'(\+|\-)?0-9+', str(args[0])):
                return int(args[0])
            else:
                return TypeNode(name=str(args[0]), params=[], fields=[])
        else:
            return TypeNode(name=str(args[0]), params=args[1:], fields=[])


    def htypedef(self, args):
        type_name = str(args[0])
        # The check should be done together with type parameters
        if type_name in self.custom_types:
            raise Exception('repeated type definition')
        self.custom_types[type_name] = TypeNode(name=type_name, params=args[1], fields=args[2])

    def hmodule(self, args):
        # TODO: fix this hack
        mod_name = str(args[0])
        self.current_mod = mod_name
        if mod_name in self.module_types:
            raise Exception('repeated module definition')
        self.module_types[mod_name] = []
        self.module_types[mod_name].extend(self.current_bindings)
        self.current_bindings = []


    def hvarref(self, args):
        return args[0]


    def htypeint(self, args):
        return int(args[0])


    def htypefields(self, args):
        return args

    def htypefield(self, args):
        return (args[0], args[1])

    def htypetemplateparams(self, args):
        return args

    def htypetemplateparam(self, args):
        return str(args[0])


