#ifndef _MATCHERS_HPP_
#define _MATCHERS_HPP_ 
#include <tuple>
#include <vector>
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang;
using namespace clang::ast_matchers;


namespace sc_ast_matchers {
// Static to make sure multiple definitions are not resulted. 

class ModuleDeclarationMatcher : public MatchFinder::MatchCallback {
//
  public:
    typedef std::vector< std::tuple<std::string, CXXRecordDecl*> > ModuleDeclarationTuple;

  public:
    void registerMatchers( MatchFinder &finder );
    virtual void run( const MatchFinder::MatchResult &result );
    const ModuleDeclarationTuple& getFoundModuleDeclarations() const;
    void dump();
  private:
    ModuleDeclarationTuple found_declarations_;
};

}; // namespace sc_ast_matchers
#endif

