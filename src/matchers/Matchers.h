#ifndef _MATCHERS_H_
#define _MATCHERS_H_
#include <map>
#include <tuple>
#include <vector>
#include <assert.h>

#include "InstanceMatcher.h"
#include "ModuleDecl.h"
#include "PortMatcher.h"
#include "clang/ASTMatchers/ASTMatchers.h"

using namespace systemc_clang;

/// Different matchers may use different DEBUG_TYPE
#undef DEBUG_TYPE
#define DEBUG_TYPE "Matchers"

namespace sc_ast_matchers {

template <typename NodeType>
auto checkMatch(const std::string &name,
                const MatchFinder::MatchResult &result) {
  return result.Nodes.getNodeAs<NodeType>(name);
}

///////////////////////////////////////////////////////////////////////////////
//
/// Class ModuleDeclarationMatcher
//
//
///////////////////////////////////////////////////////////////////////////////
class ModuleDeclarationMatcher : public MatchFinder::MatchCallback {
  //
 public:
  typedef std::vector<std::tuple<std::string, clang::CXXRecordDecl *> >
      ModuleDeclarationType;
  /// Map to hold CXXREcordDecl to module declaration type name.
  typedef std::pair<clang::CXXRecordDecl *, std::string>
      ModuleDeclarationPairType;
  typedef std::map<clang::CXXRecordDecl *, std::string>
      ModuleDeclarationMapType;

  // typedef std::tuple<std::string, Decl *> InstanceDeclType;
  typedef std::vector<InstanceMatcher::InstanceDeclType> InstanceListType;
  typedef std::pair<clang::CXXRecordDecl *, InstanceListType>
      DeclarationInstancePairType;
  typedef std::map<clang::CXXRecordDecl *, InstanceListType>
      DeclarationsToInstancesMapType;

  /// This will store all the modules as ModuleDecl.
  typedef std::pair<clang::CXXRecordDecl *, ModuleDecl *> ModulePairType;
  typedef std::multimap<clang::CXXRecordDecl *, ModuleDecl *> ModuleMapType;

 private:
  // std::string top_module_decl_;
  // One of those needs to be removed.

  DeclarationsToInstancesMapType declaration_instance_map_;

  /// This will store the pruned modules as pair of string, ModuleDecl*
  /// The string will be the class name?
  ModuleMapType modules_;

  // Match nested instances
  InstanceMatcher instance_matcher_;

 public:
  const DeclarationsToInstancesMapType &getInstances() {
    return declaration_instance_map_;
  };

  const InstanceMatcher &getInstanceMatcher() { return instance_matcher_; }

  //  void set_top_module_decl(const std::string &top) {
  // If there is no top specified, then match all, otherwise only top module.
  // The regular expression for matchesName() matcher for ".*" matches
  // anything. That regular expression is enabled only when top module is not
  // specified.
  // if ((top == "!none") || (top == "")) {
  // top_module_decl_ = ".*";
  // } else {
  // top_module_decl_ = top;
  // }
  // }

  /// Register the matchers.
  void registerMatchers(MatchFinder &finder) {
    // This is in case the set method is not called explicitly.
    // Simply pass in what is the default.
    // set_top_module_decl( top_module_decl_ );

    /* clang-format off */
    auto match_module_decls = 
      cxxRecordDecl(
          //matchesName(top_module_decl_),  // Specifies the top-level module name.
          hasDefinition(),            // There must be a definition.
          unless( isImplicit() ),     // Templates generate implicit structs - so ignore.
          isDerivedFrom(hasName("::sc_core::sc_module")),
          unless(isDerivedFrom(matchesName("sc_event_queue")))
        ).bind("sc_module");
    /* clang-format on */

    /// Add all the matchers.
    finder.addMatcher(match_module_decls, this);

    // Add instance matcher
    instance_matcher_.registerMatchers(finder);
  }

