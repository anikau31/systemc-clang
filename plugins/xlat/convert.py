from lark import Lark, Transformer, Visitor
import sys
import logging
import warnings
logging.basicConfig(level=logging.DEBUG)

l = Lark('''
        start: modulelist

        modulelist: ( hmodule)*
        hmodule:  "hModule" ID "[" modportsiglist? processlist* "]"

        ?modportsiglist: "hPortsigvarlist" "NONAME" "[" modportsigdecl+ "]" 

        ?modportsigdecl: portdecltype 
                      | sigdecltype 
                      | vardeclinit 
        portdecltype: portdecl "[" htypeinfo "]"
        sigdecltype: sigdecl "[" htypeinfo "]"
        sigdecl:  "hSigdecl" ID  
        ?portdecl: inportdecl | outportdecl
        inportdecl:  "hPortin" ID 
        outportdecl: "hPortout" ID
        vardeclinit: "hVardecl" ID "[" htypeinfo hvarinit? "]"
        ?hvarinit: "hVarInit" "NONAME" expression
        // can be no process at all in the module
        ?processlist:  "hProcesses" "NONAME" "[" hprocess* "]"
        // could be nothing
        hprocess:  "hProcess" ID  "[" hsenslist*   stmt "]"

        // can be just an empty statement
        hcstmt:  "hCStmt" "NONAME" "[" modportsiglist* stmt+ "]" // useful for raising variable decls
              |  "hCStmt" "NONAME" "NOLIST"

        stmt : expression
             | syscwrite
             | ifstmt
             | forstmt
             | hcstmt

        // for(forinit; forcond; forpostcond) stmts
        forstmt: "hForStmt" "NONAME" "[" forinit forcond forpostcond forbody "]"
        forinit: "hPortsigvarlist" "NONAME" "[" vardeclinit  "]"
                 | vardeclinit
        forcond: expression
        forpostcond: expression
        forbody: stmt

        hsenslist : "hSenslist" "NONAME" "[" hsensvars "]"
                  | "hSenslist" "NONAME" "NOLIST"
        hsensvar :  "hSensvar" ID "[" hsensedge "]"
        hsensvars : hsensvar*

        hsensedge : "hSensedge" npa "NOLIST"
        !npa : "neg" | "pos" | "always"

        // if and if-else, not handling if-elseif case
        ifstmt: "hIfStmt" "NONAME" "[" expression  stmt stmt?"]"

        exprinif: expression
         
        ?expression: hbinop
                  | hunop
                  | hliteral
                  | vardeclinit
                  | hvarref
                  | hunimp
                  | syscread
                  | hmethodcall
                  |  "[" expression "]"

        syscread : hsigassignr "[" expression "]"
        syscwrite : hsigassignl "["  expression  expression "]"
        ?hsigassignr :  "hSigAssignR" "read" 
        ?hsigassignl :  "hSigAssignL" "write" 
        // function call
        hvarref : "hVarref" ID "NOLIST"
        hunimp:  "hUnimpl" ID "NOLIST"
        hbinop:  "hBinop" BINOP "[" expression expression "]"
        hunop:  "hUnop" UNOP "[" expression "]"
        hmethodcall: "hMethodCall" hidorstr  "[" expression expression* "]" 
        ?hidorstr: ID | STRING
        hliteral:  "hLiteral" ID "NOLIST"
        htypeinfo: "hTypeinfo" "NONAME" "[" htype+ "]"
        htype:  "hType" STRING "NOLIST" 
        ID: /[a-zA-Z_0-9]+/
        BINOP: "==" | "&&" | "=" | "||" | "-" | ">" | "+" | "*" | "^" | "ARRAYSUBSCRIPT" | "<=" | "<" | "%"
        UNOP: "!" | "++" | "-"
        %import common.WS
        %ignore WS
        %import common.ESCAPED_STRING -> STRING
        ''', parser='lalr', debug=True)

