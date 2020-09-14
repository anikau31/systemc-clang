from lark import Lark

lark_grammar = Lark('''
        start: modulelist typelist

        modulelist: (hmodule)*
        typelist: (htypedef)*
        hmodule:  "hModule" ID "[" modportsiglist? (portbindinglist|processlist)* "]"

        modportsiglist: "hPortsigvarlist" "NONAME" "[" modportsigdecl+ "]" 

        ?modportsigdecl: portdecltype 
                      | sigdecltype 
                      | vardeclinit 
                      | moddecl
        moddecl: "hModdecl" ID "[" htypeinfo "]"
        portdecltype: portdecl "[" htypeinfo "]"
        sigdecltype: sigdecl "[" htypeinfo "]"
        sigdecl:  "hSigdecl" ID  
        ?portdecl: inportdecl | outportdecl
        inportdecl:  "hPortin" ID 
        outportdecl: "hPortout" ID
        vardeclinit: "hVardecl" ID "[" htypeinfo (hvarinit | hvarinitint)? "]"
        ?hvarinit: "hVarInit" "NONAME" expression
        ?hvarinitint: "hVarInit" NUM "NOLIST"
        // can be no process at all in the module
        processlist:  "hProcesses" "NONAME" "[" hprocess* "]"
        // could be nothing
        // temporarily ignore the hMethod node
        hprocess:  "hProcess" ID  "[" hsenslist*   "hMethod" "NONAME" "[" prevardecl hcstmt "]" "]"
        prevardecl: vardecl*
        vardecl: vardeclinit

        // can be just an empty statement
        hcstmt:  "hCStmt" "NONAME" "[" modportsiglist* stmts "]" // useful for raising variable decls
              |  "hCStmt" "NONAME" "NOLIST"
        stmts: stmt+
        stmt : expression_in_stmt
             | syscwrite
             | ifstmt
             | forstmt
             | hcstmt
             | whilestmt
             | switchstmt
             | blkassign

        // Port Bindings
        portbindinglist: "hPortbindings" ID "[" portbinding* "]"
        portbinding: "hPortbinding" "NONAME" "[" hvarref hvarref "]"


        // This is solely for maintaining the semicolon
        expression_in_stmt: expression
        
        // while(condition) stmts
        whilestmt: "hWhileStmt" "NONAME" "[" expression stmt "]"

        // for(forinit; forcond; forpostcond) stmts
        forstmt: "hForStmt" "NONAME" "[" forinit forcond forpostcond forbody "]"
        forinit: "hPortsigvarlist" "NONAME" "[" vardeclinit  "]"
                 | vardeclinit
                 | "hNoop" "NONAME" "NOLIST"
                 | blkassign
        forcond: expression
        forpostcond: expression
        forbody: stmt

        switchstmt: "hSwitchStmt" "NONAME" "[" switchcond switchbody "]"
        switchcond: expression
        // Note: we don't make this a noraml statement as in the context of switch, 
        // we don't use general statements
        switchbody: "hCStmt" "NONAME" "[" casestmt+ "]"
        casestmt: "hSwitchCase" "NONAME" "[" casevalue stmt "]" "hNoop" "NONAME" "NOLIST"
                | "hSwitchCase" "NONAME" "[" casevalue "hNoop" "NONAME" "NOLIST" "]"
        casevalue: expression

        hsenslist : "hSenslist" "NONAME" "[" hsensvars "]"
                  | "hSenslist" "NONAME" "NOLIST"
        hsensvar :  "hSensvar" ID "[" (hsensedge|hsensvar)* "]"
                 |  "hSensvar" ID "NOLIST"
        hsensvars : hsensvar*

        hsensedge : "hSensedge" npa "NOLIST"
        !npa : "neg" | "pos" | "always"

        // if and if-else, not handling if-elseif case
        ifstmt: "hIfStmt" "NONAME" "[" expression  stmt stmt?"]"

         
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
        // A temporary hack to handle --
        hunop:  "hUnop" UNOP_NON_SUB "[" expression "]"
             |  "hUnop" UNOP_SUB "[" expression "]"
             |  "hUnop" "-" "-" "[" expression "]" // hack to work with --

        // Separate '=' out from so that it is not an expression but a standalone statement
        blkassign: "hBinop" "=" "[" (hvarref | hliteral) (hunop | hvarref | hliteral | harrayref | hnsbinop | hunimp | syscread) "]"
                 | "hBinop" "=" "[" harrayref (hvarref | hliteral | harrayref | hnsbinop | hunimp | syscread) "]"
                 | "hSigAssignL" "write" "[" (hliteral | hvarref | harrayref) (syscread | hliteral | harrayref | hunop | hvarref)  "]"
                 | "hSigAssignL" "write" "[" (hliteral | hvarref | harrayref) nonrefexp  "]"
                 | vassign
                 | hmodassign
                 
        // Temporary hack to handle -= / +=
        hmodassign : "hBinop" hmodassigntype "[" hvarref hliteral "]"
        ?hmodassigntype : haddassign | hsubassign
        haddassign : "+" "=" 
        hsubassign : "-" "="
        vassign: "hVarAssign" "NONAME" "[" hvarref (hnsbinop | syscread | hliteral)"]"
        // Normal expressions that can not be expanded
        nonrefexp: hbinop
        
        harrayref: "hBinop" "ARRAYSUBSCRIPT"  "[" (hliteral | hvarref | syscread) expression  "]"
                 | hslice
        hslice: "hBinop" "SLICE" "[" hvarref expression expression "]"
        hnsbinop:  "hBinop" NONSUBBINOP "[" expression expression "]"

        hmethodcall: "hMethodCall" hidorstr  "[" expression expression* "]" 
        ?hidorstr: ID | STRING
        hliteral:  idlit | numlit | numlitwidth
        idlit : "hLiteral" ID "NOLIST"
        numlit : "hLiteral" NUM "NOLIST"
        numlitwidth : "hLiteral" NUM "[" htypeinfo "]"
        htypeinfo: "hTypeinfo" "NONAME" "[" htype "]"
                 | "hTypeinfo" "NONAME" "NOLIST" // ?
        htype: htypearray
             | "hType" TYPESTR "NOLIST" 
             | "hType" TYPESTR "[" (htype|htypeint)+ "]"     // nested types, type parameters
        htypearray : "hType" "array" arraydimlength "[" (htype|htypeint)+ "]"
        arraydimlength: "##" NUM 
                      | "##" NUM arraydimlength
        htypeint: "hLiteral" NUM "NOLIST"  // integer type parameters
        htypedef: "hTypedef" TYPESTR "[" htypetemplateparams htypefields "]"
        
        
        htypetemplateparams: htypetemplateparam*
        htypetemplateparam: "hTypeTemplateParam" TYPESTR "NOLIST"
        
        htypefields: htypefield*
        htypefield: "hTypeField" ID "[" htype "]"

        ID: /[a-zA-Z_][a-zA-Z_0-9#]*/
        NUM: /(\+|\-)?[0-9]+/
        TYPESTR: /[a-zA-Z_][a-zA-Z_0-9]*/
        BINOP: NONSUBBINOP | "ARRAYSUBSCRIPT" | "SLICE"
        NONSUBBINOP: "==" | "<<" | ">>" | "&&" | "||" | ">" | ARITHOP | "<=" | "<" | "%" | "!=" | "&" | ","
        ARITHOP: "+" | "-" | "*" | "/" | "^"
        UNOP_NON_SUB: "!" | "++" | "-"
        UNOP_SUB:  "-"
        UNOP_DEC:  "--"
        %import common.WS
        %ignore WS
        %import common.ESCAPED_STRING -> STRING
        ''', parser='lalr', debug=True, propagate_positions=True)

