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

///////////////////////////////////////////////////////////////////////////////
//
// Class InstanceMatcher
//
//
///////////////////////////////////////////////////////////////////////////////
class InstanceMatcher : public MatchFinder::MatchCallback {
 public:
  typedef std::tuple<std::string, Decl *> InstanceDeclType;
  typedef std::vector<InstanceDeclType> InstanceDeclarationsType;

 private:
  InstanceDeclarationsType instances_;

 public:
  // Finds the instance with the same type as the argument.
  // Pass by reference to the instance.
  bool findInstance(CXXRecordDecl *decl, InstanceDeclType &instance) {
    // First check in the instance_fields.
    // Check to see if the pointer to the type is the same as the sc_module
    // type.

    InstanceDeclarationsType::iterator found_it = std::find_if(
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
            // This is a VarDecl instance.
            auto p_var{dyn_cast<VarDecl>(p_field_var_decl)};
            auto qtype{p_var->getType().getTypePtr()};
            if (qtype->isRecordType()) {
              auto rt{qtype->getAsCXXRecordDecl()};
              return (rt == decl);
            }
          }
        });

    if (found_it != instances_.end()) {
      // cout << "FOUND AN FIELD instance: " << get<0>(*found_it) << ", "
      // << get<1>(*found_it) << endl;
      // This is an odd way to set tuples.  Perhaps replace with a nicer
      // interface.
      get<0>(instance) = get<0>(*found_it);
      get<1>(instance) = get<1>(*found_it);

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
                ), 
              hasDescendant(cxxConstructExpr(hasArgument(0, stringLiteral().bind("constructor_arg"))).bind("constructor_expr"))
              ).bind("instances_in_vardecl");
    /* clang-format on */

    finder.addMatcher(match_instances, this);
    finder.addMatcher(match_instances_vars, this);
  }

  virtual void run(const MatchFinder::MatchResult &result) {
    if (auto instance = const_cast<FieldDecl *>(
            result.Nodes.getNodeAs<FieldDecl>("instances_in_fielddecl"))) {
      std::string name{instance->getIdentifier()->getNameStart()};
      cout << "@@ Found a member field instance: " << name << endl;

      instances_.push_back(std::make_tuple(name, instance));
    }

    if (auto instance = const_cast<VarDecl *>(
            result.Nodes.getNodeAs<VarDecl>("instances_in_vardecl"))) {
      std::string name{instance->getIdentifier()->getNameStart()};
      cout << "@@ Found a member variable instance: " << name << endl;

      if (auto instance_name = const_cast<CXXConstructExpr *>(
              result.Nodes.getNodeAs<CXXConstructExpr>("constructor_expr"))) {
        cout << "Found constructor expression argument: "
             << instance_name->getNumArgs() << "\n";
        auto first_arg{instance_name->getArg(0)};

        // Code to get the instance name
        clang::LangOptions LangOpts;
        LangOpts.CPlusPlus = true;
        clang::PrintingPolicy Policy(LangOpts);

        std::string s;
        llvm::raw_string_ostream sstream(s);
        first_arg->printPretty(sstream, 0, Policy);
        std::string strip_quote_name{sstream.str()};
        strip_quote_name.erase(
            std::remove(strip_quote_name.begin(), strip_quote_name.end(), '\"'),
            strip_quote_name.end());

        instances_.push_back(std::make_tuple(strip_quote_name, instance));
      }
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
};
///////////////////////////////////////////////////////////////////////////////
// Class PortMatcher 
//
//
///////////////////////////////////////////////////////////////////////////////

class PortMatcher : public MatchFinder::MatchCallback {
 public:
  typedef std::vector<std::tuple<std::string, PortDecl *> > PortType;

 private:
  PortType clock_ports_;
  PortType in_ports_;
  PortType out_ports_;
  PortType inout_ports_;
  PortType other_fields_;
  PortType signal_fields_;

 public:
  // AST matcher to detect field declarations
  auto makeFieldMatcher(const std::string &name) {
    /* clang-format off */
  return  cxxRecordDecl(
      isExpansionInMainFile(),
      isDerivedFrom(
        hasName("::sc_core::sc_module")
        ),
      forEach(
        fieldDecl(hasType(cxxRecordDecl(hasName(name)))).bind(name)
        )
      );
    /* clang-format on */
  }

  void printTemplateArguments(PortType &found_ports) {
    // Input ports
    for (const auto &i : found_ports) {
      llvm::outs() << "name: " << get<0>(i)
                   << ", FieldDecl*: " << get<1>(i)->getFieldDecl();
      get<1>(i)->getTemplateType()->printTemplateArguments(llvm::outs());
      llvm::outs() << "\n";
    }
  }

  auto parseTemplateType(const FieldDecl *fd) {
    //}, const ModuleDeclarationMatcher::PortType &found_ports ) {
    QualType qual_type{fd->getType()};
    const Type *type_ptr{qual_type.getTypePtr()};
    auto template_ptr{new FindTemplateTypes()};
    template_ptr->Enumerate(type_ptr);
    return template_ptr;
  }

  template <typename T>
  void insert_port(PortType &port, T *decl) {
    auto name{decl->getIdentifier()->getNameStart()};
    port.push_back(std::make_tuple(
        name, new PortDecl(name, decl, parseTemplateType(decl))));
  }

  template <typename NodeType>
  auto checkMatch(const std::string &name,
                  const MatchFinder::MatchResult &result) {
    return result.Nodes.getNodeAs<NodeType>(name);
  }

  void registerMatchers(MatchFinder &finder) {
    /* clang-format off */

    auto match_sc_in_clk = cxxRecordDecl( isDerivedFrom(hasName("sc_module")),
        forEach( 
          fieldDecl(
            hasType(
              namedDecl(
                hasName("sc_in_clk")
                )
              )
            ).bind("sc_in_clk")
          )
        );
    auto match_module_decls = 
      cxxRecordDecl(
          hasDefinition(), // There must be a definition.
          unless( isImplicit() ), // Templates generate implicit structs - so ignore.
          isDerivedFrom(
            hasName("::sc_core::sc_module") 
            ),
          unless(isDerivedFrom(matchesName("sc_event_queue")))
          );      
     
    auto match_non_sc_types = cxxRecordDecl(
        match_module_decls,
        forEach(
          fieldDecl(
            allOf(
            unless(hasType(cxxRecordDecl(matchesName("sc*")))),
              unless(hasType(namedDecl(hasName("sc_in_clk"))))
              )
            ).bind("other_fields")
          )
        );
    /* clang-format on */

    auto match_in_ports = makeFieldMatcher("sc_in");
    auto match_out_ports = makeFieldMatcher("sc_out");
    auto match_in_out_ports = makeFieldMatcher("sc_inout");
    auto match_internal_signal = makeFieldMatcher("sc_signal");

    finder.addMatcher(match_in_ports, this);
    finder.addMatcher(match_out_ports, this);
    finder.addMatcher(match_in_out_ports, this);
    finder.addMatcher(match_internal_signal, this);
    finder.addMatcher(match_sc_in_clk, this);
    finder.addMatcher(match_non_sc_types, this);
  }

  virtual void run(const MatchFinder::MatchResult &result) {
    if (auto fd = checkMatch<FieldDecl>("sc_in_clk", result)) {
      std::string port_name{fd->getIdentifier()->getNameStart()};
      cout << " Found sc_in_clk: " << port_name << endl;

      insert_port(clock_ports_, fd);
    }

    if (auto fd = checkMatch<FieldDecl>("sc_in", result)) {
      auto port_name{fd->getIdentifier()->getNameStart()};
      cout << " Found sc_in: " << port_name << endl;
      insert_port(in_ports_, fd);
    }

    if (auto fd = checkMatch<FieldDecl>("sc_out", result)) {
      auto port_name{fd->getIdentifier()->getNameStart()};
      cout << " Found sc_out: " << port_name << endl;
      insert_port(out_ports_, fd);
    }

    if (auto fd = checkMatch<FieldDecl>("sc_inout", result)) {
      auto port_name{fd->getIdentifier()->getNameStart()};
      cout << " Found sc_inout: " << port_name << endl;
      insert_port(inout_ports_, fd);
    }

    if (auto fd = checkMatch<FieldDecl>("sc_signal", result)) {
      auto signal_name{fd->getIdentifier()->getNameStart()};
      cout << " Found sc_signal: " << signal_name << endl;
      insert_port(signal_fields_, fd);
    }

    if (auto fd = checkMatch<FieldDecl>("other_fields", result)) {
      auto field_name{fd->getIdentifier()->getNameStart()};
      cout << " Found others fields: " << field_name << endl;
      insert_port(other_fields_, fd);
    }
  }

  void dump() {
    printTemplateArguments(clock_ports_);
    printTemplateArguments(in_ports_);
    printTemplateArguments(out_ports_);
    printTemplateArguments(inout_ports_);
    printTemplateArguments(signal_fields_);
    printTemplateArguments(other_fields_);
  }
};

///////////////////////////////////////////////////////////////////////////////
// Class ModuleDeclarationMatcher
//
//
///////////////////////////////////////////////////////////////////////////////
class ModuleDeclarationMatcher : public MatchFinder::MatchCallback {
  //
 public:
  typedef std::vector<std::tuple<std::string, CXXRecordDecl *> >
      ModuleDeclarationType;
  // Map to hold CXXREcordDecl to module declaration type name.
  typedef std::pair<CXXRecordDecl *, std::string> ModuleDeclarationPairType;
  typedef std::map<CXXRecordDecl *, std::string> ModuleDeclarationMapType;

  // typedef std::tuple<std::string, Decl *> InstanceDeclType;
  typedef std::vector<InstanceMatcher::InstanceDeclType> InstanceListType;
  typedef std::pair<CXXRecordDecl *, InstanceListType>
      DeclarationInstancePairType;
  typedef std::map<CXXRecordDecl *, InstanceListType>
      DeclarationsToInstancesMapType;
 private:
  ModuleDeclarationType found_declarations_;
  ModuleDeclarationType found_template_declarations_;
  // One of those needs to be removed.
  ModuleDeclarationType pruned_declarations_;
  ModuleDeclarationMapType pruned_declarations_map_;

  DeclarationsToInstancesMapType declaration_instance_map_;

  // Match nested instances
  InstanceMatcher instance_matcher;

  // Match ports 
  PortMatcher port_matcher;

 public:
  const DeclarationsToInstancesMapType &getInstances() {
    return declaration_instance_map_;
  };
  void registerMatchers(MatchFinder &finder);
  virtual void run(const MatchFinder::MatchResult &result);
  const ModuleDeclarationMapType &getFoundModuleDeclarations() const {
    return pruned_declarations_map_;
  }

  void pruneMatches();
  void dump();
};

};  // namespace sc_ast_matchers

#endif
