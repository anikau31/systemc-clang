#ifndef _CALL_EXPR_UTILS_H_
#define _CALL_EXPR_UTILS_H_

#undef DEBUG_TYPE
#define DEBUG_TYPE "CallExprUtils"

#include "llvm/ADT/StringRef.h"
#include <vector>

namespace clang {
class CallExpr;
class Type;
};  // namespace clang

namespace sc_ast_matchers {
namespace utils {

/*
 * \brief Check if a Type is declared within a given namespace.
 */
bool isInNamespace(const clang::Type *tp, std::vector<llvm::StringRef> &names);

/*
 * \brief Check if a CallExpr is declared within a given namespace.
 */
bool isInNamespace(const clang::CallExpr *cexpr, llvm::StringRef name);

}  // namespace utils

}  // namespace sc_ast_matchers

#endif
