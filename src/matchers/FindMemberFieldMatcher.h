#ifndef _FIND_MEMBER_FIELD_MATCHER_H_
#define _FIND_MEMBER_FIELD_MATCHER_H_

#include <vector>

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchersInternal.h"
#include "clang/ASTMatchers/ASTMatchersMacros.h"

using namespace clang::ast_matchers;
using namespace clang;

namespace clang {
class Type;
}

namespace sc_ast_matchers {

class FindMemberFieldMatcher : public MatchFinder::MatchCallback {
 private:
  std::vector<clang::FieldDecl *> found_fields_;

 public:
  const std::vector<clang::FieldDecl *> &getFieldDecls() {
    return found_fields_;
  }

  void registerMatchers(MatchFinder &finder) {
    auto member_fields_match =
        recordDecl(forEach(fieldDecl().bind("field_decl")));

    finder.addMatcher(member_fields_match, this);
  }

  virtual void run(const MatchFinder::MatchResult &result) {
    auto fd = const_cast<FieldDecl *>(
        result.Nodes.getNodeAs<FieldDecl>("field_decl"));

    found_fields_.push_back(fd);
  }

  void dump() {
    for (auto const &field : found_fields_) {
      field->dump();
    }
  }
};

};  // namespace sc_ast_matchers

#endif
