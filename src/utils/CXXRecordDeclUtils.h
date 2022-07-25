#ifndef _CXX_RECORD_DECL_UTILS_H_
#define _CXX_RECORD_DECL_UTILS_H_

#undef DEBUG_TYPE
#define DEBUG_TYPE "CXXConstructDeclUtils"

#include <string>
#include <vector>

#include "llvm/ADT/StringRef.h"

//#include "clang/AST/Type.h"

namespace clang {
class CXXRecordDecl;
class CXXCtorInitializer;
class FieldDecl;
class CXXMemberCallExpr;
class Type;
class CallExpr;
};  // namespace clang

namespace sc_ast_matchers {
namespace utils {

bool isCXXMemberCallExprSystemCCall(const clang::Type *type,
                                    const std::vector<llvm::StringRef> &names);
bool isCXXMemberCallExprSystemCCall(const clang::CXXMemberCallExpr *mce);

bool isCXXMemberCallExprSystemCCall(const clang::CallExpr *ce,
                                    const std::vector<llvm::StringRef> &names);

typedef std::tuple<clang::FieldDecl *, std::string,
                   const clang::CXXCtorInitializer *>
    ModuleInitializerTupleType;

std::vector<const clang::CXXRecordDecl *> getAllBaseClassNames(
    const clang::CXXRecordDecl *decl);

std::vector<const clang::CXXRecordDecl *> getAllBaseClasses(
    const clang::CXXRecordDecl *decl);

/// Get the first constructor argument.
//
std::vector<ModuleInitializerTupleType> getModuleInitializerNames(
    const clang::CXXCtorInitializer *init);

}  // namespace utils

}  // namespace sc_ast_matchers

#endif
