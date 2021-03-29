#include "CXXRecordDeclUtils.h"
#include "ArrayTypeUtils.h"

#include "llvm/Support/Debug.h"
#include <queue>

namespace sc_ast_matchers {
namespace utils {
namespace cxx_construct_decl_utils {

using namespace utils::array_type;

std::vector<const clang::CXXRecordDecl *> getAllBaseClasses(
    const clang::CXXRecordDecl *decl) {
  /// Go through all the base classes, and its base classes and collect them
  /// together. This will be used to flatten all the member fields

  std::queue<const clang::CXXRecordDecl *> decl_queue;
  std::vector<const clang::CXXRecordDecl *> bases;

  decl_queue.push(decl);

  while (!decl_queue.empty()) {
    const clang::CXXRecordDecl *top_decl{decl_queue.front()};
    decl_queue.pop();

    auto name{top_decl->getNameAsString()};

    llvm::dbgs() << "Processing base named: " << name << "\n";
    if ((name != "sc_object") && (name != "sc_process_host") &&
        (name != "sc_module")) {
      llvm::dbgs() << "+ Insert into bases\n";
      bases.push_back(top_decl);
    }

    for (auto &base : top_decl->bases()) {
      const clang::CXXRecordDecl *base_decl{
          base.getType().getTypePtr()->getAsCXXRecordDecl()};

      decl_queue.push(base_decl);
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

  clang::StringLiteral *str_lit{ llvm::dyn_cast<clang::StringLiteral>(str_expr) };
  if (str_lit) {// = llvm::dyn_cast<clang::StringLiteral>(str_expr)) {
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
    auto decl{fd->getType().getTypePtr()->getUnqualifiedDesugaredType()->getAsCXXRecordDecl()};

    if (decl) {
      llvm::dbgs() << "decl: " << decl->getNameAsString() << "\n";
      for (const auto &base : decl->bases()) {
        clang::CXXRecordDecl *base_decl{
            base.getType().getTypePtr()->getUnqualifiedDesugaredType()->getAsCXXRecordDecl()};
        if (base_decl) {
          llvm::dbgs() << "base decl: " << base_decl->getNameAsString() << "\n";

          if (base_decl->getNameAsString() == "sc_module") {
            llvm::dbgs() << "Module class\n";
            module_info.push_back(std::make_tuple(fd, str_lit->getString().str(), init));
          }
        }
      }
    }
  }
  return module_info;
}

}  // namespace cxx_construct_decl_utils
}  // namespace utils
}  // namespace sc_ast_matchers
