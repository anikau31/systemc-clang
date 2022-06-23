#ifndef _HDLBODY_H
#define _HDLBODY_H

// clang-format off
#include <vector>
#include <string>
#include <unordered_set>
#include <ctype.h>
#include "clang/AST/DeclCXX.h"
#include "clang/AST/Stmt.h"
#include "clang/AST/Expr.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/Diagnostic.h"

#include "llvm/ADT/StringRef.h"

// look at https://chromium.googlesource.com/chromium/src.git/+/62.0.3178.1/tools/clang/value_cleanup/ListValueRewriter.h#include "llvm/Support/FileSystem.h"

#include "SystemCClang.h"
#include "hNode.h"
#include "HDLType.h"

// clang-format on

using namespace clang;
using namespace systemc_clang;


using namespace hnode;

namespace systemc_hdl {
  typedef enum { rnomode, rmethod, rmodinit, rthread, ruserdefclass } HDLBodyMode;

  class HDLBody: public RecursiveASTVisitor <HDLBody> {
  public:
    HDLBody(clang::DiagnosticsEngine &diag_engine,
	    const ASTContext &ast_context,
	    hdecl_name_map_t &mod_vname_map,
	    hfunc_name_map_t &allmethodecls,
	    overridden_method_map_t &overridden_method_map);

    virtual ~HDLBody();

    void Run(Stmt *stmt, hNodep &h_top, HDLBodyMode runmode, HDLType *HDLt_userclassesp = NULL);
        
    bool TraverseCompoundStmt(CompoundStmt* compoundStmt);
    bool TraverseStmt(Stmt *stmt);
    bool TraverseDeclStmt(DeclStmt * declstmt);
    bool ProcessVarDecl(VarDecl * vardecl);
    bool TraverseBinaryOperator(BinaryOperator* expr);
    bool TraverseUnaryOperator(UnaryOperator* expr);
    bool TraverseConditionalOperator(ConditionalOperator * expr);
    bool TraverseIntegerLiteral(IntegerLiteral * lit);
    bool TraverseCXXBoolLiteralExpr(CXXBoolLiteralExpr * b);
    bool TraverseDeclRefExpr(DeclRefExpr* expr);
    bool TraverseArraySubscriptExpr(ArraySubscriptExpr* expr);
    bool TraverseCXXMemberCallExpr(CXXMemberCallExpr *callexpr);
    bool TraverseCXXOperatorCallExpr(CXXOperatorCallExpr * opcall);
    bool TraverseCallExpr(CallExpr *callexpr);
    bool TraverseMemberExpr(MemberExpr *memberexpr);
    bool TraverseIfStmt(IfStmt *ifs);
    bool TraverseForStmt(ForStmt *fors);
    bool TraverseSwitchStmt(SwitchStmt *switchs);
    bool ProcessSwitchCase(SwitchCase *cases);
    bool TraverseWhileStmt(WhileStmt *whiles);
    bool TraverseDoStmt(DoStmt *whiles);
    string FindVname(NamedDecl *vard);
    void AddVnames(hNodep &hvns);
    string FindFname(FunctionDecl *funcd);

    hNodep NormalizeAssignmentChain(hNodep hinp);
    void NormalizeSwitchStmt(hNodep hswitchbody);

    hfunc_name_map_t methodecls;  //  methods called in this SC_METHOD or function
    
    clang::DiagnosticsEngine &diag_e;
    
    hdecl_name_map_t vname_map;
    
    overridden_method_map_t &overridden_method_map_;
    
  private:
  
    hNodep h_ret;   // value returned by each subexpression

    //    hdecl_name_map_t vname_map;
    hdecl_name_map_t &mod_vname_map_;
    
    bool add_info; // variation in some hcode generated for modinit body


    const string nextstate_string = "_scclang_nextstate_";
    
    HDLBodyMode thismode;
    hfunc_name_map_t &allmethodecls_;

    HDLType *HDLt_userclassesp_;

    inline bool isUserClass(const Type * classrectype) {
      return  ((thismode == ruserdefclass) && (HDLt_userclassesp_ != NULL) &&
	       (HDLt_userclassesp_->usertype_info.userrectypes.count(classrectype)));
    }
    
    bool isLogicalOp(clang::OverloadedOperatorKind opc);
    
    inline bool isAssignOp(hNodep hp) {
      return (hp->h_op == hNode::hdlopsEnum::hBinop) &&
	(hp->h_name == "=");
    }

    inline bool isArrayRef(hNodep hp) {
      return (hp->h_op == hNode::hdlopsEnum::hBinop) &&
	(hp->h_name == "ARRAYSUBSCRIPT");
    }

    inline string generate_vname(string nm) {
      return vname_map.get_prefix()+ nm;
    }
    
    util lutil;

    const ASTContext &ast_context_;
  };
}

#endif
