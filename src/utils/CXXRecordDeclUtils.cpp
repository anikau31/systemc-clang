#include "CXXRecordDeclUtils.h"
#include "ArrayTypeUtils.h"

#include "clang/AST/DeclCXX.h"
#include "clang/AST/ExprCXX.h"
#include "llvm/Support/Debug.h"
#include "llvm/ADT/StringSet.h"
#include <queue>

namespace sc_ast_matchers {
namespace utils {


bool isCXXMemberCallExprSystemCCall(const clang::CXXMemberCallExpr *mce) {
  if (!mce) {
    return false;
  }
  llvm::dbgs() << "isCXXMemberCallExprSystemCCall\n";

  if (auto mdecl = mce->getMethodDecl()) {
    if (auto rdecl = mdecl->getParent()) {
      auto base_names{getAllBaseClassNames(rdecl)};

      for (const auto &decl : base_names) {
        llvm::dbgs() << "base is called " << decl->getNameAsString() << "\n";
        auto name{ decl->getNameAsString()};
        if (name == "sc_object" || (name == "sc_simcontext")) {
          llvm::dbgs() << "() Member call is sc_object\n";
          return true;
        }
      }
    }
  }

  return false;
}


using namespace utils::array_type;

std::vector<const clang::CXXRecordDecl *> getAllBaseClassNames(
    const clang::CXXRecordDecl *decl) {
  /// Go through all the base classes, and its base classes and collect them
  /// together. This will be used to flatten all the member fields

  std::queue<const clang::CXXRecordDecl *> decl_queue;
  std::vector<const clang::CXXRecordDecl *> bases;
  llvm::StringSet<> bases_set;

  // decl_queue.push(decl);
  const clang::CXXRecordDecl *top_decl{decl};

  while (top_decl) {
    auto name{top_decl->getNameAsString()};

    llvm::dbgs() << "Processing base named: " << name << "\n";

    bases.push_back(top_decl);
    bases_set.insert(top_decl->getName());
    /// Go through all the bases of the base declaration.
    for (auto &base : top_decl->bases()) {
      const clang::CXXRecordDecl *base_decl{
          base.getType().getTypePtr()->getAsCXXRecordDecl()};

      decl_queue.push(base_decl);
    }

    if (!decl_queue.empty()) {
      top_decl = decl_queue.front();
      decl_queue.pop();
    } else {
      top_decl = nullptr;
    }
  }

  /// Print all the base classes retrieved.
  llvm::dbgs() << "Bases collected: ";
  for (auto const &base : bases) {
    llvm::dbgs() << base->getNameAsString() << "  ";
  }
  llvm::dbgs() << "\n";

  return bases;
}

std::vector<const clang::CXXRecordDecl *> getAllBaseClasses(
    const clang::CXXRecordDecl *decl) {
  /// Go through all the base classes, and its base classes and collect them
  /// together. This will be used to flatten all the member fields

  std::queue<const clang::CXXRecordDecl *> decl_queue;
  std::vector<const clang::CXXRecordDecl *> bases;

  // decl_queue.push(decl);
  const clang::CXXRecordDecl *top_decl{decl};

  while (top_decl) {
    auto name{top_decl->getNameAsString()};

    llvm::dbgs() << "Processing base named: " << name << "\n";

    /// Do not insert into bases the decl class.
    if ((top_decl != decl) && (name != "sc_object") &&
        (name != "sc_process_host") && (name != "sc_module")) {
      llvm::dbgs() << "+ Insert into bases\n";
      bases.push_back(top_decl);
    }

    /// Go through all the bases of the base declaration.
    for (auto &base : top_decl->bases()) {
      const clang::CXXRecordDecl *base_decl{
          base.getType().getTypePtr()->getAsCXXRecordDecl()};

      decl_queue.push(base_decl);
    }

    if (!decl_queue.empty()) {
      top_decl = decl_queue.front();
      decl_queue.pop();
    } else {
      top_decl = nullptr;
    }
  }

  /// Print all the base classes retrieved.
  llvm::dbgs() << "Bases collected: ";
  for (auto const &base : bases) {
    llvm::dbgs() << base->getNameAsString() << "  ";
  }
  llvm::dbgs() << "\n";

  return bases;
}

std::vector<ModuleInitializerTupleType> getModuleInitializerNames(
    const clang::CXXCtorInitializer *init) {
  // const clang::CXXConstructorDecl *ctor_decl) {

  std::vector<ModuleInitializerTupleType> module_info;

  /// Iterate through all the initializers.
  // for (auto const &init : ctor_decl->inits()) {

  /// Retrieve the first argument
  clang::Expr *str_expr{init->getInit()->IgnoreUnlessSpelledInSource()};
  llvm::dbgs() << "CtorInit dump\n";
  str_expr->dump();

  clang::StringLiteral *str_lit{llvm::dyn_cast<clang::StringLiteral>(str_expr)};
  if (str_lit) {  // = llvm::dyn_cast<clang::StringLiteral>(str_expr)) {
    llvm::dbgs() << "Get first arg: " << str_lit->getString() << "\n";
  }

  /// Get the FieldDecl corresponding to the constructor initializer.
  clang::FieldDecl *fd{init->getMember()};
  if (fd) {
    auto name{fd->getType().getAsString()};
    llvm::dbgs() << "\n   *************** Initializer names ****** : " << name
                 << "\n";
    fd->dump();
    /// Check the type of the FieldDecl.
    auto decl{fd->getType()
                  .getTypePtr()
                  ->getUnqualifiedDesugaredType()
                  ->getAsCXXRecordDecl()};

    if (decl) {
      llvm::dbgs() << "decl: " << decl->getNameAsString() << "\n";
      for (const auto &base : decl->bases()) {
        clang::CXXRecordDecl *base_decl{base.getType()
                                            .getTypePtr()
                                            ->getUnqualifiedDesugaredType()
                                            ->getAsCXXRecordDecl()};
        if (base_decl) {
          llvm::dbgs() << "base decl: " << base_decl->getNameAsString() << "\n";

          if (base_decl->getNameAsString() == "sc_module") {
            llvm::dbgs() << "Module class\n";
            module_info.push_back(
                std::make_tuple(fd, str_lit->getString().str(), init));
          }
        }
      }
    }
  }
  return module_info;
}

}  // namespace utils
}  // namespace sc_ast_matchers
