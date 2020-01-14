from lark import Lark, Transformer, Visitor
import sys
import logging
logging.basicConfig(level=logging.DEBUG)

l = Lark('''
        start: modulelist

        modulelist: ("[" hmodule "]")*
        hmodule: "(" "hModule" ID ")" "[" portsiglist "]" "[" processlist "]"

        ?portsiglist: modportsigdecl*

        ?modportsigdecl: portdecltype
                      | sigdecltype
                      | vardecltype

        sigdecltype: sigdecl "[" htype htype "]"
                   | sigdecl "[" htype htype htype "]"
        sigdecl: "(" "hSigdecl" ID ")"
        portdecltype: portdecl "[" htype htype "]"
                    | portdecl "[" htype htype htype "]"
                    | portdecl "[" htype "]"
        ?portdecl: inportdecl | outportdecl
        inportdecl: "(" "hPortin" ID ")"
        outportdecl: "(" "hPortout" ID ")"
        vardecltype: hvardecl "[" htype "]" 

        // can be no process at all in the module
        processlist: ("[" hprocess "]") *
        // could be nothing
        hprocess: "(" "hProcess" ID ")" "[" hsensvars "]"  "[" hcstmt "]"

        // can be just an empty statement
        hcstmt: "(" "hCStmt" ")" hlitdecl stmts
              | "(" "hCStmt" ")" hlitdecl

        ?stmts: stmt+
        ?stmt : exprwrapper
             | ifstmtwrapper
             | hvardefwrapper
             | htemptrigger

        exprwrapper: "[" expression "]"
        ?ifstmtwrapper: "[" ifstmt "]"
        hvardefwrapper: "[" "[" hvardef "]" "]"

        hsensvar : "(" "hSensvar" ID ")"
        hsensvars : hsensvar*


        // if and if-else, not handling if-elseif case
        ifstmt: ifcond "[" (hcstmt|exprinif|ifstmt) "]"
              | ifcond "[" (hcstmt|exprinif|ifstmt) "]" "[" (hcstmt|exprinif|ifstmt) "]"
        exprinif: expression

        ifcond: "(" "hIfStmt" ")" expression
              | "(" "hIfStmt" ")" NC

        NC: "<null child>"


        ?expression: hbinop
                  | hunop
                  | hliteral
                  | hvardecl
                  | hunimp
                  | syscread
                  | syscwrite
                  | "[" expression "]"

        syscread : hsigassignr hliteral
        syscwrite : hsigassignl hliteral expression
        ?hsigassignr : "(" "hSigAssignR" "read" ")"
        ?hsigassignl : "(" "hSigAssignL" "write" ")"
        // function call
        fcall : "[" hliteral hvardecl "]"

        hunimp: "(" "hUnimpl" ID ")"
        hbinop: "(" "hBinop" BINOP ")" (expression|fcall) (expression|fcall)
        hunop: "(" "hUnop" UNOP ")" expression
             | htempandunop 
        // just a temporary workaround
        htempandunop: "(hLiteral operator const bool &)" expression
        htemptrigger: "[" hliteral hliteral hvardecl "]"
        hliteral: "(" "hLiteral" ID ")"
        hlitdecl: hliteral*
        ?hvardecl: "(" "hVardecl" ID ")"
        hvardef: hvardecl "[" "]" hvardefsuf // ok to shift instead of reduce
               | hvardecl "[" htype "]" hvardefsuf
               | hvardecl "[" htype htype "]" hvardefsuf
        ?hvardefsuf: hunimp | hliteral | "[" syscread "]" | "[" syscwrite "]" | "[" hbinop "]"
        htype: "(" "hType" ID ")"

        ID: /[a-zA-Z_0-9]+/
        BINOP: "==" | "&&" | "=" | "||" | "-" | ">" | "+" | "*" | "^"
        UNOP: "!"
        %import common.WS
        %ignore WS
        ''', parser='lalr', debug=True)

class CType2VerilogType(object):
    type_map = { '_Bool': '[0:0]',
                 'sc_in': 'input wire',
                 'sc_out': 'output reg',
                 'sc_signal': 'reg',
                 'sc_bv': 'reg',
                 'int': 'integer'}
    @staticmethod
    def convert(t):
        if t in CType2VerilogType.type_map:
            return CType2VerilogType.type_map[t]
        else:
            # not a built in type, could be user defined or other
            return t

