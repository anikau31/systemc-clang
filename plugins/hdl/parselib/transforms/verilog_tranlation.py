import warnings
from .top_down import TopDown
from ..primitives import *
from ..utils import dprint, is_tree_type, get_ids_in_tree
from lark import Tree, Token
from functools import reduce
import pprint
import logging


class VerilogTranslationPass(TopDown):
    """Translate low-level format of the _hdl.txt into Verilog
    Note that type defs are already expanded at this point, so all htypeinfo/htype should only include primitive types
    This pass does not perform any tree transformation that alters the semantics, but **only** generates Verilog
    """
    def __init__(self):
        super().__init__()
        self.indent_character = ' '
        self.current_indent = 0
        self.indent_inc = 2
        self.indent_stack = list()
        self.bindings = dict()
        self.local_variables = set()
        self.in_for_init = False
        self.module_var_type = None
        self.current_proc_name = None
        self.__current_scope_type = [None]
        self.is_in_thread = False
        self.thread_comb = False

    def get_current_scope_type(self):
        """denotes one of four types of scope: loop, switch, branch, None
        currently, this is only useful for determine the handling of breaks
        """
        return self.__current_scope_type[-1]

    def push_current_scope_type(self, scope):
        assert scope in ['loop', 'switch', 'branch'], 'Incorrect scope type'
        self.__current_scope_type.append(scope)

    def pop_current_scope_type(self):
        self.__current_scope_type.pop()


    def start(self, tree):
        self.__push_up(tree)
        return tree.children[0]

    def modulelist(self, tree):
        self.__push_up(tree)
        res = '\n'.join(tree.children)
        return res

    def nonrefexp(self, tree):
        self.__push_up(tree)
        assert len(tree.children) == 1
        return tree.children[0]

    def __get_var_name(self, tree):
        if isinstance(tree, str):
            return tree
        elif isinstance(tree, Tree):
            if tree.data in ['hvarref']:
                return tree.children[0]
            elif tree.data in ['harrayref', 'hslice']:
                return self.__get_var_name(tree.children[0])
        assert False, 'Cannot extract variable name from {}'.format(tree)

    def __get_var_names(self, tree):
        """return a list of variable names"""
        if isinstance(tree, Tree) and tree.data in ['hconcat']:
            return reduce(lambda x, y: x + y, [self.__get_var_names(sub) for sub in tree.children])
        else:
            return [self.__get_var_name(tree)]


    def hmethodcall(self, tree):
        self.__push_up(tree)
        return '{}({})'.format(tree.children[0], ','.join(map(str, tree.children[1:])))

    def hwait(self, tree):
        warnings.warn('hwait encountered, not implemented')
        return "// hwait"

    def hvarinitlist(self, tree):
        self.__push_up(tree)
        return '{' + ','.join(tree.children) + '}'

    def blkassign(self, tree):
        # dprint("--------------START----------------")
        current_proc = self.get_current_proc_name()
        sense_list = self.get_sense_list()

        # dprint(self.current_module, ':', current_proc)
        # dprint('Sensitivity: ', pprint.pformat(self.get_sense_list()))
        # dprint('Var w/ type: ', pprint.pformat(self.module_var_type))
        var_names = self.__get_var_names(tree.children[0])
        tpes = [self.get_current_module_var_type_or_default(vn) for vn in var_names]
        all_none = all(t is None for t in tpes)
        all_non_none = all(t is not None for t in tpes)
        if not all_none and not all_non_none:
            raise ValueError('LHS of assignment must be all local variables or all non-local variables. On line: {}.'.format(tree.line))
        is_nb = [isinstance(tpe, sc_signal) or
                     isinstance(tpe, sc_out) or
                     isinstance(tpe, array) and isinstance(tpe.get_element_type(), sc_signal) for tpe in tpes]
        # is_nb checks whether one of the type needs to be non-blocking assignment
        # and special case for thread
        all_nb = all(is_nb) or current_proc in ['#thread_sync#']
        all_b = all(not p for p in is_nb) or (self.is_in_thread and current_proc in ['#function#'])
        if not all_nb and not all_b:
            raise ValueError('The assignment must not mix blocking assignment and non-blocking assignment. On line: {}.'.format(tree.line))

        # var_name = self.__get_var_name(tree.children[0])
        # tpe = self.get_current_module_var_type_or_default(var_name)
        # if tpe is None, it is either a local variable within a process or block, in this case, it should be =
        # if current_proc not in ['#initblock#', '#function#']:  # TODO: use a more programmatic way
        # dprint("--------------END-------------------")
        blocking = True
        # while using non-blocking assignments in function is not recommended, we will need to use
        # non-blocking assignments in a function so that its value can get properly assigned
        # An example of such is the m_bits_data_valid signal in encode_stream
        # In SystemC, when a signal is used in RHS, it will be added to the sensitivity list
        if current_proc in sense_list or (current_proc in ['#function#', '#thread_sync#'] and not (self.is_in_thread and current_proc == '#function#')):
            # sense_list = sense_list[current_proc]
            # tpe is only recorded if the declaration crosses process boundary
            # if tpe is not None:
            if all_non_none:
                #     dprint(tpe.get_element_type())
                # if isinstance(tpe, sc_signal) or \
                #         isinstance(tpe, sc_out) or \
                #         isinstance(tpe, array) and isinstance(tpe.get_element_type(), sc_signal):
                #    dprint('Changed to non-blocking assignment: '.format(var_name))
                if all_nb:
                    blocking = False
        if self.thread_comb:
            blocking = True
        self.__push_up(tree)
        assert len(tree.children) == 2
        is_local_var = self.__all_local_variables(var_names)
        op = '=' if self.in_for_init or blocking or is_local_var else '<='
        l = tree.children[0]
        r = tree.children[1]


        if type(l) == Tree and l.data == 'harrayref':
            # __A[__r:__s] = __X
            __A = l.children[0]
            __r = l.children[1]
            __s = l.children[2]
            l = __A
            if type(r) == Tree and r.data == 'harrayref':  # special case for irreducible RHS
                __X = r.children[3]
                __B = r.children[0]
            else:
                __X = r
                __B = r
            r = "(({} & ~(~($bits({})'('b0)) << (({})-({})+1))) << ({})) | (({}) & ((~($bits({})'('b0)) ) << (({}) + 1) | ~(( ~($bits({})'('b0)) ) << ({}))))".format(
                __X, __B,
                __r, __s,
                __s, __A,
                __A, __r, __A, __s
            )
        elif type(r) == Tree and r.data == 'harrayref':
            r = r.children[3]

        res = '{} {} {}'.format(l, op, r)
        return res

    def syscwrite(self, tree):
        raise RuntimeError('Unsupported node: syscwrite')
        self.__push_up(tree)
        res = '{} {} {}'.format(tree.children[1], tree.children[0], tree.children[2])
        return res


    def numlitwidth(self, tree):
        self.__push_up(tree)
        lit, tpe = tree.children
        assert hasattr(tpe, 'width'), 'Literal width type should have width member'
        w = tpe.width  # the primitive type must have width
        return "{}{}'d{}".format('-' if lit < 0 else '', w, abs(lit))

    def hcondop(self, tree):
        self.__push_up(tree)
        return '{} ? {} : {}'.format(tree.children[0], tree.children[1], tree.children[2])

    def hliteral(self, tree):
        """stops at literal, it is some kinds of terminal"""
        self.__push_up(tree)
        assert len(tree.children) == 1
        return str(tree.children[0])

    def hvarref(self, tree):
        assert len(tree.children) == 1
        if is_tree_type(tree.children[0], 'func_param_name_stub'):
            return tree.children[0].children[0]
        stripped = tree.children[0].replace("#", "")
        return stripped

    def syscread(self, tree):
        """syscread: hsigassignr, token"""
        self.__push_up(tree)
        return tree.children[1]

    def __check_const(self, tree):
        """check whether the tree valuates to constant"""
        if isinstance(tree, int):
            return True
        elif is_tree_type(tree, 'hliteral'):
            return True
        elif is_tree_type(tree, 'hbinop'):
            return self.__check_const(tree.children[1]) and self.__check_const(tree.children[2])
        return False

    def _clean_harrayref(self, harrayref_node):
        assert harrayref_node.data == 'harrayref'
        if is_tree_type(harrayref_node.children[0], 'hsigassignr'):
            # dprint(harrayref_node)
            harrayref_node.children = harrayref_node.children[1:]
            # dprint(harrayref_node)

    def harrayref(self, tree):
        # Check whether
        l_const = None
        r_const = None
        self._clean_harrayref(tree)
        if isinstance(tree.children[0], Tree) and tree.children[0].data == 'hslice':
            if len(tree.children[0].children) == 3:
                hslice = tree.children[0]
                l, r = hslice.children[1:]
                l_const = self.__check_const(l)
                r_const = self.__check_const(r)

        self.__push_up(tree)
        if isinstance(tree.children[0], Tree) and tree.children[0].data == 'hslice':
            hslice = tree.children[0]
            var = hslice.children[0]
            m = None
            if len(hslice.children) == 3:
                l, r = hslice.children[1:]
            elif len(hslice.children) == 2:
                m = hslice.children[1]
            # l_const = isinstance(l, int)
            # r_const = isinstance(r, int)
            if l_const and r_const:
                idx = '{}:{}'.format(l, r)
            elif m is not None:
                idx = '{}'.format(m)
            else:
                # for slicing that is not constant
                tree.children = [var, l, r, "(({}) >> ({})) & ~(~($bits({})'('b0)) << (({}) - ({}) + 1))".format(var, r, var, l, r)]
                return tree  # irreducible hslice node
        else:
            var, idx = tree.children
        res = '{}[{}]'.format(var, idx)
        return res

    def hsigassignl(self, tree):
        warnings.warn('Implementing SigAssignL as non-blocking')
        return '<='

    def __is_local_variable(self, var_name):
        return var_name in self.local_variables

    def __all_local_variables(self, var_names):
        return all(self.__is_local_variable(var_name) for var_name in var_names)

    def hbinop(self, tree):
        self.__push_up(tree)
        if tree.children[0] == 'ARRAYSUBSCRIPT':
            res = '{}[({})]'.format(tree.children[1], tree.children[2])
        else:
            op = tree.children[0]
            if op == '=':
                pass
                # op = '<='
            if op == ',':  # concatenation
                res = '{{({}), ({})}}'.format(tree.children[1], tree.children[2])
            elif op == '>>':  # Note: shift right
                res = '({}) {} ({})'.format(tree.children[1], '>>>', tree.children[2])
            elif op == 'concat':
                res = '{{ ({}) {} ({}) }}'.format(tree.children[1], ',', tree.children[2])
            elif op == '@=':  # non-blocking assignment in thread context
                res = '{} <= ({})'.format(tree.children[1], tree.children[2])
            else:
                res = '({}) {} ({})'.format(tree.children[1], op, tree.children[2])
        return res

    def hpostfix(self, tree):
        self.__push_up(tree)
        return "{}{}".format(tree.children[1], tree.children[0])

    def hprefix(self, tree):
        self.__push_up(tree)
        return "{}{}".format(tree.children[0], tree.children[1])

    def hunop(self, tree):
        self.__push_up(tree)
        if len(tree.children) == 1:
            return tree.children[0]
        elif len(tree.children) == 2:
            res = '{}({})'.format(tree.children[0], tree.children[1])
            return res
        else:
            assert False
        # The ++ and -- only shows in loop
        # The original method is deprecated, we can hand over the self-increment to the synthesizer
        # since we assue that we generate system verilog
        # if tree.children[0] == '++':
        #     res = '{} = {} + 1'.format(tree.children[1], tree.children[1])
        # elif tree.children[0] == '--':
        #     res = '{} = {} - 1'.format(tree.children[1], tree.children[1])
        # else:
        #     res = '{}({})'.format(tree.children[0], tree.children[1])
        # return res

    def hcstmt(self, tree):
        self.__push_up(tree)
        assert len(tree.children) <= 1, "hcstmt should contain 0 or 1 child"
        if tree.children:
            return tree.children[0]
        else:
            return ''

    def continuestmt(self, tree):
        return 'continue'

    def get_current_ind_prefix(self):
        ind = self.current_indent * self.indent_character
        return ind

    def casevalue(self, tree):
        self.__push_up(tree)
        return tree.children[0]

    def switchbody(self, tree):
        self.push_current_scope_type('switch')
        self.__push_up(tree)
        self.pop_current_scope_type()
        return '\n'.join(tree.children)

    def casestmt(self, tree):
        self.inc_indent()
        self.__push_up(tree)
        self.dec_indent()
        ind = self.get_current_ind_prefix()
        res = '{}{}: begin\n{}\n{}end'.format(ind, tree.children[0], '\n'.join(tree.children[1:]), ind)
        return res

    def switchcond(self, tree):
        self.__push_up(tree)
        return tree.children[0]

    def switchstmt(self, tree):
        self.inc_indent()
        self.__push_up(tree)
        self.dec_indent()
        ind = self.get_current_ind_prefix()
        res = '{}case({})\n{}\n{}endcase'.format(ind, tree.children[0], tree.children[1], ind)
        return res

    def breakstmt(self, tree):
        if self.get_current_scope_type() in ['switch']:
            return ""
        else:
            ind = self.get_current_ind_prefix()
            res = '{}break;'.format(ind)
            return res

    def stmt(self, tree):
        indentation = []
        sep = []
        noindent = ['hcstmt', 'ifstmt', 'forstmt', 'switchstmt', 'casestmt', 'breakstmt', 'whilestmt', 'dostmt']
        nosemico = ['hcstmt', 'ifstmt', 'forstmt', 'switchstmt', 'casestmt', 'breakstmt', 'whilestmt', 'dostmt']
        for x in tree.children:
            if x.data in noindent:
                indentation.append('')
            else:
                indentation.append(self.get_current_ind_prefix())
            if x.data in nosemico:
                sep.append('')
            else:
                sep.append(';')

        self.__push_up(tree)
        def f_concat(x):
            try:
                if isinstance(x[1], Tree):
                    if x[1].data == 'expression_in_stmt':
                        # logging.warning('Expression as a statement may not have an effect. On line: {}'.format(x[1].line))
                        x = (x[0], x[1].children[0], x[2])
                    else:
                        assert False, 'Unrecognized construct: {}'.format(x[1])
                res = str(x[0]) + str(x[1]) + str(x[2])
                return res
            except Exception as e:
                print(x[0])
                print(x[1])
                print(x[2])
                raise
        res = '\n'.join(map(f_concat,
                            filter(lambda x: x[1] is not None, zip(indentation, tree.children, sep))
                            ))
        return res

    def hnoop(self, tree):
        warnings.warn('Encountered noop at line: {}'.format(tree.meta.line))
        return ""

    def whilestmt(self, tree):
        self.push_current_scope_type('loop')
        self.inc_indent()
        self.__push_up(tree)
        self.dec_indent()
        prefix = self.get_current_ind_prefix()
        res = "{}while({}) begin\n".format(prefix, tree.children[0])
        res += ''.join(tree.children[1:])
        res += '\n{}end'.format(prefix)
        self.pop_current_scope_type()
        return res

    def dostmt(self, tree):
        self.push_current_scope_type('loop')
        self.inc_indent()
        self.__push_up(tree)
        self.dec_indent()
        prefix = self.get_current_ind_prefix()

        res = "{}do begin\n".format(prefix)
        res += ''.join(tree.children[1:])
        res += '\n{}end while({})'.format(prefix, tree.children[0])

        self.pop_current_scope_type()
        return res

    def stmts(self, tree):
        self.__push_up(tree)
        res = '\n'.join(tree.children)
        return res

    def inc_indent(self):
        self.current_indent += self.indent_inc

    def dec_indent(self):
        self.current_indent -= self.indent_inc

    def push_indent(self):
        """used to service temporary indent removal, such as that in for condition"""
        self.indent_stack.append(self.current_indent)
        self.current_indent = 0
    def pop_indent(self):
        self.current_indent = self.indent_stack.pop()

    def ifstmt(self, tree):
        self.push_current_scope_type('branch')
        self.inc_indent()
        self.__push_up(tree)
        self.dec_indent()
        ind = self.get_current_ind_prefix()
        res = ind + 'if ({}) begin\n'.format(tree.children[0])
        if len(tree.children) > 1:
            res += tree.children[1] + '\n'
        res += ind + 'end'
        # print('If Body: ', tree.children[1])
        if len(tree.children) == 3:
            res += ' else begin\n' + tree.children[2]
            res += '\n'
            res += ind + 'end\n'

        self.pop_current_scope_type()
        return res

    def forinit(self, tree):

        self.in_for_init = True
        self.__push_up(tree)
        self.in_for_init = False

        if tree.children:
            return tree.children[0]
        else:
            return ''

    def forcond(self, tree):
        self.__push_up(tree)
        return tree.children[0]

    def forpostcond(self, tree):
        self.__push_up(tree)
        return tree.children[0]

    def forbody(self, tree):
        self.__push_up(tree)
        return tree.children[0]

    def forstmt(self, tree):

        self.push_current_scope_type('loop')
        new_children = []
        self.push_indent()
        new_children.extend(self.visit(t) for t in tree.children[:3])
        self.pop_indent()

        self.inc_indent()
        new_children.extend(self.visit(t) for t in tree.children[3:])
        self.dec_indent()

        for_init, for_cond, for_post, for_body = new_children

        ind = self.get_current_ind_prefix()
        res = ind + 'for ({};{};{}) begin\n'.format(for_init, for_cond, for_post)
        res += for_body + '\n'
        res += ind + 'end'
        self.pop_current_scope_type()
        return res

    def hsensvars(self, tree):
        self.__push_up(tree)
        return tree

    def npa(self, tree):
        return tree.children[0]

    def hsensedge(self, tree):
        self.__push_up(tree)
        return tree.children[0]

    def get_sense_list(self):
        return self.senselist

    def hsenslist(self, tree):
        self.__push_up(tree)
        proc_name = tree.children[0]
        assert proc_name not in self.senselist, 'Duplicated process: {}'.format(proc_name)
        self.senselist[proc_name] = []
        for sv in tree.children[1:]:
            # special treatment
            sens_var, sens_edge = sv.children
            if is_tree_type(sv.children[0], "hsensvar"):
                warnings.warn("Malformatted sensitivity list")
                sens_edge, sens_var  = sv.children[0].children
                if sens_edge == 'posedge_event':
                    edge = 'posedge'
                elif sens_edge == 'negedge_event':
                    edge = 'negedge'
                else:
                    edge = ''
                sen_str = '{} {}'.format(edge, sens_var)
            else:
                if isinstance(sens_var, Token):
                    sens_var = sens_var.value
                if sens_edge == 'always':
                    sen_str = sens_var
                elif sens_edge in ['pos', 'posedge_event']:
                    sen_str = 'posedge {}'.format(sens_var)
                elif sens_edge in ['neg', 'negedge_event']:
                    sen_str = 'negedge {}'.format(sens_var)
                else:
                    raise ValueError('Edge can only be one of pos/neg/always')
            self.senselist[proc_name].append(sen_str)
        return None

    def hvalchange(self, tree):
        warnings.warn('value change is deprecated, but is detected in hcode', DeprecationWarning)
        self.__push_up(tree)
        return tree.children[0]

    def set_current_proc_name(self, name):
        self.current_proc_name = name

    def reset_current_proc_name(self):
        self.current_proc_name = None

    def get_current_proc_name(self):
        return self.current_proc_name

    def __is_synchronous_sensitivity_list(self, sense_list):
        return any('posedge' in x or 'negedge' in x for x in sense_list)

    def hprocess(self, tree):
        proc_name, proc_name_2, prevardecl, *body = tree.children
        self.set_current_proc_name(proc_name)
        for n in prevardecl.children:
            var_name = n.children[0].children[0]  # get the variable name of local variables
            self.__add_local_variables(var_name)
        self.inc_indent()
        self.__push_up(tree)
        self.dec_indent()

        proc_name, proc_name_2, prevardecl, *body = tree.children

        prevardecl.children = list(filter(lambda x: not is_tree_type(x, 'vardeclrn'), prevardecl.children))

        ind = self.get_current_ind_prefix()
        decls = list(map(lambda x: x[0] + ';', prevardecl.children))
        decls_init = list(map(lambda x: '{} = {};'.format(x[1], x[2]), filter(lambda x: len(x) == 3 and x[2] is not None, prevardecl.children)))
        sense_list = self.get_sense_list()
        assert proc_name in sense_list, "Process name {} is not in module {}".format(proc_name, self.current_module)
        # res = ind + 'always @({}) begin: {}\n'.format(' or '.join(self.get_sense_list()[proc_name]), proc_name)
        if self.__is_synchronous_sensitivity_list(sense_list[proc_name]):
            res = ind + 'always_ff @({}) begin: {}\n'.format(' or '.join(self.get_sense_list()[proc_name]), proc_name)
        else:
            res = ind + 'always @({}) begin: {}\n'.format(' or '.join(self.get_sense_list()[proc_name]), proc_name)
            # res = ind + 'always_comb begin: {}\n'.format(proc_name)
        self.inc_indent()
        ind = self.get_current_ind_prefix()
        res += ind + ('\n' + ind).join(decls) + '\n'
        res += ind + ('\n' + ind).join(decls_init) + '\n'
        self.dec_indent()
        ind = self.get_current_ind_prefix()
        res += '\n'.join(body) + '\n'
        res += ind + 'end'
        self.__reset_local_variables()
        self.reset_current_proc_name()
        return res

    def __reset_local_variables(self):
        self.local_variables = set()

    def __add_local_variables(self, var_name):
        assert var_name not in self.local_variables, 'Local variable {} already existed'.format(var_name)
        self.local_variables.add(var_name)

    def htype(self, tree):
        self.__push_up(tree)
        name, *args = tree.children
        tpe = Primitive.get_primitive(name)
        assert tpe is not None, 'Type {} is not defined'.format(name)
        return tpe(*args)

    def hreturnstmt(self, tree):
        self.__push_up(tree)
        logging.warning(
            """Return statement is detected and omitted.\n"""
            """  A return statement may not produce expected result,\n"""
            """  consider removing it in the C++ code.\n"""
        )
        if len(tree.children) == 1:
            return 'return {}'.format(tree.children[0])
        elif len(tree.children) == 0:
            return 'return'
            return ''
        else:
            assert len(tree.children) in [0, 1], 'return statement can only have 0 or 1 return value'

    def __gen_funcparam(self, tree):
        self.__push_up(tree)
        var_name, tpe = tree.children
        ctx = TypeContext(suffix='')
        decl = tpe.to_str(var_name, context=ctx)
        # if self.get_current_proc_name() is None:
        #     # we only check variables that are across processes, otherwise, there is no point in having non-blocking
        #     # assignment
        #     self.insert_current_module_var_type(var_name, tpe)
        return decl, var_name, None

    def funcparami(self, tree):
        return self.__gen_funcparam(tree)

    def funcparamio(self, tree):
        return self.__gen_funcparam(tree)

    def vardeclinit(self, tree):
        self.__push_up(tree)
        init_val = None
        tpe = None
        if len(tree.children) == 2:
            var_name, tpe = tree.children
        elif len(tree.children) == 3:
            var_name, tpe, init_val = tree.children
        else:
            assert False, 'children size of vardeclinit is not 2 or 3, there might be a bug in the translator'
        ctx = TypeContext(suffix='')
        decl = tpe.to_str(var_name, context=ctx)
        if self.get_current_proc_name() is None:
            # we only check variables that are across processes, otherwise, there is no point in having non-blocking
            # assignment
            self.insert_current_module_var_type(var_name, tpe)
        return (decl, var_name, init_val)

    def hbindingarrayref(self, tree):
        """
        this expansion should only be invoked by expanding_binding_ref and should not be invoked elsewhere
        the reason is that we need to collect binding information per arry-like port
        """
        self.__push_up(tree)
        return '{}[{}]'.format(tree.children[0], tree.children[1])

    def expand_binding_ref(self, tree):
        if not is_tree_type(tree, 'hbindingarrayref'):
            raise ValueError('expand_binding_ref only accepts hbindingarrayref')
        self.__push_back(tree)
        return '{}[{}]'.format(tree.children[0], tree.children[1])

    def moduleinst(self, tree):
        # dprint(tree)
        mod_name, mod_type = tree.children
        # expand if it is an element of module array
        mod_name = '_'.join(mod_name.split('#'))
        if len(mod_type.children[0].children) > 1:
            warnings.warn('Type parameters for modules are not supported')
        mod_type_name = mod_type.children[0].children[0]
        if mod_name not in self.bindings:
            warnings.warn('Port bindings for module instance name {} not found'.format(mod_name))
            bindings = []
        else:
            bindings = self.bindings[mod_name]
        def extract_binding_name(x):
            # FIXME: when the port connection is 2D, the original approach may not work
            return get_ids_in_tree(x[0])[0]
            # if is_tree_type(x[0], 'hbindingarrayref'):
            #     res = x[0].children[0].children[0]
            # else:
            #     res = x[0].children[0]
            # return res
        orig_bindings = bindings
        bindings_normal = list(filter(lambda x: '.' not in extract_binding_name(x), orig_bindings))
        bindings_hier = list(filter(lambda x: '.' in extract_binding_name(x), orig_bindings))
        bindings = bindings_normal
        ind = self.get_current_ind_prefix()
        res = ind + '{} {}('.format(mod_type_name, mod_name) + '\n'
        self.inc_indent()
        ind = self.get_current_ind_prefix()
        binding_str = []
        array_bindings = {}
        for binding in bindings:
            # for backward compatibility, we keep the case where binding is a list
            if type(binding) == list:
                sub, par = binding
            else:
                warnings.warn('Using Tree as binding is deprecated', DeprecationWarning)
                sub, par = binding.children
            if is_tree_type(sub, 'hbindingarrayref'):
                # The .xxx part is an array
                sub_name = get_ids_in_tree(sub)[0].value  # assuming varref
                if sub_name not in array_bindings:
                    array_bindings[sub_name] = {}
                # if sub.children[0].data == 'hbindingarrayref':
                #     raise ValueError('nested 2-D array port is not supported')
                array_bindings[sub_name][sub.children[1].children[0]] = par
            else:
                # at this point, the par should be able to be fully expanded even if it is an array
                if is_tree_type(par, 'hbindingarrayref'):
                    par = self.expand_binding_ref(par)
                else:
                    par = par.children[0].value
                binding_str.append(ind + '.{}({})'.format(sub.children[0].value, par))
        for sub_name, bindings in array_bindings.items():
            # for now, we keep a dict of array binding
            array_seq = [None] * len(bindings)
            for idx, b in bindings.items():
                # dprint(self.expand_binding_ref(b))
                # array_seq[idx] = '{}[{}]'.format(b.children[0].children[0].value, b.children[1].children[0])
                array_seq[idx] = self.expand_binding_ref(b)
            binding_str.append(ind + ".{}('{{ {} }})".format(
                sub_name, ','.join(array_seq)
            ))
        res += ',\n'.join(binding_str)
        res += '\n'
        self.dec_indent()
        ind = self.get_current_ind_prefix()
        res += ind + ');'
        res += '\n'
        res += ind + "always @(*) begin\n"
        # res += ind + "always_comb begin\n"
        self.inc_indent()
        ind = self.get_current_ind_prefix()
        for bl, br in bindings_hier:
            res += ind + '{} = {};\n'.format(bl.children[0], br.children[0])
        self.dec_indent()
        ind = self.get_current_ind_prefix()
        res += ind + "end\n"
        # add an always block for port binding when we encounter sub module case
        tree.children = [res]
        return tree

    def hlrotate(self, tree):
        self.__push_up(tree)
        val, rot = tree.children
        return '({} << {}) | ($unsigned({}) >> ($bits({}) - {}))'.format(val, rot, val, val, rot)
    def horreduce(self, tree):
        self.__push_up(tree)
        val = tree.children[0]
        return '(|{})'.format(val)

    def hconcat(self, tree):
        self.__push_up(tree)
        l, r = tree.children[0], tree.children[1]
        res = '{{ {}, {} }}'.format(l, r)
        return res

    def vardecl(self, tree):
        self.__push_up(tree)
        return tree.children

    def prevardecl(self, tree):
        self.__push_up(tree)
        new_children = []
        for t in tree.children:
            new_children.extend(t)
        tree.children = new_children
        return tree

    def htypeinfo(self, tree):
        # Note: htypeinfo should return an object that can be used to apply to a variable
        self.__push_up(tree)
        return tree.children[0]

    def func_param_name_stub(self, tree):
        self.__push_up(tree)
        tree.insert_name(self.module_var_type.keys())
        var_name, tpe = tree.children
        ctx = TypeContext(suffix='')
        decl = tpe.to_str(var_name, context=ctx)
        return decl, var_name, None

    def hfunctionparams(self, tree):
        self.__push_up(tree)
        return tree

    def hfunctionrettype(self, tree):
        self.__push_up(tree)
        if len(tree.children) != 0:
            tpe = tree.children[0]
            res = tpe.to_str(var_name='')
        else:
            res = "void"
        return res

    def htouint(self, tree):
        self.__push_up(tree)
        return '$unsigned({})'.format(tree.children[0])

    def htoint(self, tree):
        self.__push_up(tree)
        return '$signed({})'.format(tree.children[0])

    def htoulong(self, tree):
        self.__push_up(tree)
        return '$unsigned({})'.format(tree.children[0])

    def htolong(self, tree):
        self.__push_up(tree)
        return '$signed({})'.format(tree.children[0])

    def hfunctionlocalvars(self, tree):
        self.__push_up(tree)
        return tree

    def hfunction(self, tree):
        self.set_current_proc_name('#function#')
        self.inc_indent()
        self.__push_up(tree)
        self.dec_indent()
        ind = self.get_current_ind_prefix()

        # function_name, return_type, params, localvar, body = tree.children
        function_name, return_type = tree.children[:2]
        params = ''
        for ch in tree.children[2:]:
            if is_tree_type(ch, 'hfunctionparams'):
                params = ', '.join(map(lambda x: x[0], ch.children))
            elif is_tree_type(ch, 'hfunctionlocalvars'):
                localvar = ch
            elif is_tree_type(ch, "hfunctionbody"):
                body = ch


        self.inc_indent()
        ind = self.get_current_ind_prefix()
        localvars = '\n'.join(map(lambda x: ind + x[0] + ';', localvar.children))
        self.dec_indent()
        body = '\n'.join(body.children)

        ind = self.get_current_ind_prefix()
        res = '{}function automatic {} {} ({});\n{}begin\n{}\n{}\n{}end\n{}endfunction'.format(ind, return_type, function_name,
                                                                                  params, ind,
                                                                                  localvars, body, ind, ind)
        self.reset_current_proc_name()
        return res

    def collect_type(self, port_or_sig):
        assert isinstance(port_or_sig, Tree) and \
               port_or_sig.data in ['sigdecltype', 'portdecltype'], \
               'collect_type must be invoked with portdecltype or sigdecltype, but got: {}'.format(port_or_sig)
        id = port_or_sig.children[0].children[0]
        tpe = port_or_sig.children[1]
        self.insert_current_module_var_type(id, tpe)

    def sigdecltype(self, tree):
        self.__push_up(tree)
        self.collect_type(tree)
        return tree

    def portdecltype(self, tree):
        self.__push_up(tree)
        self.collect_type(tree)
        return tree

    def modportsiglist(self, tree):
        self.__push_up(tree)
        return tree

    def get_current_module_var_type(self, id):
        return self.module_var_type[id]

    def get_current_module_var_type_or_default(self, id, default=None):
        if id not in self.module_var_type:
            return default
        return self.module_var_type[id]

    def reset_current_module_var_type(self):
        self.module_var_type = dict()

    def insert_current_module_var_type(self, id, tpe):
        if id in self.module_var_type:
            raise ValueError('Duplicate signal declaration: {}'.format(id))
        self.module_var_type[id] = tpe

    def hthread(self, tree):
        # hthread concists of 4 parts as children
        # 1. thread name
        # 2. signals declaration across hmethods
        # 3. synchronous hmethod for setting state to next_state and reset
        # 4. combinational hmethod for driving next_state
        # We need 2 to be at the module level and thus its processing will be handled in hmodule
        self.is_in_thread = True
        self.thread_name = tree.children[0]
        self.__push_up(tree)
        del self.thread_name
        self.is_in_thread = False
        return tree

    def __generate_hthread_block(self, tree, is_sync):
        # currently we assume that the hthread block is simply and there is no sensitivity list and
        # var decls, this might change in the future when we add support for resets
        proc_name, *body = tree.children
        if is_sync:
            self.set_current_proc_name('#thread_sync#')
        else:
            self.set_current_proc_name(proc_name)

        ind = self.get_current_ind_prefix()

        thread_name = self.thread_name
        sense_list = self.get_sense_list()
        assert thread_name in sense_list, "Process name {} is not in module {}".format(proc_name, self.current_module)
        if is_sync:
            res = ind + 'always @({}) begin: {}\n'.format(' or '.join(self.get_sense_list()[thread_name]), proc_name)
        else:
            res = ind + 'always @(*) begin: {}\n'.format(proc_name)
        self.inc_indent()
        self.__push_up(tree)
        proc_name, *body = tree.children
        ind = self.get_current_ind_prefix()
        res += '\n'.join(body) + '\n'
        self.dec_indent()
        ind = self.get_current_ind_prefix()
        res += ind + 'end'
        self.__reset_local_variables()
        self.reset_current_proc_name()
        return res

    def hthreadsync(self, tree):
        res = self.__generate_hthread_block(tree, is_sync=True)
        return res

    def hthreadswitch(self, tree):
        self.thread_comb = True
        res = self.__generate_hthread_block(tree, is_sync=False)
        self.thread_comb = False
        return res

    def hmodule(self, tree):
        # dprint("Processing Module: ", tree.children[0])
        # print("Retrieving Portbindings")
        self.current_module = tree.children[0]
        self.senselist = {}
        self.reset_current_module_var_type()
        initialization_block = []
        encountered_initblock = False
        self.bindings = dict()
        for t in tree.children:
            if isinstance(t, Tree) and t.data == 'portbindinglist':
                self.bindings[t.children[0]] = t.children[1]
            elif is_tree_type(t, 'hmodinitblock'):  # currently we only have one block
                if encountered_initblock:
                    raise ValueError('Only one hmodinitblock should present')
                encountered_initblock = True
                name = t.children[0]
                initblock, portbindings, senslist = None, None, []
                for ch in t.children[1:]:
                    if ch.data == 'hcstmt':  # TODO: have a dedicated node for initial block
                        initblock = ch
                    elif ch.data == 'portbindinglist':
                        portbindings = ch
                    elif ch.data == 'hsenslist':
                        senslist.append(ch)
                    else:
                        raise ValueError(ch.pretty())
                if initblock:
                    self.inc_indent()
                    self.inc_indent()
                    self.set_current_proc_name('#initblock#')
                    self.__push_up(initblock)
                    self.reset_current_proc_name()
                    self.dec_indent()
                    self.dec_indent()
                if portbindings:
                    for bds in portbindings.children[1]:
                        mod_name = bds.children[0]
                        bindings = bds.children[1:]
                        if mod_name not in self.bindings:
                            self.bindings[mod_name] = []
                        self.bindings[mod_name].append(bindings)
                # has something within the initialization block
                if initblock and initblock.children:
                    initialization_block.append(initblock.children[0])
        tree.children = list(filter(lambda x: not isinstance(x, Tree) or x.data != 'portbindinglist', tree.children))
        self.inc_indent()
        self.__push_up(tree)
        self.dec_indent()

        module_name = tree.children[0]
        modportsiglist = None
        processlist = None
        functionlist = []
        vars = None
        mods = []
        for t in tree.children:
            if isinstance(t, Tree):
                if t.data == 'modportsiglist':
                    modportsiglist = t
                elif t.data == 'processlist':
                    processlist = t
                elif t.data =='hfunction':
                    functionlist.append(t)

        # module_name, modportsiglist, processlist, portbindinglist = tree.children
        if modportsiglist:
            ports = list(filter(lambda x: isinstance(x, Tree) and x.data == 'portdecltype', modportsiglist.children))
            sigs = list(filter(lambda x: isinstance(x, Tree) and x.data == 'sigdecltype', modportsiglist.children))
            vars = list(filter(lambda x: isinstance(x, tuple), modportsiglist.children))
            mods = list(filter(lambda x: isinstance(x, Tree) and x.data == 'moduleinst', modportsiglist.children))
        else:
            ports, sigs = None, None


        res = 'module {} ('.format(module_name) + '\n'
        # Generate ports
        if ports:
            self.inc_indent()
            ind = self.get_current_ind_prefix()
            for idx, p in enumerate(ports):
                name, tpe = p.children
                name = name.children[0].value
                type_context = None
                if idx == len(ports) - 1:
                    type_context = TypeContext(suffix='')
                res += ind + tpe.to_str(name, type_context) + '\n'
            self.dec_indent()
        res += ');\n'
        # Generate signals
        if sigs:
            self.inc_indent()
            ind = self.get_current_ind_prefix()
            for idx, p in enumerate(sigs):
                name, tpe = p.children
                name = name.children[0].value
                type_context = None
                res += ind + tpe.to_str(name, type_context) + '\n'
            self.dec_indent()
        # generate vars (including modules)
        if vars:
            self.inc_indent()
            ind = self.get_current_ind_prefix()
            res = self.__generate_vars_decl(ind, res, vars)
            self.dec_indent()
        # generate initialization block
        if initialization_block:
            self.inc_indent()
            ind = self.get_current_ind_prefix()
            res += '{}initial begin\n'.format(ind)
            res += '\n'.join(initialization_block)
            res += '\n{}end\n'.format(ind)
            self.dec_indent()
        # generate module instantiations
        if len(mods) > 0:
            for m in mods:
                res += m.children[0] + '\n'
        # Generate processes
        if processlist:
            for proc in processlist.children:
                if is_tree_type(proc, 'hthread'):
                    # thread_name, thread_sig, thread_sync, thread_comb = proc.children
                    thread_func = None
                    if len(proc.children) == 3:
                        thread_name,  thread_sync, thread_comb = proc.children
                    elif len(proc.children) == 4:
                        thread_name, thread_func, thread_sync, thread_comb = proc.children
                    else:
                        assert False, "thread should have 3 or 4 children node"
                    self.inc_indent()
                    ind = self.get_current_ind_prefix()
                    res += '{}// Thread: {}\n'.format(ind, thread_name)
                    # res = self.__generate_vars_decl(ind, res, thread_sig.children)
                    self.dec_indent()
                    res += thread_sync + "\n"
                    if thread_func:
                        res += thread_func + "\n"
                    res += thread_comb + "\n"
                else:
                    res += proc + '\n'

        if functionlist:
            # for f in functionlist:
            #     res += f + '\n'
            assert False, "functionlist should be empty, there may be a bug in the code"
        res += "endmodule"
        return res

    def hbuiltin(self, tree):
        self.__push_up(tree)
        return tree.children[0]

    def hscmax(self, tree):
        assert len(tree.children) == 2, "sc_max node should only have 2 children"
        self.__push_up(tree)
        L = tree.children[0]
        R = tree.children[1]
        return "(({}) < ({}) ? ({}) : ({}))".format(L, R, R, L)

    def hscmin(self, tree):
        assert len(tree.children) == 2, "sc_min node should only have 2 children"
        self.__push_up(tree)
        L = tree.children[0]
        R = tree.children[1]
        return "(({}) < ({}) ? ({}) : ({}))".format(L, R, L, R)

    def __is_generated_signal(self, name):
        return name.endswith('#')

    def __generate_vars_decl(self, ind, res, vars):
        for decl, name, init in vars:
            if self.__is_generated_signal(name):
                # decl = '(* mark_debug = "true" *) ' + decl.replace('#', "")
                decl = decl.replace('#', "")
            if init:
                decl = decl + ' = ' + str(init) + ';'
            else:
                decl += ';'
            res += ind + decl + '\n'
        return res
