"""Primitive types from System C and C++"""
import copy


class TypeContext(object):
    def __init__(self, prefix=None, suffix=None, sep=None):
        self.prefix = prefix
        self.suffix = suffix
        self.sep = sep

    @staticmethod
    def merge_field_default_this(this, that):
        if this is None and that is not None:
            return that
        elif this is not None and that is None:
            return this
        elif this is None and that is None:
            return None
        else:
            return that

    def update_not_none(self, that):
        """merges fields of two contexts, throws error if both fields are non None"""
        if that is None:
            return copy.copy(self)
        prefix = TypeContext.merge_field_default_this(self.prefix, that.prefix)
        suffix = TypeContext.merge_field_default_this(self.suffix, that.suffix)
        sep = TypeContext.merge_field_default_this(self.sep, that.sep)
        return TypeContext(prefix=prefix, suffix=suffix, sep=sep)


class Primitive:
    """
    Primitive base class,
    handles automatically subclass registration for primitive types
    """
    primitive_type_names = []
    name_mapping = { '_Bool': 'cppbool', 'unsigned_int': 'cppuint', 'int': 'cppint', 'unsigned_short': 'cppushort', 'short': 'cppshort' }

    def __init_subclass__(cls, **kwargs):
        """registers subclass automatically"""
        super().__init_subclass__(**kwargs)
        cls.primitive_type_names.append(cls)

    @staticmethod
    def get_primitive_name_dict():
        """convert the primitive name string -> type to a dict"""
        return dict(zip(map(lambda x: x.__name__, Primitive.primitive_type_names), Primitive.primitive_type_names))

    @staticmethod
    def get_primitive(name):
        """get the primitive type of name"""
        type_dict = Primitive.get_primitive_name_dict()
        name = Primitive.name_filter(name)
        if name in type_dict:
            return type_dict[name]
        else:
            return None

    @staticmethod
    def name_filter(name):
        """maps type name to internal name"""
        if name in Primitive.name_mapping:
            return Primitive.name_mapping[name]
        else:
            return name


# TODO: use decorators to embed context
# TODO: use context object instead of string
# TODO: not clear when to decide the `logic' part
class sc_in(Primitive):
    def __init__(self, T):
        self.T = T

    @staticmethod
    def get_context():
        return TypeContext(prefix='input')

    def to_str(self, var_name, context=None):
        input_context = TypeContext(prefix='input', suffix=',').update_not_none(context)
        return self.T.to_str(var_name=var_name, context=input_context)

class __func_inout(Primitive):
    def __init__(self, T):
        self.T = T

    @staticmethod
    def get_context():
        return TypeContext(prefix='inout')

    def to_str(self, var_name, context=None):
        input_context = TypeContext(prefix='inout', suffix=',').update_not_none(context)
        return self.T.to_str(var_name=var_name, context=input_context)

class sc_rvd_out(Primitive):
    def __init__(self, T):
        self.T = T

    def to_str(self, var_name, context=None):
        output_context = TypeContext(prefix='output', suffix=',')  # 
        input_context = TypeContext(prefix='input', suffix=',')  # 
        # print('sc_rvd_out ' + var_name)
        res = self.T.to_str(var_name + '_data', context=output_context)
        if res[-1] == ',':
            res = res[0:-1]
        # create 
        res += ',\n' + sc_in(sc_bv(1)).to_str(var_name + '_valid') + ',\n' + sc_out(sc_bv(1)).to_str(var_name + '_ready')
        # print('...', res)
        return res


class sc_rvd_in(Primitive):
    def __init__(self, T):
        self.T = T

    def to_str(self, var_name, context=None):
        output_context = TypeContext(prefix='output', suffix=',')  # 
        input_context = TypeContext(prefix='input', suffix=',')  # 
        # print('sc_rvd_in ' + var_name)
        res = self.T.to_str(var_name + '_data', context=input_context)
        if res[-1] == ',':
            res = res[0:-1]
        # create 
        res += ',\n' + sc_out(sc_bv(1)).to_str(var_name + '_valid') + ',\n' + sc_in(sc_bv(1)).to_str(var_name + '_ready')
        # print('...', res)
        return res


class sc_out(Primitive):
    def __init__(self, T):
        self.T = T

    def to_str(self, var_name, context=None):
        output_context = TypeContext(prefix='output', suffix=',').update_not_none(context)
        return self.T.to_str(var_name=var_name, context=output_context)
        

class sc_bv(Primitive):
    def __init__(self, width):
        self.width = width

    def to_str(self, var_name, context=None):
        prefix = ''
        suffix = ';'
        if context:
            if context.prefix is not None:
                prefix = context.prefix + ' '
            if context.suffix is not None:
                suffix = context.suffix
        if var_name:
            return f'{prefix}logic [{self.width-1}:0] {var_name}{suffix}'
        else:
            return f'{prefix}logic [{self.width-1}:0]'

class sc_uint(Primitive):
    def __init__(self, width):
        self.width = width

    def to_str(self, var_name, context=None):
        prefix = ''
        suffix = ';'
        if context:
            if context.prefix is not None:
                prefix = context.prefix + ' '
            if context.suffix is not None:
                suffix = context.suffix
        if var_name:
            return f'{prefix}logic [{self.width-1}:0] {var_name}{suffix}'
        else:
            return f'{prefix}logic [{self.width-1}:0]'

class sc_int(Primitive):
    def __init__(self, width):
        self.width = width

    def to_str(self, var_name, context=None):
        prefix = ''
        suffix = ';'
        if context:
            if context.prefix is not None:
                prefix = context.prefix + ' '
            if context.suffix is not None:
                suffix = context.suffix
        if var_name:
            return f'{prefix}logic signed[{self.width-1}:0] {var_name}{suffix}'
        else:
            return f'{prefix}logic signed[{self.width-1}:0]'


class sc_signal(Primitive):
    def __init__(self, T):
        self.T = T

    def to_str(self, var_name, context=None):
        return self.T.to_str(var_name, context)

class array(Primitive):
    def __init__(self, T, size):
        self.T = T
        self.sz = size

    def to_str(self, var_name, context=None):
        sz_str = ''.join(['[0:{}]'.format(sz - 1) for sz in self.sz])
        return self.T.to_str('{}{}'.format(var_name, sz_str), context)

class cppbool(Primitive):
    def __new__(cls):
        return sc_uint(1)


class cppint(Primitive):
    def __new__(cls):
        return sc_int(32)


class cppuint(Primitive):
    def __new__(cls):
        return sc_uint(32)

class cppshort(Primitive):
    def __new__(cls):
        return sc_int(16)

class cppushort(Primitive):
    def __new__(cls):
        return sc_uint(16)

class sc_clock(Primitive):
    def __new__(cls):
        return sc_uint(1)


class vmodule(object):
    def __init__(self, type_name, port_bindings=None):
        self.type_name = type_name
        self.port_bindings = port_bindings

    def to_str(self, var_name, context=None):
        warnings.warn('port binding not fully implemented')
        binding_str = ',\n'.join(f'.{b[0]}({b[1]})' for b in self.port_bindings)
        return f'{self.type_name} {var_name}(\n{binding_str}\n/* port bindings not fully implemented */);'


