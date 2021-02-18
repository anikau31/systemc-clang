#ifndef ARRAY_TYPE_UTILS_H_
#define ARRAY_TYPE_UTILS_H_

#include <vector>
#include "clang/AST/Type.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/ExprCXX.h"

#undef DEBUG_TYPE
#define DEBUG_TYPE "ArrayTypeUtils"

namespace sc_ast_matchers {
namespace utils {
namespace array_type {

typedef std::pair<std::string,
                  std::tuple<std::size_t, std::size_t, std::size_t>>
    IndexPairType;
typedef std::map<std::string, std::tuple<std::size_t, std::size_t, std::size_t>>
    IndexMapType;
// typedef std::map<std::string, std::tuple<std::size_t, std::size_t,
// std::size_t>> indices;
typedef std::vector<llvm::APInt> ArraySizesType;
typedef std::vector<const clang::Expr *> ArraySizesExprType;

/// Returns the indices associated with each element of an array of objects
/// that is instantiated using a constructor.
///
/// For example,
/// class A { public: A( std::string s) {} }
/// A array_a[2][3]{
///   {"0_0", "0_1", "0_2"},
///   {"1_0", "1_1", "1_2"},
///   {"2_0", "2_1", "2_2"}
/// };
///
/// This should identify the appropriate index for each instance of object A.
/// That is, array_a[0][1] should be associated with instance named "0_1".

IndexMapType getArrayInstanceIndex(clang::CXXCtorInitializer *ctor_init);

/// Returns the sizes of an array that are provided when the array is
/// declared.
///
/// For example,
/// AType a[4][5];
/// Would return 4 and 5 in a vector.
///
//ArraySizesType getConstantArraySizes(const clang::FieldDecl *fd);
ArraySizesType getConstantArraySizes(const clang::ValueDecl *fd);

/// Returns the array subscripts used in an ArraySubscriptExpr.
/// For example, arr[0][1] would return 0, 1.
ArraySizesExprType getArraySubscripts(const clang::Expr *expr);

const clang::MemberExpr *getArrayMemberExprName(const clang::Expr *expr);
};  // namespace array_type
};  // namespace utils

};  // namespace sc_ast_matchers

#endif
