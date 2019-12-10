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

  // InstanceMatcher
  class InstanceMatcher : public MatchFinder::MatchCallback {
    public:
      void registerMatchers(MatchFinder &finder) {
        //clang-format off 
        auto match_instances = 
          fieldDecl( 
              hasType( 
                cxxRecordDecl(isDerivedFrom(hasName("::sc_core::sc_module")))
                )
              ).bind("instances_in_fielddecl");

        auto match_instances_vars = 
          varDecl( 
              hasType( 
                cxxRecordDecl(isDerivedFrom(hasName("::sc_core::sc_module")))
                )
              ).bind("instances_in_vardecl");
        //clang-format on

        finder.addMatcher(match_instances, this);
        finder.addMatcher(match_instances_vars, this);
      }

      virtual void run(const MatchFinder::MatchResult &result) {
        /*
           if (auto field_decl = const_cast<FieldDecl *>(
           result.Nodes.getNodeAs<FieldDecl>("sc_in"))) {
           std::string name{field_decl->getIdentifier()->getNameStart()};
           input_port_names.push_back(name);
           }
           */
        cout << " Found an instance: "<< endl;
      }

 public:
  std::vector<std::string> input_port_names;
};


// FieldMatcher class
class FieldMatcher : public MatchFinder::MatchCallback {
 public:
  void registerMatchers(MatchFinder &finder) {
    auto match_module_decls = cxxRecordDecl(
        // isExpansionInMainFile(),
        isDerivedFrom(hasName("::sc_core::sc_module")),
        unless(isDerivedFrom(matchesName("sc_event_queue"))));

    auto match_in_ports = cxxRecordDecl(forEach(
        fieldDecl(hasType(cxxRecordDecl(hasName("sc_in")))).bind("sc_in")));

    //clang-format on

    finder.addMatcher(match_in_ports, this);
  }

  virtual void run(const MatchFinder::MatchResult &result) {
    cout << " Trying to find sc_in. " << endl;
    if (auto field_decl = const_cast<FieldDecl *>(
            result.Nodes.getNodeAs<FieldDecl>("sc_in"))) {
      std::string name{field_decl->getIdentifier()->getNameStart()};
      cout << " Found an sc_in : " << name << endl;
      input_port_names.push_back(name);
    }
  }

 public:
  std::vector<std::string> input_port_names;
};



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

