#include "ArrayTypeUtils.h"

namespace sc_ast_matchers {
namespace utils {
namespace array_type {

IndexMapType getArrayInstanceIndex(
    // clang::InitListExpr *init_expr_list) {
    clang::CXXCtorInitializer *ctor_init) {
  /// Assumption: We will only support 1D, 2D and 3D arrays.

  IndexMapType indices;
  if (!ctor_init) {
    return indices;
  }

  clang::Expr *expr{ctor_init->getInit()->IgnoreImplicit()};
  if (!expr) {
    return indices;
  }
  // clang::InitListExpr *init_expr_list{
  // clang::dyn_cast<clang::InitListExpr>(expr)};

  if (auto init_expr_list = clang::dyn_cast<clang::InitListExpr>(expr)) {
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
      if (auto cexpr = clang::dyn_cast<clang::CXXConstructExpr>(iexpr)) {
        clang::CXXConstructExpr *nested_cexpr{
            clang::dyn_cast<clang::CXXConstructExpr>(
                cexpr->getArg(0)->IgnoreImplicit())};

        auto cxxbindexpr{clang::dyn_cast<clang::CXXBindTemporaryExpr>(
            nested_cexpr->getArg(0)->IgnoreParenImpCasts())};

        auto cxxctor{clang::dyn_cast<clang::CXXConstructExpr>(
            cxxbindexpr->getSubExpr()->IgnoreParenImpCasts())};
        clang::StringLiteral *slit{clang::dyn_cast<clang::StringLiteral>(
            cxxctor->getArg(0)->IgnoreImpCasts())};
        //

        llvm::outs() << "Argument 1d: [" << i << ", "
                     << "x, x ] " << slit->getString().str() << " \n";
        indices.insert(
            IndexPairType(slit->getString().str(), std::make_tuple(i, 0, 0)));
      }

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
            indices.insert(IndexPairType(slit->getString().str(),
                                         std::make_tuple(i, j, 0)));
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
  }
  return indices;
}

ArraySizesType getConstantArraySizes(const clang::FieldDecl *fd) {
  ArraySizesType sizes;

  clang::QualType field_type{fd->getType()};

  auto array_type{clang::dyn_cast<clang::ConstantArrayType>(field_type)};
  while (array_type != nullptr) {
    llvm::APInt array_size{};
    array_size = array_type->getSize();
    LLVM_DEBUG(llvm::dbgs() << "Size of array: " << array_size << "\n";);
    array_type =
        clang::dyn_cast<clang::ConstantArrayType>(array_type->getElementType());

    sizes.push_back(array_size);
  }

  return sizes;
}

ArraySizesType getArraySubscripts(const clang::Expr *expr) {
  ArraySizesType subscripts;

  /// Check if it is an ArraySubscriptExpr
  auto arr_sub_expr{clang::dyn_cast<clang::ArraySubscriptExpr>(expr)};
  while (arr_sub_expr != nullptr) {
    auto int_lit{
        clang::dyn_cast<clang::IntegerLiteral>(arr_sub_expr->getIdx())};
    llvm::outs() << "SUBSCRIPT: " << int_lit->getValue() << "\n";
    subscripts.insert(subscripts.begin(), int_lit->getValue());

    /// INFO: For some reason, dyn_cast on the ArraySubscriptExpr to get the
    /// ImplicitCastExpr does not work. So the way to get to it is to call
    /// getBase(), which makes sense that it accesses the nested array instance.
    arr_sub_expr = clang::dyn_cast<clang::ArraySubscriptExpr>(
        arr_sub_expr->getBase()->IgnoreParenImpCasts());
  }
  return subscripts;
}

const clang::MemberExpr *getArrayMemberExprName(const clang::Expr *expr) {
  /// Check if it is an ArraySubscriptExpr
  auto arr_sub_expr{clang::dyn_cast<clang::ArraySubscriptExpr>(expr)};
  while (arr_sub_expr != nullptr) {
    auto nested_sub_expr{clang::dyn_cast<clang::ArraySubscriptExpr>(
        arr_sub_expr->getBase()->IgnoreParenImpCasts())};

    /// Get access to the MemberExpr name.
    if (nested_sub_expr == nullptr) {
      const clang::MemberExpr *name_me{clang::dyn_cast<clang::MemberExpr>(
          arr_sub_expr->getBase()->IgnoreParenImpCasts())};
      llvm::outs() << "me_name: "<< name_me->getMemberNameInfo().getAsString() << "\n";
      return name_me;
    }
    arr_sub_expr = nested_sub_expr;
  }
  return nullptr;
}

}  // namespace array_type

}  // namespace utils
}  // namespace sc_ast_matchers
