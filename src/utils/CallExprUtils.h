#ifndef _CALL_EXPR_UTILS_H_
#define _CALL_EXPR_UTILS_H_

#undef DEBUG_TYPE
#define DEBUG_TYPE "CallExprUtils"

#include "clang/AST/DeclCXX.h"
#include "clang/AST/ExprCXX.h"

#include "llvm/ADT/StringRef.h"
#include <vector>

namespace clang {
class CallExpr;
class Type;
class FunctionDecl;
class ValueDecl; 
class Expr;
class CXXRecordDecl;
class NamespaceDecl;

};  // namespace clang

namespace sc_ast_matchers {
namespace utils {

template <typename T>
llvm::StringRef getClassNameFromDecl(T *decl) {
llvm::StringRef empty{""};
  if (!decl) return empty;

  const clang::DeclContext* dc{ decl->getLexicalParent() };
  if (dc && dc->isNamespace() ) {
    if (const auto *nd = llvm::dyn_cast<clang::NamespaceDecl>(dc)) {
      clang::IdentifierInfo *iinfo = nd->getIdentifier();
      //llvm::dbgs() << "@@ name is " << iinfo->getName() << " for ";
      return iinfo->getName();
    }
  }

  return empty;
}


/*
 * \brief Check if a Type is declared within a given namespace.
 */
bool isInNamespace(const clang::Type *tp, const std::vector<llvm::StringRef> &names);
bool isInNamespace(const clang::Expr *expr, const std::vector<llvm::StringRef> &names);

bool isInNamespace(const clang::ValueDecl *fd, const std::vector<llvm::StringRef> &names);

// bool isInNamespace(const clang::CallExpr *cexpr, const std::vector<llvm::StringRef>& names);
void collect_sugar(const clang::Type* type, std::vector<clang::Type*> &unwrapped_types);

//llvm::StringRef getClassNameFromDecl( const clang::CXXRecordDecl* decl );
//llvm::StringRef getClassNameFromDecl( const clang::FunctionDecl* decl );

bool matchNames( llvm::StringRef str, const std::vector<llvm::StringRef> &names);
/*
 * \brief Check if a CallExpr is declared within a given namespace.
 */
bool isInNamespace(const clang::CallExpr *cexpr, llvm::StringRef name);

}  // namespace utils

}  // namespace sc_ast_matchers

#endif
