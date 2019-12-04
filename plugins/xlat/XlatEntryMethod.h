#ifndef _XLATENTRYMETHOD_H_
#define _XLATENTRYMETHOD_H_

#include <vector>
#include <string>
#include "clang/AST/DeclCXX.h"
#include "clang/AST/Stmt.h"
#include "clang/AST/Expr.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"

#include "llvm/ADT/StringRef.h"

// look at https://chromium.googlesource.com/chromium/src.git/+/62.0.3178.1/tools/clang/value_cleanup/ListValueRewriter.h#include "llvm/Support/FileSystem.h"

#include <stack>

#include "SystemCClang.h"

using namespace clang;
using namespace scpar;

namespace hnode {
  class hNode;

  typedef hNode * hNodep;

  class hNode {

#define HNODEen \
  etype(hNoop), \
  etype(hModule), \
  etype(hProcess), \
  etype(hCStmt), \
  etype(hPortin), \
  etype(hPortout), \
  etype(hPortio), \
  etype(hSensvar), \
  etype(hSensedge), \
  etype(hType), \
  etype(hInt), \
  etype(hSigdecl), \
  etype(hVardecl), \
  etype(hSigAssignL), \
  etype(hSigAssignR), \
  etype(hVarAssign), \
  etype(hBinop), \
  etype(hUnop), \
  etype(hIfStmt), \
  etype(hForStmt), \
  etype(hForInit), \
  etype(hForCond), \
  etype(hForInc), \
  etype(hForBody), \
    etype(hWhileStmt),				\
  etype(hLiteral), \
  etype(hUnimpl), \
  etype(hLast)


  public:

#define etype(x) x
 
    typedef enum { HNODEen } hdlopsEnum;

    bool is_leaf;
    
    string h_name;
    hdlopsEnum h_op;
    list<hNodep> child_list;
 
#undef etype
#define etype(x) #x

    const string hdlop_pn [hLast+1]  =  { HNODEen };

    hNode() { is_leaf = true;}
    hNode(bool lf) {
      is_leaf = lf;
      h_op = hdlopsEnum::hNoop;
    }
  
    hNode(string s, hdlopsEnum h) {
      is_leaf = true;
      h_name = s;
      h_op = h;
    }
  
    ~hNode() { if (!is_leaf) child_list.clear(); }
  

    void setleaf(string s, hdlopsEnum h) {
      is_leaf = true;
      h_name = s;
      h_op = h;
    }

    string printname(hdlopsEnum opc) {
      if (is_leaf) return hdlop_pn[static_cast<int>(opc)];
      else return "NON_LEAF";
    }

    // for completeness
    hdlopsEnum str2hdlopenum(string st) {
      const int n = sizeof (hdlop_pn)/sizeof (hdlop_pn[0]);
      for (int i = 0; i < n; i++) {
	if (hdlop_pn[i] == st)
	  return (hdlopsEnum) i;
      }
      return hLast;
    }
    void print(llvm::raw_fd_ostream & modelout, unsigned int indnt=2) {
      if (is_leaf) {
	modelout.indent(indnt);
	modelout << "(" << printname(h_op) << " " << h_name << ")" <<"\n";

      }
      else {
	modelout.indent(indnt);
	modelout << "[\n";
	for (auto child : child_list)
	  if (child)
	    child->print(modelout, indnt+2);
	  else {
	    modelout.indent(indnt+2);
	    modelout << "<null child>\n";
	  }
	modelout.indent(indnt);
	modelout << "]\n";
      }
    }

  };
}

using namespace hnode;

class XlatMethod: public RecursiveASTVisitor <XlatMethod> {
 public:
  XlatMethod(CXXMethodDecl * emd, hNodep &h_top, llvm::raw_ostream & os);
  XlatMethod(Stmt * stmt, hNodep & h_top, llvm::raw_ostream & os);
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
  bool TraverseWhileStmt(WhileStmt *whiles);
  //CXXMethodDecl *getEMD();
  
 private:
  
  //CXXMethodDecl * _emd;
  llvm::raw_ostream & os_;
  //  size_t n;
  
  hNodep h_ret;   // value returned by each subexpression
};


#endif
