#ifndef _XLATENTRYMETHOD_H_
#define _XLATENTRYMETHOD_H_

#include <vector>
#include <string>
#include <unordered_set>
#include <ctype.h>
#include "clang/AST/DeclCXX.h"
#include "clang/AST/Stmt.h"
#include "clang/AST/Expr.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"

#include "llvm/ADT/StringRef.h"

// look at https://chromium.googlesource.com/chromium/src.git/+/62.0.3178.1/tools/clang/value_cleanup/ListValueRewriter.h#include "llvm/Support/FileSystem.h"

#include "SystemCClang.h"
#include "hNode.h"

using namespace clang;
using namespace scpar;


using namespace hnode;

class XlatMethod: public RecursiveASTVisitor <XlatMethod> {
 public:
  XlatMethod(CXXMethodDecl * emd, hNodep &h_top, llvm::raw_ostream & os);
  XlatMethod(Stmt * stmt, hNodep &h_top, llvm::raw_ostream & os);
  virtual ~XlatMethod();

  bool TraverseCompoundStmt(CompoundStmt* compoundStmt);
  bool TraverseStmt(Stmt *stmt);
  bool TraverseDeclStmt(DeclStmt * declstmt);
  bool ProcessVarDecl(VarDecl * vardecl, hNodep &h_vardecl);
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

 private:
  
  //CXXMethodDecl * _emd;
  llvm::raw_ostream & os_;
  //  size_t n;
  
  hNodep h_ret;   // value returned by each subexpression
  typedef struct {
    string oldn;
    string newn;
    hNodep vardeclp;
  } names_t;
  std::map<Decl *, std::vector<DeclRefExpr *>> vuse_map;
  std::map<Decl *, names_t> vname_map;

  bool isLogicalOp(clang::OverloadedOperatorKind opc);
  
  const string prefix = "_XLAT_";
  int cnt;
  inline string newname() {
    return (prefix+to_string(cnt++));
  }
   util lutil;
};


#endif
