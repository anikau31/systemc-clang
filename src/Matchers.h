#ifndef _MATCHERS_HPP_
#define _MATCHERS_HPP_
#include <map>
#include <tuple>
#include <vector>
#include "FindTemplateTypes.h"
#include "ModuleDecl.h"
#include "PortDecl.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace scpar;

namespace sc_ast_matchers {

//
// InstanceMatcher
//
//
class InstanceMatcher : public MatchFinder::MatchCallback {
 public:
  typedef std::tuple<std::string, Decl *> InstanceDeclType;
  typedef std::vector<InstanceDeclType> InstanceDeclarationsType;

  typedef std::tuple<std::string, FieldDecl *> InstanceFieldType;
  typedef std::tuple<std::string, VarDecl *> InstanceVarType;
  typedef std::vector<std::tuple<std::string, FieldDecl *> > instance_fields;
  typedef std::vector<std::tuple<std::string, VarDecl *> > instance_vars;

 public:
  // Finds the instance with the same type as the argument.
  bool findInstance(CXXRecordDecl *decl) {
    // First check in the instance_fields.
    // Check to see if the pointer to the type is the same as the sc_module
    // type.

    auto found_it = std::find_if(
        instances_.begin(), instances_.end(),
        [&decl](const InstanceDeclType &element) {
          // Get the CXXRecordDecl for the instance.
          // The instance is the second element in the tuple.
          auto p_field_var_decl{get<1>(element)};

          if (auto *p_field{dyn_cast<FieldDecl>(p_field_var_decl)}) {
            auto qtype{p_field->getType().getTypePtr()};
            if (qtype->isRecordType()) {
              if (auto dp = qtype->getAs<TemplateSpecializationType>()) {
                auto rt{dp->getAsCXXRecordDecl()};
                return (rt == decl);
              }
            }
          } else {
            // VarDecl
            auto p_var{dyn_cast<VarDecl>(p_field_var_decl)};
            auto qtype{p_var->getType().getTypePtr()};
            if (qtype->isRecordType()) {
              auto rt{qtype->getAsCXXRecordDecl()};
              return (rt == decl);
            }
          }
        });

    if (found_it != instances_.end()) {
      std::cout << "FOUND AN FIELD instance: " << std::endl;
      return true;
    }
    return false;
  }

  void registerMatchers(MatchFinder &finder) {
    /* clang-format off */
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
    /* clang-format on */

    finder.addMatcher(match_instances, this);
    finder.addMatcher(match_instances_vars, this);
  }

  virtual void run(const MatchFinder::MatchResult &result) {
    if (auto instance = const_cast<FieldDecl *>(
            result.Nodes.getNodeAs<FieldDecl>("instances_in_fielddecl"))) {
      std::string name{instance->getIdentifier()->getNameStart()};
      cout << "Found a member field instance: " << name << endl;

      instances_.push_back(std::make_tuple(name, instance));
    }

    if (auto instance = const_cast<VarDecl *>(
            result.Nodes.getNodeAs<VarDecl>("instances_in_vardecl"))) {
      std::string name{instance->getIdentifier()->getNameStart()};
      cout << "Found a member variable instance: " << name << endl;
      instances_.push_back(std::make_tuple(name, instance));
    }
  }

  void dump() {
    // Instances holds both FieldDecl and VarDecl as its base class Decl.
    for (const auto &i : instances_) {
      cout << "module declarations name: " << get<0>(i) << ", " << get<1>(i)
           << std::endl;

      auto p_field_var_decl{get<1>(i)};
      if (isa<FieldDecl>(p_field_var_decl)) {
        cout << " ==> FieldDecl\n";
      } else {
        cout << " ==> VarDecl\n";
      }
    }
  }

 private:
  InstanceDeclarationsType instances_;
};

// FieldMatcher class
class FieldMatcher : public MatchFinder::MatchCallback {
 public:
  void registerMatchers(MatchFinder &finder) {
    /* clang-format off */
    auto match_module_decls = cxxRecordDecl(
        // isExpansionInMainFile(),
        isDerivedFrom(hasName("::sc_core::sc_module")),
        unless(isDerivedFrom(matchesName("sc_event_queue"))));

    auto match_in_ports = cxxRecordDecl(forEach(
        fieldDecl(hasType(cxxRecordDecl(hasName("sc_in")))).bind("sc_in")));

    /* clang-format off */

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

//
// Class ModuleDeclarationMatcher
//
//
class ModuleDeclarationMatcher : public MatchFinder::MatchCallback {
  //
 public:
  typedef std::vector<std::tuple<std::string, CXXRecordDecl *> >
      ModuleDeclarationType;
  typedef std::vector<std::tuple<std::string, PortDecl *> > PortType;

 private:
  // Template functions.
  template <typename T>
     void insert_port( PortType & port, T *decl );

 public:
  void registerMatchers(MatchFinder &finder);
  virtual void run(const MatchFinder::MatchResult &result);
  const ModuleDeclarationType &getFoundModuleDeclarations() const;
  const PortType &getFields(const std::string &port_type);

  void pruneMatches();
  void dump();

 private:
  ModuleDeclarationType found_declarations_;
  ModuleDeclarationType found_template_declarations_;
  ModuleDeclarationType pruned_declarations_;
  
  // Match nested instances
  InstanceMatcher  instance_matcher; 

  // Ports
  PortType clock_ports_;
  PortType in_ports_;
  PortType out_ports_;
  PortType inout_ports_;
  PortType other_fields_;
  PortType signal_fields_;
};

};  // namespace sc_ast_matchers

#endif
