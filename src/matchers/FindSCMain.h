#ifndef _FIND_SCMAIN_H_
#define _FIND_SCMAIN_H_

#include "clang/AST/RecursiveASTVisitor.h"
#include "llvm/Support/raw_ostream.h"

namespace clang {
class FunctionDecl;
class TranslationUnitDecl;

};  // namespace clang

namespace systemc_clang {
class FindSCMain : public clang::RecursiveASTVisitor<FindSCMain> {
 public:
  FindSCMain(clang::TranslationUnitDecl *, llvm::raw_ostream &);
  virtual ~FindSCMain();
  virtual bool VisitFunctionDecl(clang::FunctionDecl *decl);

  clang::FunctionDecl *getSCMainFunctionDecl() const;
  bool isSCMainFound() const;

 private:
  llvm::raw_ostream &os_;
  clang::FunctionDecl *sc_main_function_declaration_;
};
}  // namespace systemc_clang
#endif
