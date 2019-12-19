#include "SystemCClang.h"

#include "Matchers.h"
#include "clang/AST/ASTImporter.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"

using namespace scpar;
using namespace clang;
using namespace std;

using namespace sc_ast_matchers;

bool SystemCConsumer::preFire() { return true; }

bool SystemCConsumer::postFire() { return true; }

bool SystemCConsumer::fire() {
  os_ << "Top module: " << getTopModule() << "\n";
  TranslationUnitDecl *tu{getContext().getTranslationUnitDecl()};
  // Reflection database.
  systemcModel_ = new Model{};

  // ANI : Do we need FindGlobalEvents?
  FindGlobalEvents globals{tu, os_};
  FindGlobalEvents::globalEventMapType eventMap{globals.getEventMap()};
  globals.dump_json();
  systemcModel_->addGlobalEvents(eventMap);

  //
  // TODO:
  // A first pass should be made to collect all sc_module declarations.
  // This is important so that the top-level module can be found.
  //

  // A first pass should be made to collect all sc_module declarations.
  // This is important so that the top-level module can be found.
  //
  os_ << "================ TESTMATCHER =============== \n";

  //InstanceMatcher match_instances{};
  ModuleDeclarationMatcher module_declaration_handler{};
  MatchFinder matchRegistry{};

  // Run all the matchers
  
  module_declaration_handler.registerMatchers(matchRegistry);
  //match_instances.registerMatchers( matchRegistry );

  matchRegistry.matchAST(getContext());
  //match_instances.dump();
  module_declaration_handler.pruneMatches();
  os_ << "== Print module declarations pruned\n";
  module_declaration_handler.dump();
  os_ << "================ END =============== \n";

  // Check if the top-level module one of the sc_module declarations?
  //
  auto found_module_declarations{
      module_declaration_handler.getFoundModuleDeclarations()};

  os_ << "@@@@@@@@@@@@ FOUND MODUE: " << found_module_declarations.size() <<"\n";
  auto found_top_module{std::find_if(
      found_module_declarations.begin(), found_module_declarations.end(),
      [this](std::tuple<std::string, CXXRecordDecl *> &element) {
        return std::get<0>(element) == getTopModule();
      })};

  if (found_top_module != found_module_declarations.end()) {
    os_ << "Found the top module: " << get<0>(*found_top_module) << ", "
        << get<1>(*found_top_module) << "\n";
  }

  // ===========================================================
  // 1. Add every module declaration to the model.
  // ===========================================================
  // Every module declaration that is found should be added to the model.
  //

  for (auto const &element : found_module_declarations) {
    auto module_declaration{new ModuleDecl{get<0>(element), get<1>(element)}};
    systemcModel_->addModuleDecl(module_declaration);
  }

  /*
  // Find the sc_modules
  //
  
  FindSCModules scmod{tu, os_};

  FindSCModules::moduleMapType scmodules{scmod.getSystemCModulesMap()};

  for (FindSCModules::moduleMapType::iterator mit = scmodules.begin(),
                                              mitend = scmodules.end();
       mit != mitend; ++mit) {
    ModuleDecl *md = new ModuleDecl{mit->first, mit->second};
    systemcModel_->addModuleDecl(md);

  }
  */

  ////////////////////////////////////////////////////////////////
  // Find the sc_main
  ////////////////////////////////////////////////////////////////
  FindSCMain scmain{tu, os_};

  if (scmain.isSCMainFound()) {
    FunctionDecl *fnDecl{scmain.getSCMainFunctionDecl()};

    // TODO: find any instances in sc_main.

    // fnDecl->dump();

    FindSimTime scstart{fnDecl, os_};
    systemcModel_->addSimulationTime(scstart.returnSimTime());
  } else {
    os_ << "\n Could not find SCMain";
  }

  ////////////////////////////////////////////////////////////////
  // Find the netlist.
  ////////////////////////////////////////////////////////////////
  // This actually also finds instances, but now we have AST matchers to do it.
  
  /*
  FindNetlist findNetlist{scmain.getSCMainFunctionDecl()};
  findNetlist.dump();
  systemcModel_->addNetlist(findNetlist);
  */

  // 
  // Create a ModuleDecl for each instance with the appropriately parsed ModuleDecl.
  //

  auto found_instances_map{ module_declaration_handler.getInstances() };
  // Go through each instance and find its appropriate module declaration.

  os_ << "## Print INSTANCE MAP\n";

  for ( const auto & inst : found_instances_map ) {
    auto cxx_decl{ inst.first };

    // Vector
    auto instance_list{ inst.second };

    os_ << "CXXRecordDecl " << cxx_decl;
    for ( const auto & val : instance_list ) {
    os_ <<  ", instance name: " << get<0>(val) << ", Decl " << get<1>(val) << "\n";
    //get<1>(val)->dump();
    }
    os_ << "\n";

  }
  ////////////////////////////////////////////////////////////////
  // Figure out the module map.
  ////////////////////////////////////////////////////////////////
  Model::moduleMapType moduleMap{systemcModel_->getModuleDecl()};

  // <string, ModuleDecl*>
  for (Model::moduleMapType::iterator mit = moduleMap.begin(),
                                      mitend = moduleMap.end();
       mit != mitend; mit++) {
    ModuleDecl *mainmd{ mit->second };
    int numInstances{ mainmd->getNumInstances() };
    vector<ModuleDecl *> moduleDeclVec;

    os_ << "\nFor module: " << mit->first << " num instance : " << numInstances << "\n";

    for (unsigned int num{0}; num < numInstances; ++num) {
      auto md{ new ModuleDecl{*mainmd} };

      // Find the template arguments for the class.
      FindTemplateParameters tparms{mainmd->getModuleClassDecl(), os_};

      md->setTemplateParameters(tparms.getTemplateParameters());
      md->dump_json();

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
    systemcModel_->addModuleDeclInstances(mainmd, moduleDeclVec);
  }

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
     */

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

  os_ << "\n";
  os_ << "\n## SystemC model\n";
  systemcModel_->dump(os_);
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

Model *SystemCConsumer::getSystemCModel() { return systemcModel_; }

std::string SystemCConsumer::getTopModule() const { return top_; }

ASTContext &SystemCConsumer::getContext() const { return context_; }

SourceManager &SystemCConsumer::getSourceManager() const { return sm_; }
