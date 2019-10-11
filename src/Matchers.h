#ifndef _MATCHERS_HPP_
#define _MATCHERS_HPP_ 
#include <tuple>
#include <vector>
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang;
using namespace clang::ast_matchers;


// Static to make sure multiple definitions are not resulted. 
static DeclarationMatcher matchModuleDeclarations = cxxRecordDecl(isDerivedFrom(hasName("::sc_core::sc_module"))).bind("sc_module");
class ModuleDeclarationMatcher : public MatchFinder::MatchCallback {
  public:
    typedef std::vector< std::tuple<std::string, CXXRecordDecl*> > ModuleDeclarationTuple;

  public:
    virtual void run( const MatchFinder::MatchResult& result );
    const ModuleDeclarationTuple& getFoundModuleDeclarations() const;
    void dump();
  private:
    ModuleDeclarationTuple foundDeclarations_;
};

#endif

