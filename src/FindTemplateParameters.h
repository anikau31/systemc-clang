#ifndef _FIND_TEMPLATE_PARAMETERS_H_
#define _FIND_TEMPLATE_PARAMETERS_H_

#include "clang/AST/DeclCXX.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "llvm/Support/raw_ostream.h"

namespace scpar {

using namespace clang;
using namespace std;

class FindTemplateParameters : public RecursiveASTVisitor<FindTemplateParameters> {
public:
  FindTemplateParameters(CXXRecordDecl *, llvm::raw_ostream &);
  virtual bool VisitCXXRecordDecl(CXXRecordDecl *decl);

  virtual ~FindTemplateParameters();

  void dump();
  vector<string> getTemplateParameters() const;

private:
  CXXRecordDecl *declaration_;
  llvm::raw_ostream &os_;
  TemplateParameterList *template_parameters_;
};
} // namespace scpar
#endif
