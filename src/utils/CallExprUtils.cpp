#include "CallExprUtils.h"

#include "clang/AST/DeclCXX.h"
#include "clang/AST/ExprCXX.h"
#include "llvm/Support/Debug.h"

namespace sc_ast_matchers {
namespace utils {

  using namespace clang;
  using namespace llvm;

bool isInNamespace(const CallExpr *cexpr, llvm::StringRef name) {
  if (!cexpr) {
    return false;
  }

  /// This is a CXXMemberCallExpr.
  DeclContext *dc{nullptr};
  if (auto call = llvm::cast<CXXMemberCallExpr>(cexpr)) {
    if (auto rdecl = call->getRecordDecl()) {
      dc = const_cast<DeclContext *>(rdecl->getLexicalParent());
    }
  } else {
    /// This is a function call.
    if (const FunctionDecl *fd = cexpr->getDirectCallee()) {
      dc = const_cast<DeclContext *>(fd->getDeclContext());
    }
  }

  if (dc && dc->isNamespace()) {
    if (const auto *nd = llvm::dyn_cast<NamespaceDecl>(dc)) {
      auto iinfo = nd->getIdentifier();
      LLVM_DEBUG(llvm::dbgs() << "name is " << iinfo->getName() << " ";);
      return iinfo->isStr(name);
    }
  }

  return false;
}
}  // namespace utils
}  // namespace sc_ast_matchers
