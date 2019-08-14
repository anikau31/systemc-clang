#ifndef _FIND_MODULE_INSTANCE_H_
#define _FIND_MODULE_INSTANCE_H_

#include "clang/AST/DeclCXX.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "llvm/Support/raw_ostream.h"

namespace scpar {

using namespace clang;
using namespace std;

class FindModuleInstance : public RecursiveASTVisitor<FindModuleInstance> {
public:
  FindModuleInstance(CXXRecordDecl *, llvm::raw_ostream &);
  virtual bool VisitCXXConstructExpr(CXXConstructExpr *expr);
  virtual bool VisitFieldDecl(FieldDecl *fdecl);

  virtual ~FindModuleInstance();

  void dump();
  string getInstanceName() const;

private:
  CXXRecordDecl *declaration_;
  llvm::raw_ostream &os_;
  string instance_name_;
};
} // namespace scpar
#endif
