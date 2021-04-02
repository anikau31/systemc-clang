#ifndef _MATCHERS_H_
#define _MATCHERS_H_

#include <map>
#include <vector>

#include "InstanceMatcher.h"
#include "CXXRecordDeclUtils.h"
#include "ModuleInstance.h"
#include "PortMatcher.h"
#include "clang/ASTMatchers/ASTMatchers.h"

using namespace systemc_clang;

/// Different matchers may use different DEBUG_TYPE
#undef DEBUG_TYPE
#define DEBUG_TYPE "Matchers"

namespace sc_ast_matchers {

using namespace utils::cxx_construct_decl_utils;
///////////////////////////////////////////////////////////////////////////////
//
/// Class ModuleDeclarationMatcher
//
///////////////////////////////////////////////////////////////////////////////
class ModuleDeclarationMatcher : public MatchFinder::MatchCallback {
 public:
  typedef std::vector<InstanceMatcher::InstanceDeclType> InstanceListType;

  /// This will store all the modules as ModuleDecl.
  typedef std::pair<clang::CXXRecordDecl *, ModuleInstance *> ModulePairType;
  typedef std::multimap<clang::CXXRecordDecl *, ModuleInstance *> ModuleMapType;

 private:
  /// This will store the module instances as pair of CXXRecordDecl*,
  /// ModuleInstance*. The CXXRecordDecl* is the type of the sc_module, and
  /// ModuleInstance holds additional parsed information.
  ModuleMapType modules_;

  // Match nested instances
  InstanceMatcher instance_matcher_;

 public:
  const InstanceMatcher &getInstanceMatcher() { return instance_matcher_; }

  /// Register the matchers.
  void registerMatchers(MatchFinder &finder) {
    // Add instance matcher
    instance_matcher_.registerMatchers(finder);
  }

  virtual void run(const MatchFinder::MatchResult &result) {}

  const ModuleMapType &getFoundModuleDeclarations() const { return modules_; }

  void runPortMatcher(ASTContext &context, const clang::CXXRecordDecl *decl,
                      ModuleInstance *add_module) {
    MatchFinder port_registry{};
    PortMatcher port_matcher{};
    port_matcher.registerMatchers(port_registry);
    port_registry.match(*decl, context);
    // decl->dump();
    LLVM_DEBUG(llvm::dbgs() << "========== Port Matcher =============\n");
    port_matcher.dump();

    // All the ports for the CXXRecordDecl should be matched.
    // We can populate the ModuleInstance with that information.
    add_module->addPorts(port_matcher.getInputPorts(), "sc_in");
    // Clock ports are also sc_in
    add_module->addPorts(port_matcher.getClockPorts(), "sc_in");
    add_module->addPorts(port_matcher.getOutputPorts(), "sc_out");
    add_module->addPorts(port_matcher.getInOutPorts(), "sc_inout");
    add_module->addPorts(port_matcher.getOtherVars(), "others");
    add_module->addPorts(port_matcher.getSignals(), "sc_signal");
    add_module->addPorts(port_matcher.getInputStreamPorts(), "sc_stream_in");
    add_module->addPorts(port_matcher.getOutputStreamPorts(), "sc_stream_out");
    add_module->addPorts(port_matcher.getSubmodules(), "submodules");
  }

