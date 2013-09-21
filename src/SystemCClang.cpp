#include "SystemCClang.h"
using namespace scpar;
using namespace clang;
using namespace std;

Model* SystemCConsumer::getSystemCModel() {
  return _systemcModel;
}

bool SystemCConsumer::preFire() {

  return true;
}

bool SystemCConsumer::postFire() {
  
  return true;
}

bool SystemCConsumer::fire() {
  
  /// Find the FunctionDecl* that points to the sc_main.
  /// There can only be one sc_main so do it independently.
  TranslationUnitDecl* tu = _context.getTranslationUnitDecl();
  // Create the datastructure.
  _systemcModel = new Model();
  
  /// Get all the SystemC Modules.

	SCModules scmod(tu, _os);
  	 
  /// Get all the global variable declaration of sc_event types.
  //need to put global events in systemc model
  FindGlobalEvents fglobals(tu, _os);
  FindGlobalEvents::globalEventMapType eventMap = fglobals.getEventMap();
		
	//fglobals.dump();
  _systemcModel->addGlobalEvents(eventMap); 
  //reflectionDataStructure scmodules = scmod.getSystemCModulesMap();
  SCModules::moduleMapType scmodules = scmod.getSystemCModulesMap();

	vector<EntryFunctionContainer *> _entryFunctionContainerVector;

  for (SCModules::moduleMapType::iterator mit = scmodules.begin(),
       mitend = scmodules.end(); mit != mitend; mit++) {      
    // This is where we should calling all functions.  But we need to remove the reflectionStructure.

    // Create a ModuleDecl.
    ModuleDecl* md = new ModuleDecl(mit->first, mit->second);
    
    // Find the ports.

		FindPorts ports(mit->second, _os);
    // ports.dump();
        
    // Add ports to ModuleDecl.
    md->addInputPorts(ports.getInputPorts());
    md->addOutputPorts(ports.getOutputPorts());
    md->addInputOutputPorts(ports.getInputOutputPorts());
    
    //Find sc_signalsi

    FindSignals signals(mit->second, _os);
    // Add signals to ModuleDecl.
    md->addSignals(signals.getSignals());

    // Get all the in-class sc_events.

    FindEvents findEvents(mit->second, _os);
    // FindEvents.dump();
    // Add Events to ModuleDecl
    md->addEvents(findEvents.getInClassEvents());
    
    // Get all the in-class data members (variables) and their size.

		FindClassDataMembers findClassMembers(mit->second,_os);
    //findClassMembers.dump();

    // Add class data members to ModuleDecl
    md->addDataMembers(mit->first, findClassMembers.getInClassMembers(), 
		findClassMembers.getClassMemberSize(), _os); 

		md->addSockets(findClassMembers.getSocketCharacteristicsMap());
    // Find the entry function.

    FindEntryFunctions findEntries(mit->second, _os);
    //findEntries.dump();
		FindEntryFunctions::entryFunctionVectorType* entryFunctions = 
		findEntries.getEntryFunctions();
		// Add the entry functions to ModuleDecl
 
 		vector<CXXMethodDecl *> otherFunctions = findEntries.getOtherFunctions();
		FindSocketBinding findSocketBinding(mit->second, _os);
		//findSocketBinding.dump();

		md->addProcess(entryFunctions);   
		md->addInitiatorTargetSocketBind(findSocketBinding.getInitiatorTargetSocketMap());

		string functionName = findEntries.getEntryName();

		
    for(unsigned int i = 0; i< entryFunctions->size(); i++) {
      EntryFunctionContainer* ef = (*entryFunctions)[i];

      // Get the sensitivity list.      
			FindSensitivity findSensitivity(ef->getConstructorStmt(), _os);
			//findSensitivity.dump();
      if (ef->getEntryMethod() == NULL) {
        _os << "ERROR";
				continue;
      }

      FindWait findWaits(ef->getEntryMethod(), _os); 
			//findWaits.dump();
			// Insert the found waits into the entry function.
			ef->addWaits(findWaits);
			
			FindThreadHierarchy findThreads(ef->getEntryMethod(), _os);
			//findThreads.dump();
			ef->addThreadHierarchy(findThreads);

			FindLoopTime findLoopTime(ef->getEntryMethod(), _os);
			//findLoopTime.dump();
     	ef->addLoopTime(findLoopTime);
			// Add wait calls to ModuleDecl
			md->addWaitCalls(mit->first, ef->getName(), findWaits.getWaitCalls(), 
			findWaits.getWaitTimesMap(), findWaits.getProcessWaitEventMap());
      
			FindSocketTransportType findSocketTransportType(ef->getEntryMethod(), 
			_os);
		//	findSocketTransportType.dump();
			ef->addSocketTransportMap(findSocketTransportType);

			FindPayloadCharacteristics payloadCharacteristics(ef->getEntryMethod(), _os);
			//payloadCharacteristics.dump(_os);

			ef->addPayloadCharacteristicsMap(payloadCharacteristics);
			// Find notify calls      
			FindNotify findNotify(ef->_entryMethodDecl, _os);
			//findNotify.dump();
			ef->addNotifys(findNotify);
			// Add notify calls to ModuleDecl
			md->addNotifyCalls(mit->first, ef->getName(), findNotify.getNotifyCallList(), findNotify.getProcessNotifyEventMap());

			// Find Method local Variables
      FindMethodLocalVariables localVars(ef->_entryMethodDecl,_os);
      //localVars.dump();
			ef->addLocalVariables(localVars);
			// Find Member functions 
      FindMemberFunctions findMemberFuncts(ef->_entryMethodDecl, _os);
 			//findMemberFuncts.dump();

			FindMemberFunctions::functionMapType functionMap = findMemberFuncts.returnFunctionMap();
			// Adding member functions to ModuleDecl
      md->addMemberFunctions(functionMap); 
     	//need to remove this and make this stand alone 
			FindMemberFunctionVariables findFunctionVariables(ef->_entryMethodDecl, _os);
		
			findFunctionVariables.dump();
			FindPortTracing findPortTracing(ef->_entryMethodDecl, _os);
			//findPortTracing.dump();
			ef->addPortTrace(findPortTracing);
				
			FindPortReadWrites portOperations(ef->_entryMethodDecl, _os);
   		//portOperations.dump();
			_entryFunctionContainerVector.push_back(ef); 
			//ef->dump(_os, 0);
		
		}
    _systemcModel->addModuleDecl(md);   
		_systemcModel->addEntryFunctionContainerVector(_entryFunctionContainerVector); 

	}

  FindSCMain scmain(tu, _os);
  if (scmain.isSCMainFound()) {
    FunctionDecl* fnDecl = scmain.getSCMainFunctionDecl();
    //fnDecl->getBody()->dump();
    
    FindSignalInstances signalInstances(fnDecl);

 		//signalInstances.dump(_os);
    _systemcModel->addSignalInstances(signalInstances.getSignalInstances());
    // FIXME: This throws an assertion.
    FindSimTime scstart(fnDecl, _os); 
   	//_systemcModel->addscstart.returnSimTime();
    FindModuleInstances moduleInstances(fnDecl);
    //moduleInstances.dump(_os, 0);
    
    FindPortBinding bind(fnDecl, _os);
 		//bind.dump(_os);
    _systemcModel->addPortBindings(bind.getPortBinds());
		//bind.dump(_os);
  
    NetlistGen netlist(fnDecl,  _os);
		//netlist.dump();

		SocketBindGen socketBindGen(fnDecl, _os);
		//socketBindGen.dump();
		_systemcModel->addNetlist(netlist.getInstanceModuleMap(), netlist.getSignalPortTypeMap(), netlist.getNetlistMap());

		_systemcModel->addSimulationTime(scstart.returnSimTime());
}
  _os << "\n^^^^^^^^^^^^^^^^^^^^  BEGIN DUMP ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n";

  _systemcModel->dump(_os);

  _os << "\n^^^^^^^^^^^^^^^^^^^^  END DUMP   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n";

	/////////////////////////// Take the _systemcModel and generate the graph for 
	//entire model /////////////////////////////////////////////////////////////

  return true;
}

void SystemCConsumer::HandleTranslationUnit(ASTContext& context) {
  /////////////////////////////////////////////////////////////////
  /// Pass 1: Find the necessary information.
  /////////////////////////////////////////////////////////////////
  
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

SystemCConsumer::SystemCConsumer(CompilerInstance& ci)
: _os(llvm::errs())
, _sm(ci.getSourceManager())
, _ci(ci)
, _context(ci.getASTContext())
, _systemcModel(NULL)
 
{

}

SystemCConsumer::~SystemCConsumer() {
  if (_systemcModel != NULL) {
    delete _systemcModel;
  }
}

////////////////////////////////////////////////////////////////////////////////////////
// LightsCameraAction

//ASTConsumer* LightsCameraAction::CreateASTConsumer(CompilerInstance& ci, StringRef) {
//return new SystemCConsumer(ci);
///}
