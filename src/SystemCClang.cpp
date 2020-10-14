#include "SystemCClang.h"

#include "Matchers.h"
#include "SensitivityMatcher.h"

#include "ModuleInstanceType.h"
#include "NetlistMatcher.h"

#include "clang/AST/ASTImporter.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"

using namespace systemc_clang;
using namespace clang;

using namespace sc_ast_matchers;

// Private methods
void SystemCConsumer::populateNestedModules(
    const InstanceMatcher::InstanceDeclarations &instance_map) {
  // Decl* => ModuleInstanceType
  for (auto const &inst : instance_map) {
    // get<0>(inst) is the Decl*, and get<1>(inst) is the ModuleInstanceType
    ModuleInstanceType module_inst{get<1>(inst)};
    LLVM_DEBUG(module_inst.dump());
    // Match with the same InstanceTypeDecl
    ModuleDecl *child{
        systemcModel_->getInstance(module_inst.getInstanceDecl())};

    LLVM_DEBUG(llvm::dbgs() << "# child instance decl "
                            << module_inst.getInstanceDecl() << "\n");
    //module_inst.getInstanceDecl()->dump();

    ModuleDecl *parent{systemcModel_->getInstance(module_inst.getParentDecl())};

    if (child) {
      LLVM_DEBUG(llvm::dbgs() << "- child " << child->getName() << " : "
                              << child->getInstanceName() << "\n");
    }
    if (parent) {
      LLVM_DEBUG(llvm::dbgs() << "- parent " << parent->getName() << " : "
                              << parent->getInstanceName() << "\n");
    }

    // Insert the child into the parent.
    if (child && parent) {
      LLVM_DEBUG(llvm::dbgs() << "Add a child-parent relationship\n");
      parent->addNestedModule(child);
    }
    LLVM_DEBUG(llvm::dbgs() << "\n";);
  }
}

bool SystemCConsumer::preFire() { return true; }

bool SystemCConsumer::postFire() { return true; }

