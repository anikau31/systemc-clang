#ifndef _SENSITIVITY_MATCHER_H_
#define _SENSITIVITY_MATCHER_H_

#include <vector>

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchersInternal.h"
#include "clang/ASTMatchers/ASTMatchersMacros.h"

using namespace clang;
using namespace clang::ast_matchers;

namespace sc_ast_matchers {

///////////////////////////////////////////////////////////////////////////////
//
// Class SensitivityMatcher
//
//
///////////////////////////////////////////////////////////////////////////////
class SensitivityMatcher : public MatchFinder::MatchCallback {
 public:

 private:
 public:

    void registerMatchers(MatchFinder &finder) {
      /* clang-format off */
      /* clang-format on */

      auto match = cxxConstructorDecl();
    // Add the two matchers.
    finder.addMatcher(match, this);
  }

  // This is the callback function whenever there is a match.
  virtual void run(const MatchFinder::MatchResult &result) {
    llvm::outs()
        << " ===================== SENSITIVITY MATCHER ==================== \n";
//    auto instance_fd = const_cast<FieldDecl *>(
 //       result.Nodes.getNodeAs<FieldDecl>("instances_in_fielddecl"));
  }

  void dump() {
  }
};
};  // namespace sc_ast_matchers
#endif
