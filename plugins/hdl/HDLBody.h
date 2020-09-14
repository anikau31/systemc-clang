#ifndef _HDLBODY_H
#define _HDLBODY_H

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

using namespace clang;
using namespace systemc_clang;


using namespace hnode;

class HDLBody: public RecursiveASTVisitor <HDLBody> {
 public:
  HDLBody(CXXMethodDecl * emd, hNodep &h_top, clang::DiagnosticsEngine &diag_engine);
  HDLBody(Stmt * stmt, hNodep &h_top, clang::DiagnosticsEngine &diag_engine);
  virtual ~HDLBody();

  bool TraverseCompoundStmt(CompoundStmt* compoundStmt);
  bool TraverseStmt(Stmt *stmt);
  bool TraverseDeclStmt(DeclStmt * declstmt);
  bool ProcessVarDecl(VarDecl * vardecl);
  bool TraverseBinaryOperator(BinaryOperator* expr);
  bool TraverseUnaryOperator(UnaryOperator* expr);
  bool TraverseIntegerLiteral(IntegerLiteral * lit);
  bool TraverseCXXBoolLiteralExpr(CXXBoolLiteralExpr * b);
  bool TraverseDeclRefExpr(DeclRefExpr* expr);
  bool TraverseArraySubscriptExpr(ArraySubscriptExpr* expr);
  bool TraverseCXXMemberCallExpr(CXXMemberCallExpr *callexpr);
  bool TraverseCXXOperatorCallExpr(CXXOperatorCallExpr * opcall);
  bool TraverseMemberExpr(MemberExpr *memberexpr);
  bool TraverseIfStmt(IfStmt *ifs);
  bool TraverseForStmt(ForStmt *fors);
  bool TraverseSwitchStmt(SwitchStmt *switchs);
  bool ProcessSwitchCase(SwitchCase *cases);
  bool TraverseWhileStmt(WhileStmt *whiles);
  void AddVnames(hNodep &hvns);
  //CXXMethodDecl *getEMD();

  std::unordered_map<string, CXXMethodDecl *> methodecls;  //  methods called in this SC_METHOD
  clang::DiagnosticsEngine &diag_e;
  
 private:
  
  hNodep h_ret;   // value returned by each subexpression
  hname_map_t vname_map;
  name_serve lname;

  bool isLogicalOp(clang::OverloadedOperatorKind opc);

  util lutil;
};


#endif
