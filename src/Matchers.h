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

template <typename NodeType>
auto checkMatch(const std::string &name,
                const MatchFinder::MatchResult &result) {
  return result.Nodes.getNodeAs<NodeType>(name);
}

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
      // llvm::outs() << "FOUND AN FIELD instance: " << get<0>(*found_it) << ",
      // "
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
          hasDescendant(
            cxxConstructExpr(hasArgument(0, stringLiteral().bind("constructor_arg"))).bind("constructor_expr")
            )
          ).bind("instances_in_vardecl");
    /* clang-format on */

    finder.addMatcher(match_instances, this);
    finder.addMatcher(match_instances_vars, this);
  }

  virtual void run(const MatchFinder::MatchResult &result) {
    if (auto constructor =
            checkMatch<CXXConstructExpr>("constructor_expr", result)) {
      llvm::outs() << "@@ Found CONSTRUCTOR\n";
      constructor->dump();
    }

    if (auto instance = const_cast<FieldDecl *>(
            result.Nodes.getNodeAs<FieldDecl>("instances_in_fielddecl"))) {
      std::string name{instance->getIdentifier()->getNameStart()};
      llvm::outs() << "@@ Found a member field instance: " << name << "\n";

      instances_.push_back(std::make_tuple(name, instance));
    }

    if (auto instance = const_cast<VarDecl *>(
            result.Nodes.getNodeAs<VarDecl>("instances_in_vardecl"))) {
      std::string name{instance->getIdentifier()->getNameStart()};
      llvm::outs() << "@@ Found a member variable instance: " << name << "\n";

      if (auto instance_name = const_cast<CXXConstructExpr *>(
              result.Nodes.getNodeAs<CXXConstructExpr>("constructor_expr"))) {
        llvm::outs() << "Found constructor expression argument: "
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
      llvm::outs() << "module declarations name: " << get<0>(i) << ", "
                   << get<1>(i) << "\n";

      auto p_field_var_decl{get<1>(i)};
      if (isa<FieldDecl>(p_field_var_decl)) {
        llvm::outs() << " ==> FieldDecl\n";
      } else {
        llvm::outs() << " ==> VarDecl\n";
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
  PortType sc_ports_;

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
  void registerMatchers(MatchFinder &finder,
                        const std::string &top_module_decl) {
    /* clang-format off */

    auto match_sc_in_clk = cxxRecordDecl( 
        isDerivedFrom(hasName("sc_module")),
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
          hasName(top_module_decl), // Specifies the top-level module name.
          hasDefinition(),          // There must be a definition.
          unless( isImplicit() ),   // Templates generate implicit structs - so ignore.
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

     auto match_sc_ports = cxxRecordDecl(
        match_module_decls,
        forEach(
          fieldDecl(
            hasType(
              cxxRecordDecl(isDerivedFrom(hasName("sc_port")))
            )
          ).bind("sc_port")
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
    finder.addMatcher(match_sc_ports, this);
  }

  virtual void run(const MatchFinder::MatchResult &result) {
    if (auto fd = checkMatch<FieldDecl>("sc_in_clk", result)) {
      std::string port_name{fd->getIdentifier()->getNameStart()};
      llvm::outs() << " Found sc_in_clk: " << port_name << "\n";
      insert_port(clock_ports_, fd);
    }

    if (auto fd = checkMatch<FieldDecl>("sc_in", result)) {
      auto port_name{fd->getIdentifier()->getNameStart()};
      llvm::outs() << " Found sc_in: " << port_name << "\n";
      insert_port(in_ports_, fd);
    }

    if (auto fd = checkMatch<FieldDecl>("sc_out", result)) {
      auto port_name{fd->getIdentifier()->getNameStart()};
      llvm::outs() << " Found sc_out: " << port_name << "\n";
      insert_port(out_ports_, fd);
    }

    if (auto fd = checkMatch<FieldDecl>("sc_inout", result)) {
      auto port_name{fd->getIdentifier()->getNameStart()};
      llvm::outs() << " Found sc_inout: " << port_name << "\n";
      insert_port(inout_ports_, fd);
    }

    if (auto fd = checkMatch<FieldDecl>("sc_signal", result)) {
      auto signal_name{fd->getIdentifier()->getNameStart()};
      llvm::outs() << " Found sc_signal: " << signal_name << "\n";
      insert_port(signal_fields_, fd);
    }

    if (auto fd = checkMatch<FieldDecl>("other_fields", result)) {
      auto field_name{fd->getIdentifier()->getNameStart()};
      llvm::outs() << " Found others fields: " << field_name << "\n";
      insert_port(other_fields_, fd);
    }

    if (auto fd = checkMatch<FieldDecl>("sc_port", result)) {
      auto field_name{fd->getIdentifier()->getNameStart()};
      llvm::outs() << " Found sc_port : " << field_name << "\n";
      insert_port(sc_ports_, fd);
    }
  }

  void dump() {
    printTemplateArguments(clock_ports_);
    printTemplateArguments(in_ports_);
    printTemplateArguments(out_ports_);
    printTemplateArguments(inout_ports_);
    printTemplateArguments(signal_fields_);
    printTemplateArguments(other_fields_);
    printTemplateArguments(sc_ports_);
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
  std::string top_module_decl_;
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

  void set_top_module_decl(const std::string &top) {
    // If there is no top specified, then match all, otherwise only top module.
    if (top = "") {
      top_module_decl_ = ".*";
    } else {
      top_module_decl_ = top;
    }

    // Register the matchers
    void registerMatchers(MatchFinder & finder) {
      /* clang-format off */

  auto match_module_decls = 
    cxxRecordDecl(
        matchesName(top_module_decl_),  // Specifies the top-level module name.
        hasDefinition(),            // There must be a definition.
        unless( isImplicit() ),     // Templates generate implicit structs - so ignore.
        isDerivedFrom(
          hasName("::sc_core::sc_module") 
          ),
        unless(isDerivedFrom(matchesName("sc_event_queue")))
        ).bind("sc_module");
      /* clang-format on */

      // add all the matchers.
      finder.addMatcher(match_module_decls, this);

      // add instance matcher
      instance_matcher.registerMatchers(finder);

      // add port (field) matcher
      port_matcher.registerMatchers(finder, top_module_decl_);
    }

    virtual void run(const MatchFinder::MatchResult &result) {
      if (auto decl = const_cast<CXXRecordDecl *>(
              result.Nodes.getNodeAs<CXXRecordDecl>("sc_module"))) {
        llvm::outs() << " Found sc_module: "
                     << decl->getIdentifier()->getNameStart()
                     << " CXXRecordDecl*: " << decl << "\n";
        std::string name{decl->getIdentifier()->getNameStart()};
        // decl->dump();
        //
        if (isa<ClassTemplateSpecializationDecl>(decl)) {
          llvm::outs() << "TEMPLATE SPECIAL\n";
          found_template_declarations_.push_back(std::make_tuple(name, decl));
        } else {
          found_declarations_.push_back(std::make_tuple(name, decl));
        }

        /* This is going to match only in subtree.
        MatchFinder instance_registry{};
        instance_matcher.registerMatchers(instance_registry);
        instance_registry.match(*decl, *result.Context);
        */
      }
    }

    const ModuleDeclarationMapType &getFoundModuleDeclarations() const {
      return pruned_declarations_map_;
    }

    void pruneMatches() {
      // Must have found instances.
      // 1. For every module found, check if there is an instance.
      // 2. If there is an instance, then add it into the list.

      for (auto const &element : found_declarations_) {
        auto decl{get<1>(element)};
        // std::llvm::outs() << "## fd  name: " << get<0>(element) << "\n ";
        InstanceListType instance_list;
        InstanceMatcher::InstanceDeclType instance;
        if (instance_matcher.findInstance(decl, instance)) {
          // pruned_declarations_.push_back(element);
          pruned_declarations_map_.insert(
              ModuleDeclarationPairType(decl, get<0>(element)));
          instance_list.push_back(instance);
          declaration_instance_map_.insert(
              DeclarationInstancePairType(decl, instance_list));
        }
      }

      for (auto const &element : found_template_declarations_) {
        auto decl{get<1>(element)};
        // std::llvm::outs() << "## ftd name: " << get<0>(element) << "\n ";
        InstanceListType instance_list;
        InstanceMatcher::InstanceDeclType instance;
        if (instance_matcher.findInstance(decl, instance)) {
          // pruned_declarations_.push_back(element);
          pruned_declarations_map_.insert(
              ModuleDeclarationPairType(decl, get<0>(element)));
          instance_list.push_back(instance);
          declaration_instance_map_.insert(
              DeclarationInstancePairType(decl, instance_list));
        }
      }
    }

    void dump() {
      llvm::outs() << "## Top-level module: " << top_module_decl_ << "\n";
      llvm::outs() << "## Non-template module declarations: "
                   << found_declarations_.size() << "\n";
      for (const auto &i : found_declarations_) {
        llvm::outs() << "module name         : " << get<0>(i) << ", "
                     << get<1>(i) << "\n";
      }

      llvm::outs() << "## Template module declarations: "
                   << found_template_declarations_.size() << "\n";
      for (const auto &i : found_template_declarations_) {
        llvm::outs() << "template module name: " << get<0>(i) << ", "
                     << get<1>(i) << "\n";
      }

      // for (const auto &i : pruned_declarations_) {
      // llvm::outs() << "pruned module name: " << get<0>(i) << ", " <<
      // get<1>(i)
      // << "\n";
      // }

      llvm::outs() << "## Pruned declaration Map: "
                   << pruned_declarations_map_.size() << "\n";
      for (const auto &i : pruned_declarations_map_) {
        auto decl{i.first};
        auto decl_name{i.second};
        llvm::outs() << "CXXRecordDecl* " << i.first
                     << ", module name: " << decl_name << "\n";
      }

      // Print the instances.
      instance_matcher.dump();

      llvm::outs() << "\n## Dump map of decl->instances: "
                   << declaration_instance_map_.size() << "\n";

      for (const auto &i : declaration_instance_map_) {
        auto decl{i.first};
        auto instance_list{i.second};

        llvm::outs() << "decl: " << decl->getIdentifier()->getNameStart();
        for (const auto &instance : instance_list) {
          llvm::outs() << ", instance type: " << get<0>(instance) << ",   "
                       << get<1>(instance) << "\n";
        }
      }

      llvm::outs() << "\n";
      llvm::outs() << "## Printing ports"
                   << "\n";
      port_matcher.dump();
    }
  };

};  // namespace sc_ast_matchers

#endif
