#ifndef _FIND_GLOBAL_VARIABLES_H_
#define _FIND_GLOBAL_VARIABLES_H_

#include <map>
#include <vector>
#include <string>
#include "clang/AST/Decl.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"

namespace scpar {
  using namespace clang;
  using namespace std;
  
  class FindGlobalVariables: public RecursiveASTVisitor<FindGlobalVariables> {
  public:
    /// typedefs
    /// k: name, v: VarDecl*
    typedef map<string, VarDecl*> globalVariableMapType;
    typedef pair<string, VarDecl*> globalVariablePairType;


    FindGlobalVariables(TranslationUnitDecl*, llvm::raw_ostream&);

    /// RecursiveASTVisitor virtual methods. 
    virtual bool VisitVarDecl(VarDecl*);
		virtual bool VisitStmt(Stmt*);
    /// Accessors. 
    globalVariableMapType getVariableMap();
    vector<string> getVariableNames();
    
    void dump();

  private:
    llvm::raw_ostream &_os;
    globalVariableMapType _globalVariables;
  };
}
#endif
