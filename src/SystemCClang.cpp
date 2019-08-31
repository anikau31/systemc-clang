#include "SystemCClang.h"
using namespace scpar;
using namespace clang;
using namespace std;

Model *SystemCConsumer::getSystemCModel() { return _systemcModel; }

bool SystemCConsumer::preFire() { return true; }

bool SystemCConsumer::postFire() { return true; }

bool SystemCConsumer::fire() {

  TranslationUnitDecl *tu{_context.getTranslationUnitDecl()};
  // Reflection database.
  _systemcModel = new Model{} ;

  // ANI : Do we need FindGlobalEvents?
  FindGlobalEvents globals{tu, os_};
  FindGlobalEvents::globalEventMapType eventMap{globals.getEventMap()};
  globals.dump_json();
  _systemcModel->addGlobalEvents(eventMap);

  // Find the sc_modules
  FindSCModules scmod{tu, os_};

  FindSCModules::moduleMapType scmodules{scmod.getSystemCModulesMap()};

  for (FindSCModules::moduleMapType::iterator mit = scmodules.begin(),
         mitend = scmodules.end();  mit != mitend; ++mit) {
    ModuleDecl *md = new ModuleDecl{mit->first, mit->second};
    // md->setTemplateParameters( scmod.getTemplateParameters() );
    //       os_ << "SIZE: " << scmod.getTemplateParameters().size() << "\n";
    _systemcModel->addModuleDecl(md);

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
    _systemcModel->addSimulationTime(scstart.returnSimTime());
  } else {
    os_ << "\n Could not find SCMain";
  }

  ////////////////////////////////////////////////////////////////
  // Find the netlist.
  ////////////////////////////////////////////////////////////////
  FindNetlist findNetlist{scmain.getSCMainFunctionDecl()};
  findNetlist.dump();
  _systemcModel->addNetlist(findNetlist);

  ////////////////////////////////////////////////////////////////
  // Figure out the module map.
  ////////////////////////////////////////////////////////////////
  Model::moduleMapType moduleMap{_systemcModel->getModuleDecl()};

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
      os_ << "@@# " << mainmd->getTemplateParameters().size() << "\n";
      md->dump_json();


      vector<EntryFunctionContainer *> _entryFunctionContainerVector;
      FindConstructor constructor{mainmd->getModuleClassDecl(), os_};
      md->addConstructor(constructor.returnConstructorStmt());

      FindPorts ports{mainmd->getModuleClassDecl(), os_};
      md->addInputPorts(ports.getInputPorts());
      md->addOutputPorts(ports.getOutputPorts());
      md->addInputOutputPorts(ports.getInputOutputPorts());
      md->addOtherVars(ports.getOtherVars()); 

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
                                              ef->getEntryMethod(), &_context,
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
    _systemcModel->addModuleDeclInstances(mainmd, moduleDeclVec);
  }

  /*
FindSCMain scmain(tu, os_);

if (scmain.isSCMainFound())
{
          FunctionDecl *fnDecl = scmain.getSCMainFunctionDecl();

          FindSimTime scstart(fnDecl, os_);
          _systemcModel->addSimulationTime(scstart.returnSimTime());

}
else {
          os_ <<"\n Could not find SCMain";
}

FindNetlist findNetlist(scmain.getSCMainFunctionDecl());
findNetlist.dump();
_systemcModel->addNetlist(findNetlist);
*/

// Only do this if SAUTO flag is set.
#ifdef USE_SAUTO
  // Generate SAUTO
  // Placing it here so that unique SAUTO for each instance
  // Model::moduleMapType moduleMap = _systemcModel->getModuleDecl();
  Model::moduleInstanceMapType moduleInstanceMap =
      _systemcModel->getModuleInstanceMap();

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
            entryFunctionContainer.at(j)->getEntryMethod(), &_context,
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
  _systemcModel->dump(os_);
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

SystemCConsumer::SystemCConsumer(CompilerInstance &ci)
    : os_{llvm::errs()},
      _sm{ci.getSourceManager()},
      _context{ci.getASTContext()},
      _ci{ci},
      _systemcModel{nullptr} {}

SystemCConsumer::~SystemCConsumer() {
  if (_systemcModel != nullptr) {
    delete _systemcModel;
    _systemcModel = nullptr;
  }
}
