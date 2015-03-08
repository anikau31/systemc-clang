#ifndef _SC_MODULES_H_
#define _SC_MODULES_H_
#include <map>
#include <string>
#include <set>
#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"

namespace scpar {
  using namespace clang;
  using namespace std;

  class SCModules:public RecursiveASTVisitor < SCModules > {
  public:

    typedef map < string, CXXRecordDecl * >moduleMapType;
    typedef pair < string, CXXRecordDecl * >modulePairType;
    
    SCModules(TranslationUnitDecl *, llvm::raw_ostream &);
    virtual bool VisitCXXRecordDecl(CXXRecordDecl *);
    moduleMapType getSystemCModulesMap();
    void printSystemCModulesMap();

    // ===
    // Analyze the simple for loop
    //
    // int a[5];
    // for (int i = 0; i < 5; ++i) {
    //     a[i] = i;
    // }
    // ===
    bool VisitForStmt(ForStmt *stmt);
    void analyze_data_struct(Stmt *stmtList);
    void analyze_expr(Expr *expr);
    void analyze_lhs(Expr *expr);
    void analyze_rhs(Expr *expr);
    void analyze_array_base(Expr *base, bool isLHS);
    void analyze_decl_ref_expr(DeclRefExpr *declRef);
  
  private:
    llvm::raw_ostream & _os;
    moduleMapType _moduleMap;

    std::set<ValueDecl*> lhs_decls;
    std::set<ValueDecl*> rhs_decls;
  };
}
#endif
