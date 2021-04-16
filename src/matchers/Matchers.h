#ifndef _MATCHERS_H_
#define _MATCHERS_H_

#include <map>
#include <vector>

#include "InstanceMatcher.h"
#include "CXXRecordDeclUtils.h"
#include "ModuleInstance.h"
#include "PortMatcher.h"

#include "FindConstructor.h"
#include "FindEntryFunctions.h"
#include "FindEvents.h"
#include "FindGlobalEvents.h"
#include "FindNetlist.h"
#include "FindNotify.h"
#include "FindSimTime.h"
#include "FindTLMInterfaces.h"
#include "FindTemplateParameters.h"
#include "FindWait.h"

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

  void runModuleDeclarationMatchers(ASTContext &context,
                                    clang::CXXRecordDecl *cxx_decl,
                                    ModuleInstance *add_module_decl) {
    // setInstanceInfo done in pruneMatches
    //
    FindTemplateParameters tparms{cxx_decl};
    add_module_decl->setTemplateParameters(tparms.getTemplateParameters());
    add_module_decl->setTemplateArgs(tparms.getTemplateArgs());

    /// 3. Find constructor
    //
    //
    LLVM_DEBUG(llvm::dbgs() << "4. Set the constructor.\n";);
    vector<EntryFunctionContainer *> _entryFunctionContainerVector;
    FindConstructor constructor{add_module_decl->getModuleClassDecl(),
                                llvm::dbgs()};
    add_module_decl->addConstructor(&constructor);

    /// 4. Find ports
    /// This is done for the declaration.
    //
    //
    // 5. Find  entry functions within one sc_module.
    LLVM_DEBUG(llvm::dbgs() << "5. Set the entry functions\n";);
    FindEntryFunctions findEntries{add_module_decl->getModuleClassDecl(),
                                   llvm::dbgs()};
    FindEntryFunctions::entryFunctionVectorType *entryFunctions{
        findEntries.getEntryFunctions()};
    LLVM_DEBUG(llvm::dbgs() << "6. Set the process\n";);
    add_module_decl->addProcess(entryFunctions);

    SensitivityMatcher sens_matcher{};
    MatchFinder matchRegistry{};
    sens_matcher.registerMatchers(matchRegistry);
    matchRegistry.match(*constructor.getConstructorDecl(), context);

    for (size_t i{0}; i < entryFunctions->size(); i++) {
      EntryFunctionContainer *ef{(*entryFunctions)[i]};

      /// Add the sensitivity information to each of the entry functions.
      EntryFunctionContainer::SenseMapType sensitivity_info{
          sens_matcher.getSensitivityMap()};
      ef->addSensitivityInfo(sensitivity_info);

      if (ef->getEntryMethod() == nullptr) {
        LLVM_DEBUG(llvm::dbgs() << "ERROR";);
        continue;
      }

      FindWait findWaits{ef->getEntryMethod(), llvm::dbgs()};
      ef->addWaits(findWaits);

      FindNotify findNotify{ef->getEntryMethod(), llvm::dbgs()};
      ef->addNotifys(findNotify);

      _entryFunctionContainerVector.push_back(ef);
    }
  }

  void matchInstancesInBaseClasses(ASTContext &context) {
    auto instance_map{instance_matcher_.getInstanceMap()};
    for (const auto &inst : instance_map) {
      ModuleInstanceType instance{inst.second};
      clang::CXXRecordDecl *decl{
          dyn_cast<clang::CXXRecordDecl>(inst.second.getInstanceTypeDecl())};
      clang::ValueDecl *vd{
          dyn_cast<clang::ValueDecl>(inst.second.getInstanceDecl())};

      auto base_decls{getAllBaseClasses(decl)};
      for (const auto &base_decl : base_decls) {
        llvm::dbgs() << "=============================== BASES "
                     << decl->getNameAsString() << " =======================\n";
        llvm::dbgs() << "Run base instance matcher: "
                     << base_decl->getNameAsString() << " \n";

        InstanceMatcher base_instance_matcher;
        MatchFinder base_instance_reg{};
        base_instance_matcher.registerMatchers(base_instance_reg);
        base_instance_matcher.setParentFieldDecl(vd);
        base_instance_reg.match(*base_decl, context);
        llvm::dbgs() << "+ Dump base instance matcher\n";
        base_instance_matcher.dump();
        llvm::dbgs() << "+ End dump base instance matcher\n";

        /// Copy contents over.
        instance_matcher_ = base_instance_matcher;
      }
    }
  }

  void processInstanceCXXDecls(ASTContext &context) {
    // Must have found instances.
    // 1. For every module found, check if there is an instance.
    // 2. If there is an instance, then add it into the list.

    llvm::outs() << "###### DUMP Instance Matches \n";
    instance_matcher_.dump();

    auto instance_map{instance_matcher_.getInstanceMap()};

    /// 1. The instances found so far are only the ValueDecls.
    /// However, there can be instances in base classes of the declarations of
    /// these ValueDecls that are not going to be recognized. As a result, we
    /// need to find those instances within the base classes of the identified
    /// ValueDecls.
    matchInstancesInBaseClasses(context);

    /// Base classes may add instances. So we must get the updated instance_map.
    instance_map = instance_matcher_.getInstanceMap();

    /// DEBUG: output all the classes that have been identified. =======
    for (const auto &inst: instance_map) {
      llvm::dbgs()
          << "@@@@@@@@@@@@@ INSTANCE MAP with base instances @@@@@@@@@@@\n";

      clang::CXXRecordDecl *decl{
          dyn_cast<clang::CXXRecordDecl>(inst.second.getInstanceTypeDecl())};
      auto name{decl->getNameAsString()};
      llvm::dbgs() << "class: " << name << "\n";
    }
    /// END DEBUG ===========================================

    /// Go through each identified instance, and identify any ports and module information.
    /// 
    for (const auto &inst: instance_map) {
      ModuleInstanceType instance{inst.second};

      clang::CXXRecordDecl *decl{
          dyn_cast<clang::CXXRecordDecl>(inst.second.getInstanceTypeDecl())};
      auto name{decl->getNameAsString()};
      llvm::outs() << "############### ====> INST: " << inst.first
                   << ", name: " << name
                   << ", instance_name: " << inst.second.instance_name << "\n";

      auto add_module{new ModuleInstance(name, decl)};
      add_module->setInstanceInfo(instance);

      /// TODO: Why do we need this when we can just access the instances and then get the decl?
      modules_.insert(std::pair<clang::CXXRecordDecl *, ModuleInstance *>(
          decl, add_module));


      llvm::dbgs() << "[Running module declaration matchers]\n";
      runModuleDeclarationMatchers(context, decl, add_module);
      llvm::dbgs() << "[Running port matcher]\n";
      runPortMatcher(context, decl, add_module);

      llvm::dbgs() << "[Running Base class logic]\n";
      /// Find if the instance CXXREcordDecl has a base class, and parse that
      /// too. Any ports, signals, etc. should be incorporated into the module
      /// instance.
      auto base_decls{getAllBaseClasses(decl)};
      for (const auto &base_decl : base_decls) {
        auto name{base_decl->getNameAsString()};
        ModuleInstance *base_module_instance{
            new ModuleInstance{name, base_decl}};
        llvm::dbgs() << "=============================== BASES for " << name
                     << " =======================\n";
        llvm::dbgs() << "Base class: " << base_decl->getNameAsString() << "\n";

        runModuleDeclarationMatchers(
            context, const_cast<clang::CXXRecordDecl *>(base_decl), base_module_instance);
        runPortMatcher(context, base_decl, base_module_instance);
        add_module->addBaseInstance(base_module_instance);
      }
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
