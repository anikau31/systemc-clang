#ifndef _MATCHERS_HPP_
#define _MATCHERS_HPP_ 
#include <tuple>
#include <vector>
#include <map>
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "FindTemplateTypes.h"
#include "PortDecl.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace scpar;

namespace sc_ast_matchers {
// Static to make sure multiple definitions are not resulted. 

class ModuleDeclarationMatcher : public MatchFinder::MatchCallback {
//
  public:
    typedef std::vector< std::tuple<std::string, CXXRecordDecl*> > ModuleDeclarationTuple;
    typedef std::vector< std::tuple<std::string, PortDecl*> > PortType;

  public:
    void registerMatchers( MatchFinder &finder );
    virtual void run( const MatchFinder::MatchResult &result );
    const ModuleDeclarationTuple& getFoundModuleDeclarations() const;
    const PortType& getFields( const std::string & port_type );

    void dump();
  private:
    ModuleDeclarationTuple found_declarations_;
    PortType clock_ports_;
    PortType in_ports_;
    PortType out_ports_;
    PortType inout_ports_;
    PortType other_fields_;
    PortType signal_fields_;

};

}; // namespace sc_ast_matchers
#endif

