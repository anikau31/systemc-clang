#ifndef _FIND_TEMPLATE_PARAMETERS_H_
#define _FIND_TEMPLATE_PARAMETERS_H_

#include <vector>

#include "clang/AST/RecursiveASTVisitor.h"

#undef DEBUG_TYPE
#define DEBUG_TYPE "Find"

// Forward declarations.
namespace clang {
class CXXRecordDecl;
}

namespace systemc_clang {

class FindTemplateParameters
    : public clang::RecursiveASTVisitor<FindTemplateParameters> {
 public:
  FindTemplateParameters(clang::CXXRecordDecl *);
  virtual bool VisitCXXRecordDecl(clang::CXXRecordDecl *decl);

  virtual ~FindTemplateParameters();

  const std::vector<std::string> getTemplateParameters() const;
  const std::vector<std::string> getTemplateArgs() const;

  void dump();

 private:
  clang::CXXRecordDecl *declaration_;
  clang::TemplateParameterList *template_parameters_;
  const clang::TemplateArgumentList *template_args_;
};
}  // namespace systemc_clang
#endif
