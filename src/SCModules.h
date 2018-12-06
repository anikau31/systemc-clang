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
    virtual ~SCModules();
    virtual bool VisitCXXRecordDecl(CXXRecordDecl *);
    moduleMapType getSystemCModulesMap();
    void printSystemCModulesMap();

  private:
    llvm::raw_ostream & _os;
    moduleMapType _moduleMap;
  };
}
#endif
