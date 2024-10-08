#include "ArrayTypeUtils.h"

#include "clang/AST/DeclCXX.h"
#include "clang/AST/ExprCXX.h"
#include "llvm/Support/Debug.h"

#include <iostream>

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
    clang::Expr **iexpr_set{init_expr_list->getInits()};

    for (std::size_t i{0}; i < init_expr_list->getNumInits(); ++i) {
      LLVM_DEBUG(llvm::dbgs() << "Iterate over init 1d lists: " << i << "\n";);
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

        if (slit) {
          LLVM_DEBUG(llvm::dbgs()
                         << "Argument 1d: [" << i << ", "
                         << "x, x ] " << slit->getString().str() << " \n";);
          indices.insert(
              IndexPairType(slit->getString().str(), std::make_tuple(i, 0, 0)));
        }
      }

      /// Level 2
      if (auto init_expr_list_2d =
              clang::dyn_cast<clang::InitListExpr>(iexpr)) {
        clang::Expr **iexpr_2d_set{init_expr_list_2d->getInits()};
        for (std::size_t j{0}; j < init_expr_list_2d->getNumInits(); ++j) {
          LLVM_DEBUG(llvm::dbgs()
                         << "Iterate over 2d init lists: " << j << "\n";);
          clang::Expr *iexpr_2d{iexpr_2d_set[j]};

          /// Get the constructor argument.
          // Unwrap CXXConstructExpr
          clang::CXXConstructExpr *peel{
              clang::dyn_cast<clang::CXXConstructExpr>(iexpr_2d)};
          clang::CXXConstructExpr *cexpr{nullptr};
          while (peel) {
            cexpr = peel;
            peel = clang::dyn_cast<clang::CXXConstructExpr>(
                peel->getArg(0)->IgnoreImplicit());
          }
          llvm::dbgs() << "unwrap 2d dump\n";
          //cexpr->dump();

          if (cexpr) {  // auto cexpr =
                        // clang::dyn_cast<clang::CXXConstructExpr>(iexpr_2d)) {
            llvm::dbgs() << "0 arg dump\n";
            clang::StringLiteral *slit{clang::dyn_cast<clang::StringLiteral>(
                cexpr->getArg(0)->IgnoreImpCasts())};
            LLVM_DEBUG(llvm::dbgs()
                           << "Argument 2d: [" << i << ", " << j << "] "
                           << slit->getString().str() << " \n";);
            indices.insert(IndexPairType(slit->getString().str(),
                                         std::make_tuple(i, j, 0)));
          }

          /// Level 3
          if (auto init_expr_list_3d =
                  clang::dyn_cast<clang::InitListExpr>(iexpr_2d)) {
            clang::Expr **iexpr_3d_set{init_expr_list_3d->getInits()};
            init_expr_list_3d->dump();
            for (std::size_t k{0}; k < init_expr_list_3d->getNumInits(); ++k) {
              LLVM_DEBUG(llvm::dbgs()
                             << "Iterate over 3d init lists: " << k << "\n";);
              clang::Expr *iexpr_3d{iexpr_3d_set[k]};
              iexpr_3d->dump();

              clang::CXXConstructExpr *peel{
                  clang::dyn_cast<clang::CXXConstructExpr>(iexpr_3d)};
              clang::CXXConstructExpr *cexpr{nullptr};
              while (peel) {
                cexpr = peel;
                peel = clang::dyn_cast<clang::CXXConstructExpr>(
                    peel->getArg(0)->IgnoreImplicit());
              }
              llvm::dbgs() << "unwrap 3d dump\n";
              cexpr->dump();

              if (cexpr) {
                // auto cexpr =
                      // clang::dyn_cast<clang::CXXConstructExpr>(iexpr_3d)) {
                // clang::CXXConstructExpr *nested_cexpr{
                    // clang::dyn_cast<clang::CXXConstructExpr>(
                        // cexpr->getArg(0)->IgnoreImplicit())};
                // auto cxxbindexpr{clang::dyn_cast<clang::CXXBindTemporaryExpr>(
                    // nested_cexpr->getArg(0)->IgnoreParenImpCasts())};
                // auto cxxctor{clang::dyn_cast<clang::CXXConstructExpr>(
                    // cxxbindexpr->getSubExpr()->IgnoreParenImpCasts())};
                clang::StringLiteral *slit{
                    clang::dyn_cast<clang::StringLiteral>(
                        cexpr->getArg(0)->IgnoreImpCasts())};
                // slit->dump();
                  LLVM_DEBUG(llvm::dbgs() << "Argument 3d: [" << i << ", " << j
                                          << ", " << k << "] "
                                          << slit->getString().str() << " \n";);
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

// ArraySizesType getConstantArraySizes(const clang::FieldDecl *fd) {
ArraySizesType getConstantArraySizes(const clang::ValueDecl *fd) {
  ArraySizesType sizes;

  clang::QualType field_type{fd->getType()};
  clang::QualType save_field_type = field_type;
  if (field_type->isReferenceType()) {
    // need a qualtype
    field_type = field_type->getPointeeType()
                     ->getLocallyUnqualifiedSingleStepDesugaredType();
  }

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

ArraySizesExprType getArraySubscripts(const clang::Expr *expr) {
  ArraySizesExprType subscripts;

  /// Check if it is an ArraySubscriptExpr
  auto arr_sub_expr{clang::dyn_cast<clang::ArraySubscriptExpr>(expr)};
  while (arr_sub_expr != nullptr) {
    arr_sub_expr->getIdx()->dump();

    /// The index could be an integer or a variable name a[x].
    /// The integer a[4] is represented as an IntegerLiteral, and the variable
    /// name uses a DeclRefExpr.

    auto int_lit{
        clang::dyn_cast<clang::IntegerLiteral>(arr_sub_expr->getIdx())};
    auto decl_ref_expr{clang::dyn_cast<clang::DeclRefExpr>(
        arr_sub_expr->getIdx()->IgnoreImpCasts())};

    if (int_lit) {
      // llvm::outs() << "SUBSCRIPT: " << int_lit->getValue() << "\n";
      // subscripts.insert(subscripts.begin(), int_lit->getValue());
      subscripts.insert(subscripts.begin(), arr_sub_expr->getIdx());
    }

    if (decl_ref_expr) {
      LLVM_DEBUG(llvm::dbgs()
                     << "SUBSCRIPT: "
                     << decl_ref_expr->getNameInfo().getName().getAsString()
                     << "\n";);
      subscripts.insert(subscripts.begin(), decl_ref_expr);
      /// TODO: Need to insert into subscripts, but it should really be Expr
      /// that is the value element. So, change subscripts to hold that, and
      /// then use it to hold both IntegerLiteral and DeclRefExpr.
    }

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
      return name_me;
    }
    arr_sub_expr = nested_sub_expr;
  }
  return nullptr;
}

}  // namespace array_type

}  // namespace utils
}  // namespace sc_ast_matchers
