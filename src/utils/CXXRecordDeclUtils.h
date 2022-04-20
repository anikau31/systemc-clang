#ifndef _CXX_RECORD_DECL_UTILS_H_
#define _CXX_RECORD_DECL_UTILS_H_

#undef DEBUG_TYPE
#define DEBUG_TYPE "CXXConstructDeclUtils"

#include <string>
#include <vector>
//#include "clang/AST/Type.h"

namespace clang {
class CXXRecordDecl;
class CXXCtorInitializer;
class FieldDecl;
};  // namespace clang

namespace sc_ast_matchers {
namespace utils {
namespace cxx_construct_decl_utils {

typedef std::tuple<clang::FieldDecl *, std::string,
                   const clang::CXXCtorInitializer *>
    ModuleInitializerTupleType;

std::vector<const clang::CXXRecordDecl *> getAllBaseClasses(
    const clang::CXXRecordDecl *decl);

/// Get the first constructor argument.
//
std::vector<ModuleInitializerTupleType> getModuleInitializerNames(
    const clang::CXXCtorInitializer *init);

}  // namespace cxx_construct_decl_utils

}  // namespace utils

}  // namespace sc_ast_matchers

#endif
