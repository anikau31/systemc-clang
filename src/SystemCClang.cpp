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

bool SystemCConsumer::fire()
{

	TranslationUnitDecl *tu = _context.getTranslationUnitDecl();

	_systemcModel = new Model();

	SCModules scmod(tu, _os);

	// ANI : Do we need FindGlobalEvents?
	FindGlobalEvents fglobals(tu, _os);
	FindGlobalEvents::globalEventMapType eventMap = fglobals.getEventMap();
	_systemcModel->addGlobalEvents(eventMap);

	SCModules::moduleMapType scmodules = scmod.getSystemCModulesMap();

	for (SCModules::moduleMapType::iterator mit = scmodules.begin(),
		 mitend = scmodules.end(); mit != mitend; mit++)
	{

		ModuleDecl *md = new ModuleDecl(mit->first, mit->second);
	 vector < EntryFunctionContainer * >_entryFunctionContainerVector;
		
  FindConstructor constructor(mit->second, _os);
		md->addConstructor(constructor.returnConstructorStmt());

		FindPorts ports(mit->second, _os);
		md->addInputPorts(ports.getInputPorts());
		md->addOutputPorts(ports.getOutputPorts());
		md->addInputOutputPorts(ports.getInputOutputPorts());
  
  FindTLMInterfaces findTLMInterfaces(mit->second, _os);
  md->addInputInterfaces(findTLMInterfaces.getInputInterfaces());
  md->addOutputInterfaces(findTLMInterfaces.getOutputInterfaces());
  md->addInputOutputInterfaces(findTLMInterfaces.getInputOutputInterfaces());

		FindSignals signals(mit->second, _os);
		md->addSignals(signals.getSignals());

		FindEntryFunctions findEntries(mit->second, _os);
		FindEntryFunctions::entryFunctionVectorType * entryFunctions =
			findEntries.getEntryFunctions();
		findEntries.dump();

  md->addProcess(entryFunctions);

		for (unsigned int i = 0; i < entryFunctions->size(); i++)
		{
			EntryFunctionContainer *ef = (*entryFunctions)[i];

			FindSensitivity
				findSensitivity(constructor.returnConstructorStmt(), _os);
			ef->addSensitivityInfo(findSensitivity);

			if (ef->getEntryMethod() == NULL)
			{
				_os << "ERROR";
				continue;
			}

			FindWait findWaits(ef->getEntryMethod(), _os);
			ef->addWaits(findWaits);

			FindNotify findNotify(ef->_entryMethodDecl, _os);
			ef->addNotifys(findNotify);
   
   //FindGPUMacro findGPUMacro(ef->_entryMethodDecl, _os);
   //findGPUMacro.dump(); 

   SuspensionAutomata suspensionAutomata(findWaits.getWaitCalls(), ef->getEntryMethod(), &_context, llvm::errs());
   suspensionAutomata.initialize();
   suspensionAutomata.genSusCFG();
   suspensionAutomata.genSauto();
   
   ef->addSusCFGAuto(suspensionAutomata); 
			_entryFunctionContainerVector.push_back(ef);
		}
		_systemcModel->addModuleDecl(md);

	}

	FindSCMain scmain(tu, _os);

	if (scmain.isSCMainFound())
	{
		FunctionDecl *fnDecl = scmain.getSCMainFunctionDecl();

		FindSimTime scstart(fnDecl, _os);
		_systemcModel->addSimulationTime(scstart.returnSimTime());

	}
	FindNetlist findNetlist(scmain.getSCMainFunctionDecl());
 _systemcModel->addNetlist(findNetlist); 
 
 return true;
}

void SystemCConsumer::HandleTranslationUnit(ASTContext & context)
{
	// ///////////////////////////////////////////////////////////////
	// / Pass 1: Find the necessary information.
	// ///////////////////////////////////////////////////////////////

	bool pre = false;

	pre = preFire();

	if (!pre)
	{
		return;
	}

	bool f = false;

	f = fire();

	if (!f)
	{
		return;
	}
	postFire();
}

SystemCConsumer::SystemCConsumer(CompilerInstance & ci):_os(llvm::errs()), _sm(ci.getSourceManager()), _ci(ci),
_context(ci.getASTContext()),
_systemcModel(NULL)
{

}

SystemCConsumer::~SystemCConsumer()
{
	if (_systemcModel != NULL)
	{
		delete _systemcModel;
	}
}
