#include "SystemCClang.h"
using namespace scpar;
using namespace clang;
using namespace std;

Model *SystemCConsumer::getSystemCModel() {
	return _systemcModel;
}

bool SystemCConsumer::preFire() {
	return true;
}

bool SystemCConsumer::postFire() {
	return true;
}

bool SystemCConsumer::fire() {

	TranslationUnitDecl *tu = _context.getTranslationUnitDecl();
  // Reflection database.
	_systemcModel = new Model();

  // Find the sc_modules 
	SCModules scmod(tu, _os);

	// ANI : Do we need FindGlobalEvents?
	FindGlobalEvents fglobals(tu, _os);
	FindGlobalEvents::globalEventMapType eventMap = fglobals.getEventMap();
	_systemcModel->addGlobalEvents(eventMap);


	SCModules::moduleMapType scmodules = scmod.getSystemCModulesMap();

  for ( SCModules::moduleMapType::iterator mit = scmodules.begin(),
          mitend = scmodules.end(); mit != mitend; ++mit )  {
    ModuleDecl *md = new ModuleDecl(mit->first, mit->second);
    _systemcModel->addModuleDecl(md);
  }



	////////////////////////////////////////////////////////////////
  // Find the sc_main
  ////////////////////////////////////////////////////////////////
	FindSCMain scmain(tu, _os);

	if ( scmain.isSCMainFound() ) {
      FunctionDecl *fnDecl = scmain.getSCMainFunctionDecl();

      FindSimTime scstart(fnDecl, _os);
      _systemcModel->addSimulationTime(scstart.returnSimTime());
  }	else {
		_os <<"\n Could not find SCMain";
	}

  ////////////////////////////////////////////////////////////////
  // Find the netlist.
  ////////////////////////////////////////////////////////////////
	FindNetlist findNetlist(scmain.getSCMainFunctionDecl());
 	findNetlist.dump();
	_systemcModel->addNetlist(findNetlist);


	////////////////////////////////////////////////////////////////
  // Figure out the module map.
  ////////////////////////////////////////////////////////////////
	Model::moduleMapType moduleMap = _systemcModel->getModuleDecl();

	for (Model::moduleMapType::iterator mit = moduleMap.begin(), mitend = moduleMap.end();   mit != mitend;   mit++) {
		ModuleDecl *mainmd = mit->second;
		int numInstances = mainmd->getNumInstances();
		vector<ModuleDecl*> moduleDeclVec;

    _os << "\n";
		_os <<"For module: " << mit->first <<" num instance : " << numInstances;

		for (unsigned int num {0}; num < numInstances; ++num ) {
			ModuleDecl *md = new ModuleDecl();
			vector < EntryFunctionContainer * >_entryFunctionContainerVector;
	  	FindConstructor constructor(mainmd->getModuleClassDecl(), _os);
			md->addConstructor(constructor.returnConstructorStmt());

			FindPorts ports(mainmd->getModuleClassDecl(), _os);
			md->addInputPorts(ports.getInputPorts());
			md->addOutputPorts(ports.getOutputPorts());
			md->addInputOutputPorts(ports.getInputOutputPorts());

	  	FindTLMInterfaces findTLMInterfaces(mainmd->getModuleClassDecl(), _os);
	  	md->addInputInterfaces(findTLMInterfaces.getInputInterfaces());
	  	md->addOutputInterfaces(findTLMInterfaces.getOutputInterfaces());
	  	md->addInputOutputInterfaces(findTLMInterfaces.getInputOutputInterfaces());

			FindSignals signals(mainmd->getModuleClassDecl(), _os);
			md->addSignals(signals.getSignals());

			FindEntryFunctions findEntries(mainmd->getModuleClassDecl(), _os);
			FindEntryFunctions::entryFunctionVectorType * entryFunctions = findEntries.getEntryFunctions();
	  	md->addProcess(entryFunctions);

			for (size_t i = 0; i < entryFunctions->size(); i++)  {
          EntryFunctionContainer *ef = (*entryFunctions)[i];
          FindSensitivity findSensitivity(constructor.returnConstructorStmt(), _os);
          ef->addSensitivityInfo(findSensitivity);

          if (ef->getEntryMethod() == nullptr)  {
            _os << "ERROR";
            continue;
          }

          FindWait findWaits(ef->getEntryMethod(), _os);
          ef->addWaits(findWaits);

          FindNotify findNotify(ef->_entryMethodDecl, _os);
          ef->addNotifys(findNotify);

          /*
            SuspensionAutomata suspensionAutomata(findWaits.getWaitCalls(), ef->getEntryMethod(), &_context, llvm::errs());
            if (suspensionAutomata.initialize()) {
            suspensionAutomata.genSusCFG();
            //suspensionAutomata.dumpSusCFG();
            suspensionAutomata.genSauto();    
            //suspensionAutomata.dumpSauto();
            ef->addSusCFGAuto(suspensionAutomata); 
            }
          */ 
          _entryFunctionContainerVector.push_back(ef);
			
        }
			moduleDeclVec.push_back(md);
		}
    _systemcModel->addModuleDeclInstances(mainmd, moduleDeclVec);
	}
	
	/*
    FindSCMain scmain(tu, _os);

    if (scmain.isSCMainFound())
    {
		FunctionDecl *fnDecl = scmain.getSCMainFunctionDecl();

		FindSimTime scstart(fnDecl, _os);
		_systemcModel->addSimulationTime(scstart.returnSimTime());

    }
    else {
		_os <<"\n Could not find SCMain";
    }
    FindNetlist findNetlist(scmain.getSCMainFunctionDecl());
    findNetlist.dump();
    _systemcModel->addNetlist(findNetlist); 
	*/
	// Generate SAUTO
	// Placing it here so that unique SAUTO for each instance
	//Model::moduleMapType moduleMap = _systemcModel->getModuleDecl();
	Model::moduleInstanceMapType moduleInstanceMap = _systemcModel->getModuleInstanceMap();

	for (Model::moduleInstanceMapType::iterator it = moduleInstanceMap.begin(), eit = moduleInstanceMap.end();
       it != eit;  it++) {
		vector<ModuleDecl*> moduleDeclVec = it->second;
		for (size_t i = 0; i < moduleDeclVec.size(); i++) {
			ModuleDecl *moduleDecl = moduleDeclVec.at(i);
			vector<EntryFunctionContainer*> entryFunctionContainer = moduleDecl->getEntryFunctionContainer();
			for (size_t j = 0; j < entryFunctionContainer.size(); j++) {
		
   			SuspensionAutomata suspensionAutomata(entryFunctionContainer.at(j)->getWaitCalls(), entryFunctionContainer.at(j)->getEntryMethod(), &_context, llvm::errs());
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

  _os <<"\n";
  _os << "\n## SystemC model\n";
  _systemcModel->dump(_os); 
  return true;
}

void SystemCConsumer::HandleTranslationUnit(ASTContext & context) {
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

	if (!f) 	{
		return;
	}
	postFire();
}

SystemCConsumer::SystemCConsumer(CompilerInstance & ci) :
  _os(llvm::errs()),
  _sm(ci.getSourceManager()),
  _ci(ci),
  _context(ci.getASTContext()),
  _systemcModel(nullptr) {

}

SystemCConsumer::~SystemCConsumer() {
	if ( _systemcModel != nullptr )	{
		delete _systemcModel;
    _systemcModel = nullptr;
	}
}