class VerilogTransformer(Transformer):

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.vardecl_map = dict()

    def start(self, args):
        return args[0]

    def hliteral(self, args):
        return str(args[0])

    def hlitdecl(self, args):
        return args

    def hcstmt(self, args):
        stmt_list = []
        for idx, stmt in enumerate(args):
            if idx == 0:
                if len(stmt) > 0:
                    annotated_stmt = map(lambda x: 'reg ' + x + ';', stmt)
                    stmt_list.extend(list(annotated_stmt))
            elif isinstance(stmt, list):
                stmt_list.extend(stmt)
            else:
                stmt_list.append(stmt)
        # currently it's ok to append a comma
        res = '\n'.join(x for x in stmt_list)
        return res

    def exprwrapper(self, args):
        return args[0] + ';'

    def hvardefwrapper(self, args):
        return args[0] + ';'

    def hbinop(self, args):
        op = str(args[0])
        if op == '=':
            return f'{args[1]} {args[0]} {args[2]}'
        else:
            return f'({args[1]}) {args[0]} ({args[2]})'

    def hunop(self, args):
        if len(args) == 1:
            return f'{args[0]}'
        elif len(args) == 2:
            op = str(args[0])
            return f'{args[0]}({args[1]})'

    def stmts(self, args):
        return args

    def hvardef(self, args):
        # Init without a type
        if len(args) == 2:
            return f'{args[0]} = {args[1]}'
        elif len(args) == 3:
            # by default, use reg
            print(args)
            self.vardecl_map[str(args[0])] = args[1]
            return f'{args[0]} = {args[2]}'
        elif len(args) == 4:
            print(args)
            width = int(args[2]) - 1
            self.vardecl_map[str(args[0])] = f'{args[1]} [{width}:0]'
            return f'{args[0]} = {args[3]}'

    def htype(self, args):
        return CType2VerilogType.convert(str(args[0]))

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

    def hprocess(self, args):
        vardecl_str = '\n'.join(map(lambda x: f'{x[1]} {x[0]};', self.vardecl_map.items()))
        senslist = '*' if len(args[1]) == 0 else args[1]
        res = (f"always @({senslist}) begin: {args[0]}\n"
               f"{vardecl_str}\n"
               f"{args[2]}\n"
               f"end // {args[0]}")
        self.vardecl_map.clear()
        return res

    def portsiglist(self, args):
        return args

    def processlist(self, args):
        return '\n\n'.join(args)

    def inportdecl(self, args):
        return str(args[0])

    def portdecltype(self, args):
        if len(args) == 3:
            return ('port', f'{args[1]} {args[2]} {args[0]}')
        elif len(args) == 2:
            # Note: we remove the notation of signed vs unsigned for now
            return ('port', f'{args[1]} {args[0]}')
        else:
            assert False, 'impossible path, args: {}'.format(args)
            return ('port', f'{args[1]} [{width}:0] {args[0]}')

    def outportdecl(self, args):
        return str(args[0])

    def sigdecltype(self, args):
        if len(args) == 3:
            return ('sig', f'{args[1]} {args[2]} {args[0]}')
        else:
            width = int(args[3]) - 1
            # Note: we remove the notation of signed vs unsigned for now
            return ('sig', f'{args[1]} [{width}:0] {args[0]}')
    
    def sigdecl(self, args):
        return str(args[0])

    def hmodule(self, args):
        modname = str(args[0])
        portsiglist = args[1]
        proclist = args[2]

        # separate port from list of port, sig
        portlist = list(map(lambda x: x[1], filter(lambda x: x[0] == 'port', portsiglist)))
        portstr = ',\n'.join(portlist)

        siglist = list(map(lambda x: x[1], filter(lambda x: x[0] == 'sig', portsiglist)))
        sigstr = ';\n'.join(siglist) + (';' if len(siglist) > 0 else '')  # the last semi-colon

        varlist = list(map(lambda x: x[1], filter(lambda x: x[0] == 'var', portsiglist)))
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

    def hsensvar(self, args):
        return str(args[0])

    def hsigassignl(self, args):
        return None
    def hsigassignr(self, args):
        return None
    def syscwrite(self, args):
        return f'{args[1]} <= {args[2]}'
    def syscread(self, args):
        return f'{args[1]}'

    def vardecltype(self, args):
        return ('var', f'{args[1]} {args[0]}')

    def htempandunop(self, args):
        return f'&{args[0]}'

    def htemptrigger(self, args):
        return '//#(' + ','.join(args) + ');'

    def hsensvars(self, args):
        return ' or '.join(args)

    def fcall(self, args):
        fname = str(args[0])
        caller = str(args[1])
        if fname == 'to_int':
            return caller
        else:
            raise NotImplementedError

def main():
    if len(sys.argv) != 2:
        print('Usage: convert.py filename')
    filename = sys.argv[1]
    with open(filename, 'r') as f:
        file_content = f.read()
    t = l.parse(file_content)
    # print(t)
    res = VerilogTransformer().transform(t)
    with open(filename + '.v', 'w+') as f:
        f.writelines(res)
    print(res)


if __name__ == '__main__':
    main()
    # l.parse('')
