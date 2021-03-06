#ifndef _FIND_SCMAIN_H_
#define _FIND_SCMAIN_H_

#include "clang/AST/DeclCXX.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "llvm/Support/raw_ostream.h"

namespace systemc_clang {

using namespace clang;
class FindSCMain : public RecursiveASTVisitor<FindSCMain> {
public:
  FindSCMain(TranslationUnitDecl *, llvm::raw_ostream &);
  virtual ~FindSCMain();
  virtual bool VisitFunctionDecl(FunctionDecl *decl);

  FunctionDecl *getSCMainFunctionDecl() const;
  bool isSCMainFound() const;

private:
  llvm::raw_ostream &os_;
  FunctionDecl *sc_main_function_declaration_;
};
} // namespace systemc_clang
#endif
