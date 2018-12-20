#ifndef _FIND_MODULE_H_
#define _FIND_MODULE_H_

#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"

namespace scpar {

  using namespace clang;
  using namespace std;

  class FindModule: public RecursiveASTVisitor <FindModule> {
  public:
    FindModule( CXXRecordDecl *, llvm::raw_ostream & );
    virtual bool VisitCXXRecordDecl( CXXRecordDecl *decl );
    virtual ~FindModule();

    void dump();
    string getModuleName() const;
    bool isSystemCModule() const;

  private:
    CXXRecordDecl *declaration_;
    llvm::raw_ostream &os_;
    bool is_systemc_module_;
    string module_name_;

  };
}
#endif
