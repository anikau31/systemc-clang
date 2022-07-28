
class TypeNode(object):
    """
    This class represents any type that is present in the _hdl.txt
    Note: this type is used in two places:
        1. representing the nested type itself, with typedef, in which it is a tree like structure
        2. representing the type instantiation, which is only provided with concrete value of type params
    """
    def __init__(self, name, params, aliases, fields):
        """
        name: type name
        params: type parameteres, a list of string, in fp_t, this would be E, F
        aliases: type aliases, used to represent typedef's within a type, in block_header, this would be
                 hType typename FP::expo_t NOLIST
        fields: a list of (string, TypeNode) tuple, representing the fields, a primitive type should keep this as empty. In fp_t, this would be (frac, ... ), (expo, ... ), (sign, ... )
        """
        self.name = name
        self.params = params
        self.aliases = aliases
        self.fields = fields

    
    # The two function below (bind and build_param_dict)
    def build_param_dict(self, params):
        # This is not very efficient - could be improved with topological sorting, but we don't need that
        # 1. map type parameter to TypeNode
        type_nodes = []
        type_resolved = []
        for k, p in zip(self.params, params):
            type_nodes.append((k, p))
            type_resolved.append((k, False))
        type_nodes = dict(type_nodes)
        type_resolved = dict(type_resolved)
        type_resolved_count = 0
        for k, v in type_nodes.items():
            if type(v) is not type(self):
                type_resolved_count += 1
        # 2. instantiate the types
        while type_resolved_count != len(type_nodes):
            for k in type_nodes.keys():
                if type_resolved[k]:
                    continue
                else:
                    if len(type_nodes[k].params) == 0:
                        type_nodes[k] = type_nodes[k].instantiate() 
                        type_resolved_count += 1
                    else:
                        raise
        return type_nodes

    def bind(self, param_list, param_dict=None):
        """
        if params_dict is not empty, use params_dict, otherwise, first build param dict
        currently, things like template<E, sc_in<E> > is not supported
        """
        param_dict = param_dict or self.build_param_dict(param_list)
        fields = []
        for field_name, field_type in self.fields:
            if len(field_type.params) > 0:
                instantiate_list = [ param_dict[p.name] if p.name in param_dict else p.instantiate() for p in field_type.params ]
                res = field_type.instantiate(params=instantiate_list)
                fields.append((field_name, res))
            else:
                fields.append((field_name, field_type.instantiate()))
        fields = [ (str(field[0]), field[1]) for field in fields ]
        return aggregate(params=None, fields=fields)

    def instantiate(self, params=None):
        # create an object with to_str for the classes
        primitive_type = Primitive.get_primitive(self.name)
        is_int = re.match(r'^[-+]?[0-9]+$', self.name)
        # print('Instantiating: ', self.name, ' with ', params)

        if primitive_type:
            if params is None:
                res = primitive_type(*list(map(lambda x: x.instantiate(), self.params)))
            else:
                res = primitive_type(*list(params))
            return res
        elif is_int:
            return int(self.name)
        else:
            # Non-primitive type, like fp_t
            res = aggregate(params, self.fields)
        assert CType.types.is_custom_type(self.name), f'{self.name} is not a custom type'
        t = CType.types.get_custom_type(self.name)
        return t.bind(param_list=self.params)

    # Use this class as a metaclass and register other types?