  virtual void run(const MatchFinder::MatchResult &result) {
    if (auto decl = const_cast<clang::CXXRecordDecl *>(
            result.Nodes.getNodeAs<clang::CXXRecordDecl>("sc_module"))) {
      LLVM_DEBUG(llvm::dbgs() << " Found sc_module: "
                              << decl->getIdentifier()->getNameStart()
                              << " CXXRecordDecl*: " << decl << "\n");
      std::string name{decl->getIdentifier()->getNameStart()};
            /*
      // This is the new data structure that uses ModuleDecl internally.
      // Unpruned
      auto add_module{new ModuleDecl(name, decl)};
      modules_.insert(
          std::pair<clang::CXXRecordDecl *, ModuleDecl *>(decl, add_module));

      // Instances should not be in subtree matching.
      //

      // Subtree matcher
      MatchFinder port_registry{};
      PortMatcher port_matcher{};
      port_matcher.registerMatchers(port_registry);
      port_registry.match(*decl, *result.Context);
      // decl->dump();
      LLVM_DEBUG(llvm::dbgs() << "========== Port Matcher =============\n");
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
      add_module->addPorts(port_matcher.getSubmodules(),
                           "submodules");

                           */
      // TODO: Add all ports (getPorts) that are derived from sc_port.
      // This was requested by Scott.  The matcher is already in place.
      //
    }
  }

  const ModuleMapType &getFoundModuleDeclarations() const { return modules_; }

  void removeModule(clang::CXXRecordDecl *decl) {
    // Remove declarations from modules_
    // This is the pruning.

    auto remove_it{modules_.find(decl)};
    if (remove_it != modules_.end()) {
      modules_.erase(remove_it);
    }
  }

  void pruneMatches(ASTContext &context) {
    // Must have found instances.
    // 1. For every module found, check if there is an instance.
    // 2. If there is an instance, then add it into the list.

    llvm::outs() << "###### DUMP Instance Matches \n";
    instance_matcher_.dump();

    auto instance_map{instance_matcher_.getInstanceMap()};
    // Each inst is of type pair<Decl*, ModuleInstancetype>
    for (auto inst : instance_map) {
      ModuleInstanceType instance{ inst.second };

      llvm::outs() << "############### ====> INST: " << inst.first << "\n";
      clang::CXXRecordDecl *decl{
          dyn_cast<clang::CXXRecordDecl>(inst.second.decl)};
      auto name{decl->getNameAsString()};

      InstanceListType instance_list;
      instance_matcher_.findInstanceByVariableType(decl, instance_list);
      declaration_instance_map_.insert(
          DeclarationInstancePairType(decl, instance_list));

      // This is the new data structure that uses ModuleDecl internally.
      if (instance_list.size() > 1 ) {
        assert(true);

      }
      auto add_module{new ModuleDecl(name, decl)};
      add_module->setInstanceInfo(instance);

      modules_.insert(
          std::pair<clang::CXXRecordDecl *, ModuleDecl *>(decl, add_module));


      MatchFinder port_registry{};
      PortMatcher port_matcher{};
      port_matcher.registerMatchers(port_registry);
      port_registry.match(*decl, context);
      // decl->dump();
      LLVM_DEBUG(llvm::dbgs() << "========== Port Matcher =============\n");
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
      add_module->addPorts(port_matcher.getSubmodules(), "submodules");
    }
  }

  void dump() {
    llvm::outs() << "[DBG] Module declarations: " << modules_.size() << "\n";
    for (const auto &i : modules_) {
      auto cxx_decl{i.first};
      // TODO: really awkward
      auto module_decl{i.second};
      auto decl_name{module_decl->getName()};

      llvm::outs() << "CXXRecordDecl* " << cxx_decl
                   << ", module name: " << decl_name << "\n";
    }

    // Print the instances.
    instance_matcher_.dump();

    llvm::outs() << "\n[DBG] Dump map of decl->instances: "
                 << declaration_instance_map_.size() << "\n";

    for (const auto &i : declaration_instance_map_) {
      auto decl{i.first};
      auto instance_list{i.second};

      llvm::outs() << "- decl: " << decl->getIdentifier()->getNameStart();
      for (const auto &instance : instance_list) {
        llvm::outs() << ", instance name: " << get<0>(instance)
                     << ", inst type decl: " << get<1>(instance) << "\n";
      }
    }
  }
};

};  // namespace sc_ast_matchers

#endif
