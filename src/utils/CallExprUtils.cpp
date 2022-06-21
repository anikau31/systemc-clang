#include "CallExprUtils.h"

#include "clang/AST/DeclCXX.h"
#include "clang/AST/ExprCXX.h"
#include "llvm/Support/Debug.h"

namespace sc_ast_matchers {
namespace utils {

bool isInNamespace(const clang::CallExpr *cexpr, llvm::StringRef name) {
  if (!cexpr) {
    return false;
  }
  if (const clang::FunctionDecl *fd = cexpr->getDirectCallee()) {
    if (auto dc = fd->getDeclContext()) {
      if (dc->isNamespace()) {
        if (const auto *nd = llvm::cast<clang::NamespaceDecl>(dc)) {
          auto iinfo = nd->getIdentifier();
          llvm::dbgs() << "name is " << iinfo->getName() << " for "
          << fd->getQualifiedNameAsString() ;
          return iinfo->isStr(name);
        }
      }
    }
  }

  return false;
}

}  // namespace utils
}  // namespace sc_ast_matchers