bool SystemCConsumer::fire() {
  TranslationUnitDecl *tu{getContext().getTranslationUnitDecl()};
  // Reflection database.
  systemcModel_ = new Model{};

  FindGlobalEvents globals{tu, os_};
  FindGlobalEvents::globalEventMapType eventMap{globals.getEventMap()};
  systemcModel_->addGlobalEvents(eventMap);

  ModuleDeclarationMatcher module_declaration_handler{};
  MatchFinder matchRegistry{};
  module_declaration_handler.registerMatchers(matchRegistry);
  matchRegistry.matchAST(getContext());
  module_declaration_handler.pruneMatches(getContext());

  LLVM_DEBUG(llvm::dbgs() << "== Print module declarations pruned\n";
             module_declaration_handler.dump();
             llvm::dbgs() << "================ END =============== \n";);

  // MultiMap CXXRecordDecl => ModuleDecl*
  // Instances with same CXXRecordDecl will have multiple entries
  auto found_module_declarations{
      module_declaration_handler.getFoundModuleDeclarations()};
    ////////////////////////////////////////////////////////////////
  // Find the sc_main
  ////////////////////////////////////////////////////////////////
  FindSCMain scmain{tu, os_};

  if (scmain.isSCMainFound()) {
    FunctionDecl *fnDecl{scmain.getSCMainFunctionDecl()};

    // TODO: find any instances in sc_main.

    FindSimTime scstart{fnDecl, os_};
    systemcModel_->addSimulationTime(scstart.returnSimTime());
  } else {
    LLVM_DEBUG(llvm::dbgs() << "\n Could not find SCMain";);
  }

  ////////////////////////////////////////////////////////////////
  // Find the netlist.
  ////////////////////////////////////////////////////////////////

  //  std::map<clang::CXXRecordDecl *, InstanceListType>
  auto found_instances_declaration_map{module_declaration_handler.getInstances()};

  //
  // Create a ModuleDecl for each instance with the appropriately parsed
  // ModuleDecl.
  //

  // Go through each instance and find its appropriate module declaration.
  for (const auto &inst : found_instances_declaration_map) {
    // This is the CXXRecordDecl
    auto cxx_decl{inst.first};
    // List of InstanceListType
    auto instance_list{inst.second};

    auto incomplete_module_decl{found_module_declarations[cxx_decl]};
    // TODO:
    //
    // FIXME: This has to be replaced once xlat is fixed.
    std::vector<ModuleDecl *> module_decl_instances;

    for (const auto &instance : instance_list) {
      auto add_module_decl{new ModuleDecl{*incomplete_module_decl}};
      // 0: string
      // 1: Decl* (InstaceTypeDecl)
      // 2: ModuleInstanceType

      // Insert what you know about the parsed sc_module
      // 1. Insert the instance name from Matchers
      //
      auto inst_info{get<2>(instance)};
      inst_info.dump();

      LLVM_DEBUG(llvm::dbgs()
                     << "\n"
                     << "1. Set instance name: " << get<0>(instance) << "\n"
                     << "2. Set instance decl: " << cxx_decl->getNameAsString()
                     << " " << inst_info.getInstanceDecl() << "\n";);
      add_module_decl->setInstanceInfo(get<2>(instance));

      /// 2. Find the template arguments for the class.
      // In clang lingo: parameters are the templated values, and the arguments
      // are the specialization values for the templates.
      //
      LLVM_DEBUG(llvm::dbgs() << "3. Set template arguments\n";);
      FindTemplateParameters tparms{cxx_decl};
      add_module_decl->setTemplateParameters(tparms.getTemplateParameters());
      add_module_decl->setTemplateArgs(tparms.getTemplateArgs());

      /// 3. Find constructor
      //
      //
      LLVM_DEBUG(llvm::dbgs() << "4. Set the constructor.\n";);
      vector<EntryFunctionContainer *> _entryFunctionContainerVector;
      FindConstructor constructor{add_module_decl->getModuleClassDecl(), os_};
      add_module_decl->addConstructor(&constructor);

      /// 4. Find ports
      /// This is done for the declaration.
      //
      //
      // 5. Find  entry functions within one sc_module.
      LLVM_DEBUG(llvm::dbgs() << "5. Set the entry functions\n";);
      FindEntryFunctions findEntries{add_module_decl->getModuleClassDecl(),
                                     os_};
      FindEntryFunctions::entryFunctionVectorType *entryFunctions{
          findEntries.getEntryFunctions()};
      LLVM_DEBUG(llvm::dbgs() << "6. Set the process\n";);
      add_module_decl->addProcess(entryFunctions);

      /// We should only go through one of the CXXRecordDecls

      LLVM_DEBUG(
          llvm::dbgs()
          << "\n================ SENSITIVITY MATCHER =============== \n");
      SensitivityMatcher sens_matcher{};
      MatchFinder matchRegistry{};
      sens_matcher.registerMatchers(matchRegistry);
      matchRegistry.match(*constructor.getConstructorDecl(), getContext());

      LLVM_DEBUG(sens_matcher.dump();
                 llvm::dbgs() << "================ END =============== \n";);

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

        FindWait findWaits{ef->getEntryMethod(), os_};
        ef->addWaits(findWaits);

        FindNotify findNotify{ef->_entryMethodDecl, os_};
        ef->addNotifys(findNotify);

        _entryFunctionContainerVector.push_back(ef);
      }

      // Insert the module into the model.
      // All modules are also instances.

      // Make the dummy equal to the updated add_module_decl
      // This will make module declarations be one of the module instances.
      //*p_dummy_module_decl = *add_module_decl;
      // systemcModel_->addModuleDecl(p_dummy_module_decl);
      module_decl_instances.push_back(add_module_decl);
    }
    LLVM_DEBUG(llvm::dbgs() << "\n";);

    // TODO:
    //
    // FIXME: Only there to make sure xlat still compiles.
    // This should be removed.
    LLVM_DEBUG(llvm::dbgs() << "[HDP] Add instances to model\n";);
    systemcModel_->addModuleDeclInstances(incomplete_module_decl,
                                          module_decl_instances);
  }

  // Module instance map.
  auto module_instance_map{systemcModel_->getModuleInstanceMap()};

  LLVM_DEBUG(llvm::dbgs() << " @@@@@@@@ =============== Populate sub-modules "
                             "============= \n";);
  // This must have the instance matcher already run.
  // You need systemcModel_ and instance_matcher to build the hierarchy of
  // sub-modules.
  auto instance_matcher{module_declaration_handler.getInstanceMatcher()};
  auto instance_map{instance_matcher.getInstanceMap()};
  LLVM_DEBUG(
      llvm::dbgs() << "- Print out all the instances in the instance map\n";);
  populateNestedModules(instance_map);

  LLVM_DEBUG(
      llvm::dbgs() << "===========END  Populate sub-modules ============= \n";);

  // All instances are within the SystemC model.
  //  This must come after instances of ModuleDecl have been generated.
  //  This is because the netlist matcher inserts the port bindings into the
  //  instance.

  LLVM_DEBUG(
      llvm::dbgs() << "=============== ##### TEST NetlistMatcher ##### \n";);
  NetlistMatcher netlist_matcher{};
  MatchFinder netlist_registry{};
  netlist_matcher.registerMatchers(netlist_registry, systemcModel_,
                                   &module_declaration_handler);

  netlist_registry.match(*scmain.getSCMainFunctionDecl(), getContext());
  // TODO: Fix the top-level
  // if (getTopModule() == "!none") {
  // llvm::outs() << " No top module\n";
  // }
  //
  LLVM_DEBUG(llvm::dbgs() << "Begin netlist parsing on instances: "
                          << found_instances_declaration_map.size() << "\n";);
  for (const auto &inst : module_instance_map) {
    auto incomplete_mdecl{inst.first};
    auto instance_list{inst.second};

    for (auto const &instance : instance_list) {
      // ModuleDecl *mdecl{systemcModel_->getInstance(get<0>(instance))};
      ModuleDecl *mdecl{instance};
      auto ctordecl{mdecl->getConstructorDecl()};
      if (ctordecl != nullptr) {
        const FunctionDecl *fd{dyn_cast<FunctionDecl>(ctordecl)};
        ctordecl->getBody(fd);

        LLVM_DEBUG(llvm::dbgs() << "==============> RUN netlist matcher\n";);

        netlist_registry.match(*fd, getContext());

        LLVM_DEBUG(llvm::dbgs() << "==============> DONE netlist matcher\n";);
      }
    }
  }
  LLVM_DEBUG(netlist_matcher.dump();
             llvm::dbgs() << "##### END TEST NetlistMatcher ##### \n";);

  /*
  ////////////////////////////////////////////////////////////////
  // Figure out the module map.
  ////////////////////////////////////////////////////////////////
  Model::moduleMapType moduleMap{systemcModel_->getModuleDecl()};

  // <string, ModuleDecl*>
  for (Model::moduleMapType::iterator mit = moduleMap.begin(),
                                      mitend = moduleMap.end();
       mit != mitend; mit++) {
    ModuleDecl *mainmd{mit->second};
    int numInstances{mainmd->getNumInstances()};
    vector<ModuleDecl *> moduleDeclVec;

    os_ << "\nFor module: " << mit->first << " num instance : " << numInstances
        << "\n";

    for (unsigned int num{0}; num < numInstances; ++num) {
      auto md{new ModuleDecl{*mainmd}};

      // Find the template arguments for the class.
      // FindTemplateParameters tparms{mainmd->getModuleClassDecl(), os_};

      // md->setTemplateParameters(tparms.getTemplateParameters());
      // md->dump_json();

      vector<EntryFunctionContainer *> _entryFunctionContainerVector;
      FindConstructor constructor{mainmd->getModuleClassDecl(), os_};
      md->addConstructor(constructor.returnConstructorStmt());

      FindPorts ports{mainmd->getModuleClassDecl(), os_};
      //      ports.dump();
      md->addInputPorts(ports.getInputPorts());
      md->addOutputPorts(ports.getOutputPorts());
      md->addInputOutputPorts(ports.getInputOutputPorts());
      md->addOtherVars(ports.getOtherVars());
      md->addInputStreamPorts(ports.getInStreamPorts());
      md->addOutputStreamPorts(ports.getOutStreamPorts());

      FindTLMInterfaces findTLMInterfaces{mainmd->getModuleClassDecl(), os_};
      md->addInputInterfaces(findTLMInterfaces.getInputInterfaces());
      md->addOutputInterfaces(findTLMInterfaces.getOutputInterfaces());
      md->addInputOutputInterfaces(
          findTLMInterfaces.getInputOutputInterfaces());

      FindSignals signals{mainmd->getModuleClassDecl(), os_};
      md->addSignals(signals.getSignals());

      FindEntryFunctions findEntries{mainmd->getModuleClassDecl(), os_};
      FindEntryFunctions::entryFunctionVectorType *entryFunctions{
          findEntries.getEntryFunctions()};
      md->addProcess(entryFunctions);

      for (size_t i = 0; i < entryFunctions->size(); i++) {
        EntryFunctionContainer *ef{(*entryFunctions)[i]};
        FindSensitivity findSensitivity{constructor.returnConstructorStmt(),
                                        os_};
        ef->addSensitivityInfo(findSensitivity);

        if (ef->getEntryMethod() == nullptr) {
          os_ << "ERROR";
          continue;
        }

        FindWait findWaits{ef->getEntryMethod(), os_};
        ef->addWaits(findWaits);

        FindNotify findNotify{ef->_entryMethodDecl, os_};
        ef->addNotifys(findNotify);

#ifdef USE_SAUTO
        /// Does not compile
        SuspensionAutomata suspensionAutomata(findWaits.getWaitCalls(),
                                              ef->getEntryMethod(),
                                              &getContext(), llvm::errs());
        if (suspensionAutomata.initialize()) {
          suspensionAutomata.genSusCFG();
          suspensionAutomata.dumpSusCFG();
          suspensionAutomata.genSauto();
          suspensionAutomata.dumpSauto();
          ef->addSusCFGAuto(suspensionAutomata);
        }
#endif

        _entryFunctionContainerVector.push_back(ef);
      }
      moduleDeclVec.push_back(md);
    }
    // systemcModel_->addModuleDeclInstances(mainmd, moduleDeclVec);
  }
  */

  /*
     FindSCMain scmain(tu, os_);

     if (scmain.isSCMainFound())
     {
     FunctionDecl *fnDecl = scmain.getSCMainFunctionDecl();

     FindSimTime scstart(fnDecl, os_);
     systemcModel_->addSimulationTime(scstart.returnSimTime());

     }
     else {
     os_ <<"\n Could not find SCMain";
     }

     FindNetlist findNetlist(scmain.getSCMainFunctionDecl());
     findNetlist.dump();
     systemcModel_->addNetlist(findNetlist);

  // Only do this if SAUTO flag is set.
#ifdef USE_SAUTO
  // Generate SAUTO
  // Placing it here so that unique SAUTO for each instance
  // Model::moduleMapType moduleMap = systemcModel_->getModuleDecl();
  Model::moduleInstanceMapType moduleInstanceMap =
      systemcModel_->getModuleInstanceMap();

  for (Model::moduleInstanceMapType::iterator it = moduleInstanceMap.begin(),
                                              eit = moduleInstanceMap.end();
       it != eit; it++) {
    vector<ModuleDecl *> moduleDeclVec = it->second;
    for (size_t i = 0; i < moduleDeclVec.size(); i++) {
      ModuleDecl *moduleDecl = moduleDeclVec.at(i);
      vector<EntryFunctionContainer *> entryFunctionContainer =
          moduleDecl->getEntryFunctionContainer();
      for (size_t j = 0; j < entryFunctionContainer.size(); j++) {
        SuspensionAutomata suspensionAutomata(
            entryFunctionContainer.at(j)->getWaitCalls(),
            entryFunctionContainer.at(j)->getEntryMethod(), &getContext(),
            llvm::errs());
        if (suspensionAutomata.initialize()) {
          suspensionAutomata.genSusCFG();
          // suspensionAutomata.dumpSusCFG();
          suspensionAutomata.genSauto();
          // suspensionAutomata.dumpSauto();
          entryFunctionContainer.at(j)->addSusCFGAuto(suspensionAutomata);
        }
      }
    }
  }
#endif

     */
  LLVM_DEBUG(
      llvm::dbgs() << "Parsed SystemC model from systemc-clang\n";
      llvm::dbgs() << "============= MODEL ============================\n";
      systemcModel_->dump(llvm::dbgs());
      llvm::dbgs() << "==============END========================\n";);
  return true;
}

