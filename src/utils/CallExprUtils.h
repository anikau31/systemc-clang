#ifndef _CALL_EXPR_UTILS_H_
#define _CALL_EXPR_UTILS_H_

#undef DEBUG_TYPE
#define DEBUG_TYPE "CallExprUtils"

#include "clang/AST/DeclCXX.h"
#include "clang/AST/ExprCXX.h"

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "llvm/ADT/StringRef.h"
#include <vector>

using namespace clang::ast_matchers;

/*
namespace clang {
class CallExpr;
class Type;
class FunctionDecl;
class ValueDecl;
class Expr;
class CXXRecordDecl;
class NamespaceDecl;

};  // namespace clang
*/

namespace sc_ast_matchers {
namespace utils {

template <typename T>
llvm::StringRef getClassNameFromDecl(T *decl) {
  llvm::StringRef empty{"<NULL>"};
  if (!decl) return empty;
  const clang::DeclContext *dc = decl->getLexicalParent();
  if (dc ) {
    llvm::dbgs() << "@@@@@ Enter IF \n";
    if (const auto *nd = llvm::dyn_cast<clang::NamespaceDecl>(dc)) {
      clang::IdentifierInfo *iinfo = nd->getIdentifier();
      llvm::dbgs() << "@@ name is " << iinfo->getName() << " for ";
      return iinfo->getName();
    }
  } else {
    llvm::dbgs() << "NOT NS\n";
  }

  return empty;
}

class NamespaceMatcher : public MatchFinder::MatchCallback {
 private:
  llvm::StringRef ns_name;
  std::string func_name;

 public:
  llvm::StringRef getNamespaceName() { return ns_name; }
  std::string getFunctionName() { return func_name; }

  void registerMatchers(MatchFinder &finder) {
    ns_name = "<NULL>";
    func_name = "<NULL>";
    // clang-format off
    // auto member_matcher = cxxMemberCallExpr(callee(functionDecl().bind("ce_decl"))).bind("expr");
    auto call_matcher = callExpr(callee(functionDecl().bind("ce_decl"))).bind("expr");
    // clang-format on

    // finder.addMatcher(member_matcher, this);
    finder.addMatcher(call_matcher, this);
  }

  virtual void run(const MatchFinder::MatchResult &result) {
    auto expr =
        const_cast<clang::Expr *>(result.Nodes.getNodeAs<clang::Expr>("expr"));
    auto ce_decl = const_cast<clang::FunctionDecl *>(
        result.Nodes.getNodeAs<clang::FunctionDecl>("ce_decl"));

    llvm::dbgs() << "@@@@@@## callExpr \n";

    if (expr && ce_decl){
      ce_decl->dump();
      // wait() statements
        if (auto mdecl = llvm::dyn_cast<clang::CXXMethodDecl>(ce_decl)) {
          ns_name = getClassNameFromDecl(ce_decl->getParent());
          llvm::dbgs() << "@@@@@ MDECL FUNC NAME " << mdecl->getNameAsString() << "\n";
          func_name = mdecl->getNameAsString();
        } 
        else {
          // sc_min, sc_max, sc_abs
          ns_name = getClassNameFromDecl(ce_decl);
          func_name = ce_decl->getNameAsString();
        }
      llvm::dbgs() << "@@@@@@ NAME " << ns_name << " func_name " << func_name << "\n";
    }
  }

  void dump() {}
};

void dumpExprName(const clang::Expr *expr);

/*
 * \brief Check if a Type is declared within a given namespace.
 */
bool isInNamespace(const clang::Type *tp,
                   const std::vector<llvm::StringRef> &names);
bool isInNamespace(const clang::Expr *expr,
                   const std::vector<llvm::StringRef> &names);

bool isInNamespace(const clang::ValueDecl *fd,
                   const std::vector<llvm::StringRef> &names);

// bool isInNamespace(const clang::CallExpr *cexpr, const
// std::vector<llvm::StringRef>& names);
void collect_sugar(const clang::Type *type,
                   std::vector<clang::Type *> &unwrapped_types);

// llvm::StringRef getClassNameFromDecl( const clang::CXXRecordDecl* decl );
// llvm::StringRef getClassNameFromDecl( const clang::FunctionDecl* decl );

bool matchNames(llvm::StringRef str, const std::vector<llvm::StringRef> &names);
/*
 * \brief Check if a CallExpr is declared within a given namespace.
 */
bool isInNamespace(const clang::CallExpr *cexpr, llvm::StringRef name);

}  // namespace utils

}  // namespace sc_ast_matchers

#endif