  void processInstanceCXXDecls(ASTContext &context) {
    // Must have found instances.
    // 1. For every module found, check if there is an instance.
    // 2. If there is an instance, then add it into the list.

    llvm::outs() << "###### DUMP Instance Matches \n";
    instance_matcher_.dump();

    auto instance_map{instance_matcher_.getInstanceMap()};

    /// Go through every instance found (ValueDecl), and find if there are any
    /// instances in any of its base classes, and add them to the instance_map.

    for (const auto inst : instance_map) {
      ModuleInstanceType instance{inst.second};
      clang::CXXRecordDecl *decl{
          dyn_cast<clang::CXXRecordDecl>(inst.second.getInstanceTypeDecl())};
      auto base_decls{getAllBaseClasses(decl)};
      for (const auto &base_decl : base_decls) {
        llvm::dbgs() << "=============================== BASES =======================\n";
        llvm::dbgs() << "Run base instance matcher: "
                     << base_decl->getNameAsString() << " \n";
        InstanceMatcher base_instance_matcher;
        MatchFinder base_instance_reg{};
        base_instance_matcher.registerMatchers(base_instance_reg);
        base_instance_matcher.setParentFieldDecl(instance.getParentDecl());
        base_instance_reg.match(*base_decl, context);
        llvm::dbgs() << "+ Dump base instance matcher\n";
        base_instance_matcher.dump();
        llvm::dbgs() << "+ End dump base instance matcher\n";

        /// Copy contents over.
        instance_matcher_.operator=(base_instance_matcher);
      }
    }
    /// Base classes may add instances. So we must get the updated instance_map.
    instance_map = instance_matcher_.getInstanceMap();

    // Each inst is of type pair<Decl*, ModuleInstancetype>
    for (auto inst : instance_map) {
      ModuleInstanceType instance{inst.second};

      clang::CXXRecordDecl *decl{
          dyn_cast<clang::CXXRecordDecl>(inst.second.getInstanceTypeDecl())};
      auto name{decl->getNameAsString()};
      llvm::outs() << "############### ====> INST: " << inst.first
                   << ", name: " << name
                   << ", instance_name: " << inst.second.instance_name << "\n";

      InstanceListType instance_list;
      instance_matcher_.findInstanceByVariableType(decl, instance_list);
      // This is the new data structure that uses ModuleDecl internally.
      if (instance_list.size() > 1) {
        assert(true);
      }

      auto add_module{new ModuleInstance(name, decl)};
      add_module->setInstanceInfo(instance);

      modules_.insert(std::pair<clang::CXXRecordDecl *, ModuleInstance *>(
          decl, add_module));

      llvm::dbgs() << "[Running port matcher]\n";
      runPortMatcher(context, decl, add_module);

      llvm::dbgs() << "[Running Base class logic]\n";
      /// Find if the instance CXXREcordDecl has a base class, and parse that
      /// too. Any ports, signals, etc. should be incorporated into the module
      /// instance.
      /*
      auto base_decls{getAllBaseClasses(decl)};
      for (const auto &base_decl : base_decls) {
        llvm::dbgs() << "=============================== BASES for " << name
                     << " =======================\n";
        llvm::dbgs() << "Base class: " << base_decl->getNameAsString() << "\n";
        runPortMatcher(context, base_decl, add_module);

        llvm::dbgs() << "Run base instance matcher: "
                     << base_decl->getNameAsString() << " \n";
        InstanceMatcher base_instance_matcher;
        MatchFinder base_instance_reg{};
        base_instance_matcher.registerMatchers(base_instance_reg);
        base_instance_matcher.setParentFieldDecl(instance.getParentDecl());
        base_instance_reg.match(*base_decl, context);
        llvm::dbgs() << "+ Dump base instance matcher\n";
        base_instance_matcher.dump();
        llvm::dbgs() << "+ End dump base instance matcher\n";

        /// Copy contents over.
        instance_matcher_.operator=(base_instance_matcher);
      }
      */
    }
  }

  void dump() {
    llvm::outs() << "[DBG] Module instances: " << modules_.size() << "\n";
    for (const auto &i : modules_) {
      auto cxx_decl{i.first};
      // TODO: really awkward
      auto module_decl{i.second};
      auto decl_name{module_decl->getName()};

      LLVM_DEBUG(llvm::outs() << "CXXRecordDecl* " << cxx_decl
                              << ", module name: " << decl_name << "\n";);
    }

    // Print the instances.
    instance_matcher_.dump();
  }
};

};  // namespace sc_ast_matchers

#endif
