#ifndef _GET_AST_INFO_
#define _GET_AST_INFO_

#include <vector>
#include "clang/AST/Type.h"
#include "clang/AST/DeclCXX.h"

#undef DEBUG_TYPE
#define DEBUG_TYPE "GetASTInfo"

namespace sc_ast_matchers {

class GetASTInfo {
 private:
  GetASTInfo(){};

 public:
  typedef std::pair<std::string,
                    std::tuple<std::size_t, std::size_t, std::size_t>>
      IndexPairType;
  typedef std::map<std::string,
                   std::tuple<std::size_t, std::size_t, std::size_t>>
      IndexMapType;
  // typedef std::map<std::string, std::tuple<std::size_t, std::size_t,
  // std::size_t>> indices;
  typedef std::vector<llvm::APInt> ArraySizesType;
  /// \brief Returns the indices associated with each element of an array of
  /// objects that is instantiated using a constructor.
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
  static IndexMapType getArrayInstanceIndex(
      clang::InitListExpr *init_expr_list) {
    /// Assumption: We will only support 1D, 2D and 3D arrays.

    IndexMapType indices;
    /// Retrieve the number of initializer lists.
    llvm::outs() << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
    llvm::outs() << "Number of inits: " << init_expr_list->getNumInits()
                 << "\n";
    clang::Expr **iexpr_set{init_expr_list->getInits()};

    for (std::size_t i{0}; i < init_expr_list->getNumInits(); ++i) {
      llvm::outs() << "Iterate over init 1d lists: " << i << "\n";
      clang::Expr *iexpr{iexpr_set[i]};

      // Level 1
      // iexpr->dump();

      /// Level 2
      if (auto init_expr_list_2d =
              clang::dyn_cast<clang::InitListExpr>(iexpr)) {
        clang::Expr **iexpr_2d_set{init_expr_list_2d->getInits()};
        for (std::size_t j{0}; j < init_expr_list_2d->getNumInits(); ++j) {
          llvm::outs() << "Iterate over 2d init lists: " << j << "\n";
          clang::Expr *iexpr_2d{iexpr_2d_set[j]};

          // iexpr_2d->dump();
          /// Get the constructor argument.
          if (auto cexpr = clang::dyn_cast<clang::CXXConstructExpr>(iexpr_2d)) {
            clang::CXXConstructExpr *nested_cexpr{
                clang::dyn_cast<clang::CXXConstructExpr>(
                    cexpr->getArg(0)->IgnoreImplicit())};
            clang::StringLiteral *slit{clang::dyn_cast<clang::StringLiteral>(
                nested_cexpr->getArg(0)->IgnoreImpCasts())};
            llvm::outs() << "Argument 2d: [" << i << ", " << j << "] "
                         << slit->getString().str() << " \n";
          }

          /// Level 3
          if (auto init_expr_list_3d =
                  clang::dyn_cast<clang::InitListExpr>(iexpr_2d)) {
            clang::Expr **iexpr_3d_set{init_expr_list_3d->getInits()};
            for (std::size_t k{0}; k < init_expr_list_3d->getNumInits(); ++k) {
              llvm::outs() << "Iterate over 3d init lists: " << k << "\n";
              clang::Expr *iexpr_3d{iexpr_3d_set[k]};
              // iexpr_3d->dump();

              if (auto cexpr =
                      clang::dyn_cast<clang::CXXConstructExpr>(iexpr_3d)) {
                clang::CXXConstructExpr *nested_cexpr{
                    clang::dyn_cast<clang::CXXConstructExpr>(
                        cexpr->getArg(0)->IgnoreImplicit())};
                auto cxxbindexpr{clang::dyn_cast<clang::CXXBindTemporaryExpr>(
                    nested_cexpr->getArg(0)->IgnoreParenImpCasts())};
                auto cxxctor{clang::dyn_cast<clang::CXXConstructExpr>(
                    cxxbindexpr->getSubExpr()->IgnoreParenImpCasts())};
                clang::StringLiteral *slit{
                    clang::dyn_cast<clang::StringLiteral>(
                        cxxctor->getArg(0)->IgnoreImpCasts())};
                // slit->dump();
                llvm::outs() << "Argument 3d: [" << i << ", " << j << ", " << k
                             << "] " << slit->getString().str() << " \n";
                indices.insert(IndexPairType(slit->getString().str(),
                                         std::make_tuple(i, j, k)));
              }
            }
          }
        }
      }
    }
    return indices;
  }

  /// \brief Returns the sizes of an array that are provided when the array is
  /// declared.
  ///
  /// For example,
  /// AType a[4][5];
  /// Would return 4 and 5 in a vector.
  ///
  static ArraySizesType getConstantArraySizes(const clang::FieldDecl *fd) {
    ArraySizesType sizes;

    clang::QualType field_type{fd->getType()};

    auto array_type{clang::dyn_cast<clang::ConstantArrayType>(field_type)};
    while (array_type != nullptr) {
      llvm::APInt array_size{};
      array_size = array_type->getSize();
      LLVM_DEBUG(llvm::dbgs() << "Size of array: " << array_size << "\n";);
      array_type = clang::dyn_cast<clang::ConstantArrayType>(
          array_type->getElementType());

      sizes.push_back(array_size);
    }

    return sizes;
  }
};

};  // namespace sc_ast_matchers

#endif
