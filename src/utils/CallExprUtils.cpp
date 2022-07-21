#include "CallExprUtils.h"

#include "clang/AST/DeclCXX.h"
#include "clang/AST/ExprCXX.h"
#include "llvm/Support/Debug.h"

namespace sc_ast_matchers {
namespace utils {

using namespace clang;
using namespace llvm;

bool isInNamespace(const clang::Type *tp, const std::vector<llvm::StringRef> &names) {
  if (!tp) {
    return false;
  }

  Type *tap = const_cast<Type *>(tp->getUnqualifiedDesugaredType());

  if (tap->isBuiltinType()) {
    return false;
  }

  DeclContext *dc{nullptr};
  if (tap->isArrayType()) {
    auto cat1 = dyn_cast<ArrayType>(tap);
    const Type *tp = cat1->getElementType().getTypePtr();

    // Unwrap array element type.
    Type *unwrap_tp{nullptr};
    while (tp && tp != unwrap_tp) {
      if (auto cat2 = dyn_cast<ArrayType>(tp)) {
        unwrap_tp = const_cast<Type *>(tp);
        tp = cat2->getElementType().getTypePtr();
      } else {
        unwrap_tp = const_cast<Type *>(tp);
      }
    }

    tap = unwrap_tp;
  }

  if (tap->isRecordType()) {
    const RecordDecl *rdecl = tap->getAsRecordDecl();
    dc = const_cast<clang::DeclContext *>(rdecl->getLexicalParent());
  }

  if (dc && dc->isNamespace()) {
    if (const auto *nd = llvm::dyn_cast<clang::NamespaceDecl>(dc)) {
      auto iinfo = nd->getIdentifier();
      //llvm::dbgs() << "@@ name is " << iinfo->getName() << " for ";
      for (const auto name : names) {
        if (iinfo->isStr(name)) {
          return true;
        }
      }
      return false;
    }
  }

  return false;
}

bool isInNamespace(const CallExpr *cexpr, const std::vector<llvm::StringRef>& names) {
  if (!cexpr) {
    return false;
  }
  return isInNamespace(cexpr->getType().getTypePtr(), names);
}

bool isInNamespace(const CallExpr *cexpr, llvm::StringRef name) {
  if (!cexpr) {
    return false;
  }
  std::vector<llvm::StringRef> names{name};
  return isInNamespace(cexpr->getType().getTypePtr(), names);
  /*
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
  */

  // return false;
}
}  // namespace utils
}  // namespace sc_ast_matchers