void SystemCConsumer::HandleTranslationUnit(ASTContext &context) {
  // ///////////////////////////////////////////////////////////////
  // / Pass 1: Find the necessary information.
  // ///////////////////////////////////////////////////////////////

  bool pre = false;
  pre = preFire();

  if (!pre) {
    return;
  }

  bool f = false;
  f = fire();

  if (!f) {
    return;
  }
  postFire();
}

SystemCConsumer::SystemCConsumer(ASTUnit *from_ast, std::string top)
    : os_{llvm::errs()},
      sm_{from_ast->getSourceManager()},
      context_{from_ast->getASTContext()},
      top_{top},
      systemcModel_{nullptr} {}

SystemCConsumer::SystemCConsumer(CompilerInstance &ci, std::string top)
    : os_{llvm::errs()},
      sm_{ci.getSourceManager()},
      context_{ci.getASTContext()},
      top_{top},
      systemcModel_{nullptr} {}

SystemCConsumer::~SystemCConsumer() {
  if (systemcModel_ != nullptr) {
    delete systemcModel_;
    systemcModel_ = nullptr;
  }
}

void SystemCConsumer::setTopModule(const std::string &top_module_decl) {
  top_ = top_module_decl;
}

Model *SystemCConsumer::getSystemCModel() { return systemcModel_; }

const std::string &SystemCConsumer::getTopModule() const { return top_; }

ASTContext &SystemCConsumer::getContext() const { return context_; }

SourceManager &SystemCConsumer::getSourceManager() const { return sm_; }
