#ifndef _CALL_EXPR_UTILS_H_
#define _CALL_EXPR_UTILS_H_

#undef DEBUG_TYPE
#define DEBUG_TYPE "CallExprUtils"

#include "llvm/ADT/StringRef.h"
#include <vector>

namespace clang {
class CallExpr;
class Type;
class FunctionDecl;
};  // namespace clang

namespace sc_ast_matchers {
namespace utils {

/*
 * \brief Check if a Type is declared within a given namespace.
 */
bool isInNamespace(const clang::Type *tp, const std::vector<llvm::StringRef> &names);

bool isInNamespace(const clang::FunctionDecl *fd, const std::vector<llvm::StringRef> &names);

bool isInNamespace(const clang::CallExpr *cexpr, const std::vector<llvm::StringRef>& names);
void collect_sugar(const clang::Type* type, std::vector<clang::Type*> &unwrapped_types);

/*
 * \brief Check if a CallExpr is declared within a given namespace.
 */
bool isInNamespace(const clang::CallExpr *cexpr, llvm::StringRef name);

}  // namespace utils

}  // namespace sc_ast_matchers

#endif
