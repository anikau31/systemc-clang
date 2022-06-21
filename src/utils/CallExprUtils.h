#ifndef _CALL_EXPR_UTILS_H_
#define _CALL_EXPR_UTILS_H_

#undef DEBUG_TYPE
#define DEBUG_TYPE "CallExprUtils"

#include "llvm/ADT/StringRef.h"

namespace clang {
class CallExpr;
};  // namespace clang

namespace sc_ast_matchers {
namespace utils {

bool isInNamespace(const clang::CallExpr *cexpr, llvm::StringRef name);

}  // namespace utils

}  // namespace sc_ast_matchers

#endif
