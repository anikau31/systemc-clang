#include "SystemCClang.h"
using namespace scpar;
using namespace clang;
using namespace std;

bool SystemCConsumer::preFire() { return true; }

bool SystemCConsumer::postFire() { return true; }

bool SystemCConsumer::fire() {

  os_ << "Top module: " << getTopModule() << "\n";
  TranslationUnitDecl *tu{getContext().getTranslationUnitDecl()};
  // Reflection database.
  systemcModel_ = new Model{} ;

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

  // Find the sc_modules
  //
  FindSCModules scmod{tu, os_};

  FindSCModules::moduleMapType scmodules{scmod.getSystemCModulesMap()};

  for (FindSCModules::moduleMapType::iterator mit = scmodules.begin(),
         mitend = scmodules.end();  mit != mitend; ++mit) {
    ModuleDecl *md = new ModuleDecl{mit->first, mit->second};
    // md->setTemplateParameters( scmod.getTemplateParameters() );
    //       os_ << "SIZE: " << scmod.getTemplateParameters().size() << "\n";
    systemcModel_->addModuleDecl(md);

    //
    // TODO: find any instances in the module declarations
    os_ << "=> Processing module: " << mit->first << "\n";
    //md->getModuleClassDecl()->dump();
    FindModuleInstance module_instance{md->getModuleClassDecl(), os_};
  }


  ////////////////////////////////////////////////////////////////
  // Find the sc_main
  ////////////////////////////////////////////////////////////////
  FindSCMain scmain{tu, os_};

  if (scmain.isSCMainFound()) {
    FunctionDecl *fnDecl{scmain.getSCMainFunctionDecl()};

  // TODO: find any instances in sc_main.

    //fnDecl->dump();

    FindSimTime scstart{fnDecl, os_};
    systemcModel_->addSimulationTime(scstart.returnSimTime());
  } else {
    os_ << "\n Could not find SCMain";
  }

  ////////////////////////////////////////////////////////////////
  // Find the netlist.
  ////////////////////////////////////////////////////////////////
  FindNetlist findNetlist{scmain.getSCMainFunctionDecl()};
  findNetlist.dump();
  systemcModel_->addNetlist(findNetlist);

  ////////////////////////////////////////////////////////////////
  // Figure out the module map.
  ////////////////////////////////////////////////////////////////
  Model::moduleMapType moduleMap{systemcModel_->getModuleDecl()};

  for (Model::moduleMapType::iterator mit = moduleMap.begin(),
         mitend = moduleMap.end();   mit != mitend; mit++) {
    ModuleDecl *mainmd{mit->second};
    int numInstances{mainmd->getNumInstances()};
    vector<ModuleDecl *> moduleDeclVec;

    os_ << "\n";
    os_ << "For module: " << mit->first << " num instance : " << numInstances;

    for (unsigned int num{0}; num < numInstances; ++num) {
      ModuleDecl *md = new ModuleDecl{};

      // Find the template arguments for the class.
      FindTemplateParameters tparms{mainmd->getModuleClassDecl(), os_};

      md->setTemplateParameters( tparms.getTemplateParameters() );
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
      md->addInputStreamPorts( ports.getInStreamPorts() );
      md->addOutputStreamPorts( ports.getOutStreamPorts() );

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
                                              ef->getEntryMethod(), &getContext(),
                                              llvm::errs());
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
          //suspensionAutomata.dumpSusCFG();
          suspensionAutomata.genSauto();
          //suspensionAutomata.dumpSauto();
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

SystemCConsumer::SystemCConsumer( CompilerInstance &ci, std::string top )
  : os_{llvm::errs()}, sm_{ci.getSourceManager()},
  context_{ci.getASTContext()},
  ci_{ci},
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

ASTContext& SystemCConsumer::getContext() const { return context_; }
  
SourceManager& SystemCConsumer::getSourceManager() const { return sm_; }