class CType2VerilogType(object):
    type_map = { '_Bool': '[0:0]',
                 'sc_in': 'input wire',
                 'sc_out': 'output reg',
                 'sc_signal': 'reg',
                 'int': 'integer'}

    @staticmethod
    def varref_wrapper(t, v):
        if t == 'double':
            return '$bitstoreal(v)'
        else:
            return v

    @staticmethod
    def convert_type_list(l):
        """converts a list of htype info into proper type, also annotate for intended types for necessary conversion"""
        res = []
        in_port = 'sc_in' in l
        out_port = 'sc_out' in l
        # we almost always use reg, reg also for implicit values
        wire_or_reg = 'reg'
        if in_port:
            wire_or_reg = 'wire'
        bw = None
        vtype = None
        l_iter = iter(l)
        for arg in l_iter:
            if arg == 'sc_in':
                in_port = True
                wire_or_reg = 'wire'
            elif arg == 'sc_out':
                out_port = True
            else: 
                if arg in ['sc_bv', 'sc_int']:
                    if bw is None:
                        bw = int(next(l_iter))
                    else:
                        assert False, 'incorrect htype specification'
                elif arg in ['double', 'int']:
                    # bw = CType2VerilogType.get_width(arg)
                    if arg == 'double':
                        vtype = 'real'
                    elif arg == 'int':
                        vtype = 'integer'
        inout = ''
        if in_port:
            inout = 'input'
        elif out_port:
            inout = 'output'
        if bw is None:
            width_or_type_str = ''
            if vtype == 'real':
                width_or_type_str = ' real'
            elif vtype == 'integer':
                width_or_type_str = ' [31:0]'
        else:
            width_or_type_str = f' [{bw-1}:0]'
        return [f'{inout} {wire_or_reg}{width_or_type_str}', vtype]

    @staticmethod
    def get_width(t):
        if t == 'double':
            return 64
        elif t == 'int':
            return 32
        else:
            assert False


debug = True
def p(decorated):
    """a decorator that helps printing out the transformation results"""
    if debug:
        def wrapper(self, args):
            print(f'{decorated.__name__}: \n{args} \n\n')
            res = decorated(self, args)
            return res
        return wrapper
    else:
        return decorated


def flatten(L):
    """a simple flatten helper"""
    for item in L:
        try:
            if isinstance(item, str) or isinstance(item, tuple):
                yield item
            else:
                yield from flatten(item)
        except TypeError:
            yield item



