#include <string>
#include "Model.h"
#include "SCModules.h"

using namespace scpar;
using namespace std;

//#include "json.hpp"
//using json = nlohmann:json;

Model::Model() {

}

Model::~Model() {
  //  llvm::errs() << "\n[[ Destructor Model ]]\n";
  // Delete all ModuleDecl pointers.
  for (Model::moduleMapType::iterator mit = _modules.begin();
       mit != _modules.end(); mit++) {
    // Second is the ModuleDecl type.
    delete mit->second;
  }
  _modules.clear();
}

Model::Model( const Model & from ) {
  _modules = from._modules;
}

void Model::addModuleDecl( ModuleDecl * md ) {
  _modules.insert( Model::modulePairType( md->getName(), md ) );
}

void Model::addModuleDeclInstances( ModuleDecl* md, vector<ModuleDecl*> mdVec ) {
	_moduleInstanceMap.insert(moduleInstancePairType(md, mdVec));
}

void Model::addSimulationTime(FindSimTime::simulationTimeMapType simTime) {
  _simTime = simTime;
}

void Model::addEntryFunctionGPUMacroMap(entryFunctionGPUMacroMapType e) {
	//_entryFunctionGPUMacroMap.insert(e.begin(), e.end());
	_entryFunctionGPUMacroMap = e;
	llvm::errs()<<" \n Size : " <<_entryFunctionGPUMacroMap.size()<<" " <<e.size();
}


void Model::addGlobalEvents(FindGlobalEvents::globalEventMapType eventMap) {
  for (FindGlobalEvents::globalEventMapType::iterator it = eventMap.begin();
       it != eventMap.end(); it++) {
    string eventName = it->first;
    EventContainer *event = new EventContainer(eventName, it->second);

    _eventMap.insert(eventPairType(eventName, event));
  }
}

void Model::addSCMain(FunctionDecl *fnDecl) {
  _scmainFcDecl = fnDecl;
}

void Model::addNetlist( FindNetlist &n ) {
  _instanceModuleMap = n.getInstanceModuleMap();
  _portSignalMap = n.getPortSignalMap();
  _instancePortSignalMap = n.getInstancePortSignalMap();
  _instanceListModuleMap = n.getInstanceListModuleMap();

  updateModuleDecl();
}

void Model::updateModuleDecl() {

  for (moduleMapType::iterator it = _modules.begin(), eit = _modules.end();
       it != eit;  it++) {
    string moduleName = it->first;
    ModuleDecl *md = it->second;
    vector<string> instanceList;

    llvm::errs() << "Finding instances for " << moduleName << " declaration: ";
    if ( _instanceListModuleMap.find(moduleName) != _instanceListModuleMap.end() ) {
      FindNetlist::instanceListModuleMapType::iterator instanceListModuleMapFind = _instanceListModuleMap.find(moduleName);
      md->addInstances(instanceListModuleMapFind->second);

      // Print the names of all the instances
      for ( auto instance : instanceListModuleMapFind->second ) {
        llvm::errs() << instance << " ";
      }
      llvm::errs() << "\n";

      for (size_t i = 0 ; i < instanceListModuleMapFind->second.size(); i++) {
        if (_instancePortSignalMap.find(instanceListModuleMapFind->second.at(i)) != _instancePortSignalMap.end()) {
          FindNetlist::instancePortSignalMapType::iterator portSignalMapFound = _instancePortSignalMap.find(instanceListModuleMapFind->second.at(i));
          FindNetlist::portSignalMapType portSignalMap = portSignalMapFound->second;

          md->addSignalBinding(portSignalMap);
        }  else {
          llvm::errs() <<"\n Could not find instance and signal";
        }
      }
    } else {
      llvm::errs() <<"NONE.";
    }
  }
}

void Model::addSCModules(SCModules * m)
{
  SCModules::moduleMapType mods = m->getSystemCModulesMap();

  for (SCModules::moduleMapType::iterator mit = mods.begin();
       mit != mods.end(); mit++) {
    addModuleDecl(new ModuleDecl(mit->first, mit->second));
  }
}

Model::moduleMapType Model::getModuleDecl()
{
  return _modules;
}

Model::entryFunctionGPUMacroMapType Model::getEntryFunctionGPUMacroMap() {
	llvm::errs()<<"\n return Size : " <<_entryFunctionGPUMacroMap.size();
	return _entryFunctionGPUMacroMap;
}

Model::moduleInstanceMapType Model::getModuleInstanceMap() {
	return _moduleInstanceMap;
}

Model::eventMapType Model::getEventMapType()
{
  return _eventMap;
}

unsigned int Model::getNumEvents() {
  return (_eventMap.size() - 3);
}

void Model::dump( llvm::raw_ostream & os ) {

  os << "\n# Number of modules : " << _modules.size();

  for (Model::moduleMapType::iterator mit = _modules.begin();
       mit != _modules.end(); mit++) {
    // Second is the ModuleDecl type.

    vector<ModuleDecl*> instanceVec = _moduleInstanceMap[mit->second];
    os << "\n# Module " << mit->first << ": " << instanceVec.size() << " instances.";
		for (size_t i = 0; i < instanceVec.size(); i++) {
      //			os <<", instance: " << i + 1 << " ";
			instanceVec.at(i)->dump(os);
		}
  }
  os << "\n\n";
  os << "# Global events:\n";
  for (Model::eventMapType::iterator it = _eventMap.begin(), ite =
         _eventMap.end(); it != ite; it++) {
    os << "   Event: " << it->first << "  VarDecl: " << it->second << "\n";
  }

  os << "\n";
  os << "# Simulation time: ";
  for (FindSimTime::simulationTimeMapType::iterator it =
         _simTime.begin(), eit = _simTime.end(); it != eit; it++) {
    os << it->first << " " << it->second;
  }

  os << "\n\n";
  os <<"# Netlist: " ;
  for (FindNetlist::instanceModuleMapType::iterator it = _instanceModuleMap.begin(), eit = _instanceModuleMap.end(); it != eit; it++) {
    os << "\n";
    os << "Instance Name: " << it->first << ", module name : " << it->second;
		string instanceName = it->first;
		if (_instancePortSignalMap.find(instanceName) !=
        _instancePortSignalMap.end())	{
      FindNetlist::instancePortSignalMapType::iterator instancePortSignalMapFound =
				_instancePortSignalMap.find(instanceName);
      FindNetlist::portSignalMapType portSignalMap =
				instancePortSignalMapFound->second;
			for (FindNetlist::portSignalMapType::iterator pit =
             portSignalMap.begin(), pite = portSignalMap.end();
           pit != pite; pit++)	{
        os << "\n";
        os << "Port: " << pit->first << " bound to signal " << pit->second;
			}
		}	else	{
      os << "\n No instance name found ////// weird.";
		}
    os << "\n ------------------------------------------------------\n";
	}
}
