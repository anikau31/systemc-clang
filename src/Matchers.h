#ifndef _MATCHERS_H_
#define _MATCHERS_H_
#include <map>
#include <tuple>
#include <vector>
#include "FindTemplateTypes.h"
#include "ModuleDecl.h"
#include "PortDecl.h"

#include "InstanceMatcher.h"
#include "PortMatcher.h"

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchersInternal.h"
#include "clang/ASTMatchers/ASTMatchersMacros.h"

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

  // This will store all the modules as ModuleDecl
  // typedef ModuleDecl* ModulePairType;
  typedef std::pair<CXXRecordDecl *, ModuleDecl *> ModulePairType;
  typedef std::map<CXXRecordDecl *, ModuleDecl *> ModuleMapType;

 private:
  std::string top_module_decl_;
  ModuleDeclarationType found_declarations_;
  ModuleDeclarationType found_template_declarations_;
  // One of those needs to be removed.
  ModuleDeclarationType pruned_declarations_;
  ModuleDeclarationMapType pruned_declarations_map_;

  DeclarationsToInstancesMapType declaration_instance_map_;

  // This will store the pruned modules as pair of string, ModuleDecl*
  // The string will be the class name?
  ModuleMapType modules_;

  // Match nested instances
  InstanceMatcher instance_matcher;

  // Match ports
  // PortMatcher port_matcher_;

 public:
  const DeclarationsToInstancesMapType &getInstances() {
    return declaration_instance_map_;
  };

  const InstanceMatcher &getInstanceMatcher() {
    return instance_matcher;
  }

  void set_top_module_decl(const std::string &top) {
    // If there is no top specified, then match all, otherwise only top module.
    // The regular expression for matchesName() matcher for ".*" matches
    // anything. That regular expression is enabled only when top module is not
    // specified.
    if ((top == "!none") || (top == "")) {
      top_module_decl_ = ".*";
    } else {
      top_module_decl_ = top;
    }
  }

  // Register the matchers
  void registerMatchers(MatchFinder &finder) {
    /* clang-format off */

    // This is in case the set method is not called explicitly.
    // Simply pass in what is the default.
    set_top_module_decl( top_module_decl_ );

    auto match_module_decls = 
      cxxRecordDecl(
          //matchesName(top_module_decl_),  // Specifies the top-level module name.
          hasDefinition(),            // There must be a definition.
          unless( isImplicit() ),     // Templates generate implicit structs - so ignore.
          isDerivedFrom(hasName("::sc_core::sc_module")),
          unless(isDerivedFrom(matchesName("sc_event_queue")))
          ).bind("sc_module");
    /* clang-format on */

    // add all the matchers.
    finder.addMatcher(match_module_decls, this);

    // add instance matcher
    instance_matcher.registerMatchers(finder);

    // add port (field) matcher
    // port_matcher_.registerMatchers(finder);
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
      // TODO: Should we need this separation now?
      // It seems that we can simply store them whether they are template
      // specializations or not.
      // This is necessary because of the way clang represents them in their
      // AST.
      //
      if (isa<ClassTemplateSpecializationDecl>(decl)) {
        // llvm::outs() << "TEMPLATE SPECIAL\n";
        found_template_declarations_.push_back(std::make_tuple(name, decl));
      } else {
        // llvm::outs() << "NOT TEMPLATE SPECIAL\n";
        found_declarations_.push_back(std::make_tuple(name, decl));
      }

      // This is the new data structure that uses ModuleDecl internally.
      // Unpruned
      auto add_module{new ModuleDecl(name, decl)};
      modules_.insert(
          std::pair<CXXRecordDecl *, ModuleDecl *>(decl, add_module));

      // Instances should not be in subtree matching.
      //

      // Subtree matcher
      MatchFinder port_registry{};
      PortMatcher port_matcher{top_module_decl_};
      port_matcher.registerMatchers(port_registry);
      port_registry.match(*decl, *result.Context);
      // decl->dump();
      port_matcher.dump();

      // All the ports for the CXXRecordDecl should be matched.
      // We can populate the ModuleDecl with that information.
      add_module->addPorts(port_matcher.getInputPorts(), "sc_in");
      // Clock ports are also sc_in
      add_module->addPorts(port_matcher.getClockPorts(), "sc_in");
      add_module->addPorts(port_matcher.getOutputPorts(), "sc_out");
      add_module->addPorts(port_matcher.getInOutPorts(), "sc_inout");
      add_module->addPorts(port_matcher.getOtherVars(), "others");
      add_module->addPorts(port_matcher.getSignals(), "sc_signal");
      add_module->addPorts(port_matcher.getInputStreamPorts(), "sc_stream_in");
      add_module->addPorts(port_matcher.getOutputStreamPorts(),
                           "sc_stream_out");

      // TODO: Add all ports (getPorts) that are derived from sc_port.
      // This was requested by Scott.  The matcher is already in place.
      //
    }
  }

  const ModuleMapType &getFoundModuleDeclarations() const { return modules_; }

  void removeModule(CXXRecordDecl *decl) {
    // Remove declarations from modules_
    // This is the pruning.

    auto remove_it{modules_.find(decl)};
    if (remove_it != modules_.end()) {
      modules_.erase(remove_it);
    }
  }

  void pruneMatches() {
    // Must have found instances.
    // 1. For every module found, check if there is an instance.
    // 2. If there is an instance, then add it into the list.

    std::string dbg{"[pruneMatches]"};
    llvm::outs() << "\n"
                 << dbg << " number_of_declarations "
                 << found_declarations_.size() << " number_of_instances "
                 << instance_matcher.getInstanceMap().size() << "\n";

    for (auto const &element : found_declarations_) {
      auto decl{get<1>(element)};
      llvm::outs() << "- decl name: " << get<0>(element) << " " << decl << "\n ";
      InstanceListType instance_list;

      if (instance_matcher.findInstanceByVariableType(decl, instance_list)) {
        pruned_declarations_map_.insert(
            ModuleDeclarationPairType(decl, get<0>(element)));
        // instance_list.push_back(instance);
        declaration_instance_map_.insert(
            DeclarationInstancePairType(decl, instance_list));
      } else {
        // Remove it from modules_
        removeModule(decl);
      }
    }

    for (auto const &element : found_template_declarations_) {
      auto decl{get<1>(element)};
      // std::llvm::outs() << "## ftd name: " << get<0>(element) << "\n ";
      InstanceListType instance_list;
      // InstanceMatcher::InstanceDeclType instance;
      if (instance_matcher.findInstanceByVariableType(decl, instance_list)) {
        // pruned_declarations_.push_back(element);
        pruned_declarations_map_.insert(
            ModuleDeclarationPairType(decl, get<0>(element)));
        // instance_list.push_back(instance);
        declaration_instance_map_.insert(
            DeclarationInstancePairType(decl, instance_list));
      } else {
        removeModule(decl);
      }
    }
  }

  void dump() {
    llvm::outs() << "[ModuleDeclarationMatcher] Top-level module: "
                 << top_module_decl_ << "\n";
    llvm::outs()
        << "[ModuleDeclarationMatcher] Non-template module declarations: "
        << found_declarations_.size() << "\n";
    for (const auto &i : found_declarations_) {
      llvm::outs() << "[ModuleDeclarationMatcher] module name     : "
                   << get<0>(i) << ", " << get<1>(i) << "\n";
    }

    llvm::outs() << "[ModuleDeclarationMatcher] Template module declarations: "
                 << found_template_declarations_.size() << "\n";
    for (const auto &i : found_template_declarations_) {
      llvm::outs() << "[ModuleDeclarationMatcher] template module name: "
                   << get<0>(i) << ", " << get<1>(i) << "\n";
    }

    llvm::outs() << "[ModuleDeclarationMatcher] Pruned declaration Map: "
                 << pruned_declarations_map_.size() << "\n";
    for (const auto &i : pruned_declarations_map_) {
      auto decl{i.first};
      auto decl_name{i.second};
      llvm::outs() << "CXXRecordDecl* " << i.first
                   << ", module name: " << decl_name << "\n";
    }

    llvm::outs() << "[DBG] Module declarations: " << modules_.size() << "\n";
    for (const auto &i : modules_) {
      auto cxx_decl{i.first};
      // TODO: really awkward
      auto module_decl{i.second};
      auto decl_name{module_decl->getName()};

      llvm::outs() << "CXXRecordDecl* " << cxx_decl
                   << ", module name: " << decl_name << "\n";
      module_decl->dump(llvm::outs());
    }

    // Print the instances.
    instance_matcher.dump();

    llvm::outs() << "\n[DBG] Dump map of decl->instances: "
                 << declaration_instance_map_.size() << "\n";

    for (const auto &i : declaration_instance_map_) {
      auto decl{i.first};
      auto instance_list{i.second};

      llvm::outs() << "- decl: " << decl->getIdentifier()->getNameStart();
      for (const auto &instance : instance_list) {
        llvm::outs() << ", instance name: " << get<0>(instance) << ", inst type decl: " << get<1>(instance) << "\n";
      }
    }
  }
};

};  // namespace sc_ast_matchers

#endif
