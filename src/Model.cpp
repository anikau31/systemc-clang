#include <string>
#include "Model.h"
#include "SCModules.h"
using namespace scpar;
using namespace std;

Model::Model(){

}

Model::~Model() {
  llvm::errs() << "\n[[ Destructor Model ]]\n";
  // Delete all ModuleDecl pointers.
  for (Model::moduleMapType::iterator mit = _modules.begin();
       mit != _modules.end(); mit++) {
    // Second is the ModuleDecl type.
    delete mit->second;
  }
  _modules.clear();
  
  for (Model::signalInstanceContainerMapType::iterator sit = _signals.begin(), site = _signals.end();
       sit != site;
       sit++) {
    delete sit->second;
  }
  _signals.clear();

  for (Model::portBindMapType::iterator it = _portBinds.begin(), eit = _portBinds.end();
       it != eit;
       it++) {
    delete it->second;
  }
  _portBinds.clear();
}

Model::Model(const Model& from) {
  // Disallowed for now.
  _modules = from._modules;
}

void Model::addModuleDecl(ModuleDecl* md) {
  // Add the <name, md> in the map.
  _modules.insert(Model::modulePairType(md->getName(), md));
}

void Model::addSignalInstances(Model::signalInstanceContainerMapType sm) {
  for (Model::signalInstanceContainerMapType::iterator sit = sm.begin(), site = sm.end();
       sit!=site;
       sit++) {
    _signals.insert(Model::signalInstanceContainerPairType(sit->first, sit->second));

  }
}
void Model::addSimulationTime(FindSimTime::simulationTimeMapType simTime){

	_simTime = simTime;
}

void Model::addNetlist(NetlistGen::instanceModuleMapType instanceModuleMap, NetlistGen::signalPortTypeMapType signalPortTypeMap, NetlistGen::netlistMapType netlistMap){

	_instanceModuleMap = instanceModuleMap;
	_signalPortTypeMap = signalPortTypeMap;
	_netlistMap = netlistMap;

}

void Model::addPortBindings(Model::portBindMapType pb) {
  // This should invoke the copy constructor ??
  // for (Model::portBindMapType::iterator it = pb.begin(), eit = pb.end();
  //      it != eit;
  //      it++) {
  //   _portBinds.insert(Model::portBindPairType(it->first, it->second));
  // }
  _portBinds = pb;
}
void Model::addGlobalEvents(FindGlobalEvents::globalEventMapType eventMap) {
	for (FindGlobalEvents::globalEventMapType::iterator it = eventMap.begin();
		it != eventMap.end();
		it++) {
		string eventName = it->first;
		EventContainer *event = new EventContainer(eventName, it->second);
		_eventMap.insert(eventPairType(eventName, event));	
	}
}

void Model::addSCModules(SCModules* m) {
  SCModules::moduleMapType mods = m->getSystemCModulesMap();

  for (SCModules::moduleMapType::iterator mit = mods.begin();
       mit != mods.end();
       mit++) {
    addModuleDecl(new ModuleDecl(mit->first, mit->second));
  }
}

void Model::addEntryFunctionContainerVector(vector<EntryFunctionContainer*> entryFunctionContainer) {
	_entryFunctionContainerVector = entryFunctionContainer;
}

Model::moduleMapType Model::getModuleDecl() {
	return _modules;
}

Model::eventMapType Model::getEventMapType() {
	return _eventMap;
}

NetlistGen::instanceModuleMapType Model::getInstanceModuleMap() {
	return _instanceModuleMap;
}

NetlistGen::signalPortTypeMapType Model::getSignalPortTypeMap() {
	return _signalPortTypeMap;
}

NetlistGen::netlistMapType Model::getNetlistMap() {
	return _netlistMap;
}				

vector<EntryFunctionContainer*> Model::getEntryFunctionContainerVector() {
	return _entryFunctionContainerVector;
}

unsigned int Model::getNumEvents() {
	return (_eventMap.size() - 3); // three systemC event generic types are added to this map. Only want user defined sc_events
}	
void Model::dump(raw_ostream& os) {

  int counterModel = 0;
  os << "\nNumber of Models : "<<_modules.size();

  for (Model::moduleMapType::iterator mit = _modules.begin();
       mit != _modules.end(); mit++) {
    // Second is the ModuleDecl type.

    os <<"\nModel "<<counterModel++;
    mit->second->dump(os);
  }
  os <<"\nGlobal Events:\n";
  for (Model::eventMapType::iterator it = _eventMap.begin(), ite = _eventMap.end();
		it != ite;
		it++) {
		
		os <<"   Event: "<<it->first<<"  VarDecl: "<<it->second<<"\n";
	}
  
  os << "\nSignals:\n";
  for (Model::signalInstanceContainerMapType::iterator sit = _signals.begin(), site = _signals.end();
       sit != site;
       sit++) {
    // Second is the SignalInstanceContainer type
    sit->second->dump(os);
  }
  
  os << "\nPort bindings:\n";
  for (Model::portBindMapType::iterator it = _portBinds.begin(), eit = _portBinds.end();
       it != eit;
       it++) {
    it->second->dump(os);
  }

  
  os <<"\nNetlist :";
  os << "\n ======================= Begin netlist ====================";

  for (NetlistGen::netlistMapType::iterator it = _netlistMap.begin(), eit = _netlistMap.end();
        it != eit;
        it++) {
    NetlistGen::instanceModulePairType tmpInstanceModulePair = it->first;
    NetlistGen::signalPortPairType tmpSignalPortPair = it->second;

    os <<"\n Module instance : " <<it->first.first<<" Module Name : " <<it->first.second<< " Signal Name : " <<it->second.first<<" Port Name : " <<it->second.second;
    NetlistGen::signalPortTypeMapType::iterator signalFound = _signalPortTypeMap.find(it->second);
    os <<" Port Type : " <<signalFound->second;

  }
  os << "\n ======================= End netlist ====================";

  os<<"\nSimulation Time : ";
  for(FindSimTime::simulationTimeMapType::iterator it = _simTime.begin(), eit = _simTime.end();
		it != eit;
		it++) {		
		os <<it->first<<" "<<it->second;
	}
}