class VerilogTransformer(Transformer):

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.vardecl_map = dict()
        self.global_type_map = dict()

    def start(self, args):
        return args[0]

    @p
    def htypeinfo(self, args):
        """resolving type information"""

        # drop sc_signal for types without width
        # if args[0] == 'reg' and args[1] != 'sc_bv':
        #     args = args[1:]

        res = CType2VerilogType.convert_type_list(args)
        return res

    def hliteral(self, args):
        return str(args[0])

    @p
    def hlitdecl(self, args):
        return args

    @p
    def hcstmt(self, args):
        stmt_list = []
        for idx, stmt in enumerate(args):
            # if idx == 0:
            #     if len(stmt) > 0:
            #         print(stmt)
            #         annotated_stmt = map(lambda x: 'reg ' + x + ';', stmt)
            #         stmt_list.extend(list(annotated_stmt))
            if isinstance(stmt, list):
                stmt_list.extend(stmt)
            else:
                if stmt:
                    stmt_list.append(stmt)
        # currently it's ok to append a comma
        print("stmtlist in hcstmt is ", args, stmt_list)
        res = ';\n'.join(x for x in stmt_list) + ';'
        return res

    def exprwrapper(self, args):
        return args[0] + ';'

    def hvardefwrapper(self, args):
        return args[0] + ';'

    def hbinop(self, args):
        op = str(args[0])
        if op == '=':
            return f'{args[1]} {args[0]} {args[2]}'
        elif op == 'ARRAYSUBSCRIPT':
            return f'{args[1]} [ {args[2]} ]'
        else:
            return f'({args[1]}) {args[0]} ({args[2]})'

    def hunop(self, args):
        if len(args) == 1:
            return f'{args[0]}'
        elif len(args) == 2:
            op = str(args[0])
            if op == '++':
                # only used in the for loop
                return f'{args[1]}={args[1]}+1'
            elif op == '--':
                # only used in the for loop
                return f'{args[1]}={args[1]}-1'
            else:
                return f'{args[0]}({args[1]})'
    @p
    def hmethodcall(self, args):
        print("in hmethodcall, returning ", f'{args[0]}',"(", f'{",".join(args[1:])}', ")")
        return (f'{args[0]}(' f'{",".join(args[1:])})')

    def stmts(self, args):
        return args

    @p
    def vardeclonly(self, args):
        assert(isinstance(args[1], list))
        assert(len(args[1]) == 2)
        self.vardecl_map[str(args[0])] = args[1]
        return None

    @p
    def hvardef(self, args):
        # Init without a type
        if len(args) == 3:
            # by default, use reg
            self.vardecl_map[str(args[0])] = args[1]
            return f'{args[0]} = {args[2]}'
        assert False
    @p
    def vardeclinit(self, args):
        print("vardeclinit: ", args)
        self.vardecl_map[str(args[0])] = args[1]
        if len(args)==3:
            return f'{args[0]} = {args[2]}'
        return None
    
    @p
    def htype(self, args):
        # NOTE: the name of htype will always be a STRING
        # NOTE: args[0][1:-1] removes the quotes
        # return CType2VerilogType.convert(str(args[0]))
        return str(args[0][1:-1])

    def hunimp(self, args):
        return f'\"//# Unimplemented: {args[0]}\"'

    def ifstmt(self, args):
        if len(args) == 2:  # if
            return (f"if({args[0]}) begin\n"
            f"{args[1]}\n"
            f"end"
            )
        elif len(args) == 3: # if-else
            return (f"if({args[0]}) begin\n"
            f"{args[1]}\n"
            f"end else begin\n"
            f"{args[2]}\n"
            f"end"
            )

    def ifcond(self, args):
        if str(args[0]) == '<null child>':
            return f'\"NULL CHILD\"'
        return f'{args[0]}'

    @p
    def hprocess(self, args):
        vardecl_str = '\n'.join(map(lambda x: f'{x[1][0]} {x[0]};', self.vardecl_map.items()))
        senslist = '*' if len(args[1]) == 0 else args[1]
        res = (f"always @({senslist}) begin: {args[0]}\n"
               f"{vardecl_str}\n"
               f"{args[2]}\n"
               f"end // {args[0]}")
        self.vardecl_map.clear()
        return res

    @p
    def portsiglist(self, args):
        return args

    def processlist(self, args):
        return '\n\n'.join(args)

    def inportdecl(self, args):
        return str(args[0])

    @p
    def portdecltype(self, args):
        self.global_type_map[args[0]] = args[1][1]
        return ('port', f'{args[1][0]} {args[0]}')

    def outportdecl(self, args):
        return str(args[0])

    @p
    def sigdecltype(self, args):
        self.global_type_map[args[0]] = args[1][1]
        return ('sig', f'{args[1][0]} {args[0]}')
    
    def sigdecl(self, args):
        return str(args[0])

    @p
    def hmodule(self, args):
        print("args: ", args)
        modname = str(args[0])
        print(modname)
        if modname.startswith('sc_stream_0'):
            warnings.warn('TODO: temporary fix, manually skipping sc_stream_0')
            return ''
        if len(args)>1:
            portsiglist = args[1]
            print("portsiglist ", portsiglist)
            if (len(args)>2):
                proclist = args[2]
            else:
                proclist = None
        else:
            portsiglist = None

        # separate port from list of port, sig
        if portsiglist:
            portlist = list(map(lambda x: x[1], filter(lambda x: x is not None and  x[0] == 'port', portsiglist[0])))
            portstr = ',\n'.join(portlist)
            
            siglist = list(map(lambda x: x[1], filter(lambda x: x is not None and  x[0] == 'sig', portsiglist[0])))
            sigstr = ';\n'.join(siglist) + (';' if len(siglist) > 0 else '')  # the last semi-colon
            
            varlist = portsiglist[1]# list(map(lambda x: x[1], filter(lambda x: x[0] == 'var', portsiglist[1])))
            varstr = ';\n'.join(varlist) + (';' if len(varlist) > 0 else '')

        # for the processes
        procstr = proclist

        return (f"module {modname}(\n"
                f"{portstr} \n"
                f");\n"
                f"{varstr}\n"
                f"{sigstr}\n"
                f"{proclist}\n"
                f"endmodule // {modname}"
                )
    def modulelist(self, args):
        return "\n".join(args)

    def exprinif(self, args):
        return str(args[0]) + ';'

    @p
    def hsensvar(self, args):
        if args[1] == 'pos':
            return f'posedge {args[0]}'
        elif args[1] == 'neg':
            return f'negedge {args[0]}'
        elif args[1] == 'always':
            return args[0]
        else:
            raise RuntimError('edge sensitivity should be one of pos/neg/always')

    @p
    def hsenslist(self, args):
        assert len(args) <= 1
        if len(args) == 0:
            return '*'
        else:
            return args[0]

    def hsigassignl(self, args):
        return None
    def hsigassignr(self, args):
        return None
    def syscwrite(self, args):
        return f'{args[1]} <= {args[2]}'
    def syscread(self, args):
        return f'{args[1]}'

    @p
    def vardecltype(self, args):
        assert(len(args) == 1)
        # return ('var', f'{args[1]} {args[0]}')
        # return ('var', f'{args[0]}')
        return args[0]

    def htempandunop(self, args):
        return f'&{args[0]}'

    def htemptrigger(self, args):
        return '//#(' + ','.join(args) + ');'

    @p
    def hsensvars(self, args):
        return ' or '.join(args)

    def fcall(self, args):
        fname = str(args[0])
        caller = str(args[1])
        if fname == 'to_int':
            return caller
        else:
            raise NotImplementedError
        
    @p
    def npa(self, args):
        return str(args[0])

    @p
    def hsensedge(self, args):
        return args[0]

    def hvarref(self, args):
        if str(args[0]) in self.vardecl_map:
            return CType2VerilogType.varref_wrapper(
                self.vardecl_map[args[0]][1], 
                str(args[0])
            )
        else:
            return str(args[0]) + '/* definition not found */'

    @p
    def hnoop(self, args):
        if len(args) == 2:
            return str(args[1])
        elif len(args) == 1:
            return str(args[0])
        elif len(args) == 3 and args[0] == 'next_trigger':
            return '#' + str(args[1])
        else:
            assert False

    @p
    def modportsiglist(self, args):
        args = list(flatten(args))
        var_list = []
        for k, v in self.vardecl_map.items():
            var_list.append(f'{v[0]} {k}')
            # move these signals to the global scope
            self.global_type_map[k] = v[1]
        self.vardecl_map.clear()
        print(var_list)
        return [args, var_list]

    @p
    def forinit(self, args):
        # note: var decl is raised up to the top
        return args[0]

    @p
    def forcond(self, args):
        return args[0]

    @p
    def forpostcond(self, args):
        # note: the args might be wrong
        return args[0]

    @p
    def forbody(self, args):
        return args

    @p
    def forstmt(self, args):
        forinit, forcond, forpostcond, forbody = args
        forbody = ';\n  '.join(forbody) + ';'
        res = [
                f'for({forinit};{forcond};{forpostcond}) begin',
                f'  {forbody}',
                f'end'
            ]
        return '\n'.join(res)

    @p
    def stmt(self, args):
        assert(len(args) == 1)
        return args[0]

def main():
    if len(sys.argv) != 2 and len(sys.argv) != 3:
        print('Usage: convert.py filename [outputfilename]')
    filename = sys.argv[1]
    outputname = filename + ".v"
    if len(sys.argv) == 3:
        outputname = sys.argv[2]
    with open(filename, 'r') as f:
        file_content = f.read()
    t = l.parse(file_content)
    # print(t)
    res = VerilogTransformer().transform(t)
    with open(outputname, 'w+') as f:
        f.writelines(res)
        f.write("\n\n")
    print(res)




if __name__ == '__main__':
    main()
    # l.parse('')
