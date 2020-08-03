#ifndef _FIND_TEMPLATE_PARAMETERS_H_
#define _FIND_TEMPLATE_PARAMETERS_H_

#include <vector>

#include "clang/AST/RecursiveASTVisitor.h"
#include "llvm/Support/raw_ostream.h"

// Forward declarations.
namespace clang {
class CXXRecordDecl;
}

namespace systemc_clang {

class FindTemplateParameters
    : public clang::RecursiveASTVisitor<FindTemplateParameters> {
 public:
  FindTemplateParameters(clang::CXXRecordDecl *, llvm::raw_ostream &);
  virtual bool VisitCXXRecordDecl(clang::CXXRecordDecl *decl);

  virtual ~FindTemplateParameters();

  const std::vector<std::string> getTemplateParameters() const;
  const std::vector<std::string> getTemplateArgs() const;

  void dump();

 private:
  clang::CXXRecordDecl *declaration_;
  llvm::raw_ostream &os_;
  clang::TemplateParameterList *template_parameters_;
  const clang::TemplateArgumentList *template_args_;
};
}  // namespace systemc_clang
#endif
