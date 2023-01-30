#include "CallExprUtils.h"

#include "clang/AST/Type.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/ExprCXX.h"
#include "llvm/Support/Debug.h"

namespace sc_ast_matchers {
namespace utils {

using namespace clang;

void collect_sugar(const Type *type,
                   std::vector<clang::Type *> &unwrapped_types) {
  const Type *desugared_final{type->getUnqualifiedDesugaredType()};
  Type *curr_type{const_cast<Type *>(type)};
  unwrapped_types.push_back(const_cast<Type *>(desugared_final));
  while (curr_type != desugared_final) {
    unwrapped_types.push_back(curr_type);
    curr_type = const_cast<Type *>(
        curr_type->getLocallyUnqualifiedSingleStepDesugaredType().getTypePtr());
  }
}

bool isInNamespace(const clang::ValueDecl *fd,
                   const std::vector<llvm::StringRef> &names) {
  llvm::dbgs() << "isInNamespace with ValueDecl\n";


  if (!fd) {
    return false;
  }

  //fd->dump();
  return isInNamespace(fd->getType().getTypePtr(), names);

  /*
   *  When is this used?  Must have comments next time.
   */
  // if (auto dc = dyn_cast<DeclContext>(fd)) {
        // llvm::dbgs() << "DeclContext\n";
    // auto dcc = dc->getLexicalParent();
//
    // if (dcc->isNamespace()) {
      // if (const auto *nd = llvm::dyn_cast<clang::NamespaceDecl>(dcc)) {
        // llvm::dbgs() << "Namespace\n";
        // std::vector<llvm::StringRef> names{"sc_dt"};
        // auto iinfo = nd->getIdentifier();
        // llvm::dbgs() << "@@@@ name " << nd->getName() << "\n";
        // for (const auto name : names) {
          // if (iinfo->isStr(name)) {
            // return true;
          // }
        // }
        // return false;
      // }
    // }
  // }
  return false;
}

bool isInNamespace(const clang::Type *tp,
                   const std::vector<llvm::StringRef> &names) {
  if (!tp) {
    return false;
  }

  /// Peel off every type and then check that each type (including typedef) is
  /// of a certain namespace or not.
  std::vector<clang::Type *> unwrapped_types{};
  collect_sugar(tp, unwrapped_types);

  for (auto tap : unwrapped_types) {
    if (tap->isBuiltinType()) {
      llvm::dbgs() << "isBuiltinType\n";
      return false;
    }

    DeclContext *dc{nullptr};
    if (tap->isArrayType()) {
      auto cat1 = dyn_cast<ArrayType>(tap);
      const Type *tp = cat1->getElementType().getTypePtr();

      // Unwrap array element type.
      Type *unwrap_tp{nullptr};
      while (tp && tp != unwrap_tp) {
        if (auto cat2 = dyn_cast<ArrayType>(tp)) {
          unwrap_tp = const_cast<Type *>(tp);
          tp = cat2->getElementType().getTypePtr();
        } else {
          unwrap_tp = const_cast<Type *>(tp);
        }
      }

      tap = unwrap_tp;
    }

    // tap->dump();
    if (tap->isRecordType()) {
      llvm::dbgs() << "isRecordType\n";
      const RecordDecl *rdecl = tap->getAsRecordDecl();
      dc = const_cast<clang::DeclContext *>(rdecl->getLexicalParent());
    }

    if (dc && dc->isNamespace()) {
      llvm::dbgs() << "isNamespace \n";
      if (const auto *nd = llvm::dyn_cast<clang::NamespaceDecl>(dc)) {
        auto iinfo = nd->getIdentifier();
        // llvm::dbgs() << "@@ name is " << iinfo->getName() << " for ";
        for (const auto name : names) {
          if (iinfo->isStr(name)) {
            return true;
          }
        }
        return false;
      }
    }
  }

  return false;
}

bool matchNames(StringRef str, const std::vector<llvm::StringRef> &names) {
  for (const auto name : names) {
    if (str == name) {
      return true;
    }
  }

  return false;
}

// llvm::StringRef getClassNameFromDecl(const clang::FunctionDecl *decl) {
//   if (!decl) return StringRef{""};

//   const clang::DeclContext* dc{ decl->getLexicalParent() };
//   if (dc && dc->isNamespace() ) {
//     if (const auto *nd = llvm::dyn_cast<clang::NamespaceDecl>(dc)) {
//       IdentifierInfo *iinfo = nd->getIdentifier();
//       llvm::dbgs() << "@@ name is " << iinfo->getName() << " for ";
//       return iinfo->getName();
//     }
//   }

//   return StringRef{""};
// }

// llvm::StringRef getClassNameFromDecl(const clang::CXXRecordDecl *decl) {
//   if (!decl) return StringRef{""};

//   const clang::DeclContext* dc{ decl->getLexicalParent() };
//   if (dc && dc->isNamespace() ) {
//     if (const auto *nd = llvm::dyn_cast<clang::NamespaceDecl>(dc)) {
//       IdentifierInfo *iinfo = nd->getIdentifier();
//       llvm::dbgs() << "@@ name is " << iinfo->getName() << " for ";
//       return iinfo->getName();
//     }
//   }

//   return StringRef{""};
// }

void dumpExprName(const Expr *expr) {
  if (auto cexpr = dyn_cast<CXXOperatorCallExpr>(expr)) {
    if (const FunctionDecl *fd = cexpr->getDirectCallee()) {
      auto decl_info{fd->getNameInfo()};
      llvm::dbgs() << decl_info.getName().getAsString();
    }
  }

  if (auto cexpr = dyn_cast<CallExpr>(expr)) {
    if (const FunctionDecl *fd = cexpr->getDirectCallee()) {
      auto decl_info{fd->getNameInfo()};
      if (decl_info.getName().isIdentifier()) {
        llvm::dbgs() << decl_info.getName().getAsString();
      }
    }
  }
}

bool isInNamespace(const Expr *expr, ASTContext &context,
                   const std::vector<llvm::StringRef> &names) {
  // std::vector<llvm::StringRef> func_names{"range", "or_reduce" };
  llvm::dbgs() << "2. Expr isNamespace\n";

  // do not handle operator=
  // if (auto cexpr = dyn_cast <CXXOperatorCallExpr>(expr)) {
  // return false;
  // }
  //
  if (auto cexpr = dyn_cast<CallExpr>(expr)) {
    MatchFinder finder{};
    NamespaceMatcher ns_matcher{};
    ns_matcher.registerMatchers(finder);
    finder.match(*expr, context);
    llvm::dbgs() << "@@@ MATCHNAMES " << ns_matcher.getNamespaceName() << "  "
                 << ns_matcher.getFunctionName() << "\n";
    return matchNames(ns_matcher.getNamespaceName(), names);
  }

  /*
  // wait() call
// Expr is a MemberExpr ()
if (auto me_expr = dyn_cast<MemberExpr>(expr) ) {
  if (auto mdecl = dyn_cast<CXXMethodDecl>(me_expr->getMemberDecl()) ) {
    auto cxxdecl{ mdecl->getParent() };
    llvm::dbgs() << "@@@@@@@@@@@@ MemberExpr\n";
    cxxdecl->dump();
    StringRef ns_name{ getClassNameFromDecl(cxxdecl) };
    llvm::dbgs() << "@@@@ name is " << cxxdecl->getNameAsString() << " and ref
is " << ns_name << "\n"; return matchNames(ns_name, names);
  }
}
*/

  /*
  if (auto cexpr = dyn_cast<CallExpr>(expr)) {
    llvm::dbgs() << "Decl for callexpr\n";
    cexpr->getCallee()->dump();
    // const Decl *d = cexpr->getCalleeDecl();
    // const DeclContext* dc = d->getLexicalParent();
    // d->dump();
      // StringRef ns_name{ getClassNameFromDecl(d) };
      // llvm::dbgs() << "NS NAME = " << ns_name << "\n";

      if (const FunctionDecl* fd = cexpr->getDirectCallee()) {
        StringRef ns_name{ getClassNameFromDecl(fd) };
        return matchNames(ns_name, names);
      }

    //return isInNamespace(cexpr->getType().getTypePtr(), names);
  }
  */

  /*
  if (auto dexpr = dyn_cast<DeclRefExpr>(expr)) {
    llvm::dbgs() << "@@@@ in DeclRefExpr\n";
    dexpr->dump();
    dexpr->getDecl()->dump();
    return isInNamespace(dexpr->getDecl(), names);
  }

  */
  return false;
}

//
// bool isInNamespace(const CallExpr *cexpr,
// const std::vector<llvm::StringRef> &names) {
// llvm::dbgs() << "@@@ callexpr version of isNS\n";
// if (!cexpr) {
// return false;
// }
// return isInNamespace(cexpr->getType().getTypePtr(), names);
// }

bool isInNamespace(const CallExpr *cexpr, llvm::StringRef name) {
  if (!cexpr) {
    return false;
  }
  std::vector<llvm::StringRef> names{name};
  return isInNamespace(cexpr->getType().getTypePtr(), names);
}
}  // namespace utils
}  // namespace sc_ast_matchers
