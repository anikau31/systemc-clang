#include "Model.h"
//#include "FindSCModules.h"
#include <string>

using namespace scpar;
using namespace std;

Model::Model() {}

Model::~Model() {
  //  llvm::errs() << "\n[[ Destructor Model ]]\n";
  // Delete all ModuleDecl pointers.
  for (Model::moduleMapType::iterator mit = modules_.begin();
       mit != modules_.end(); mit++) {
    //
    // c++17 feature
    // for (auto const & [it, module_decl] : modules_ ) {
    // Second is the ModuleDecl type.
    delete mit->second;
    // delete module_decl;
  }
  modules_.clear();
}

Model::Model(const Model &from) { modules_ = from.modules_; }

void Model::addModuleDecl(ModuleDecl *md) {
  modules_.insert(Model::modulePairType(md->getName(), md));
}

void Model::addModuleDeclInstances(ModuleDecl *md, vector<ModuleDecl *> mdVec) {
  module_instance_map_.insert(moduleInstancePairType(md, mdVec));
}

void Model::addSimulationTime(FindSimTime::simulationTimeMapType simTime) {
  simulation_time_ = simTime;
}

void Model::addEntryFunctionGPUMacroMap(entryFunctionGPUMacroMapType e) {
  // entry_function_gpu_macro_map_.insert(e.begin(), e.end());
  entry_function_gpu_macro_map_ = e;
  llvm::errs() << " \n Size : " << entry_function_gpu_macro_map_.size() << " "
               << e.size();
}

void Model::addGlobalEvents(FindGlobalEvents::globalEventMapType eventMap) {
  for (FindGlobalEvents::globalEventMapType::iterator it = eventMap.begin();
       it != eventMap.end(); it++) {
    string eventName = it->first;
    EventContainer *event = new EventContainer(eventName, it->second);

    event_map_.insert(eventPairType(eventName, event));
  }
}

void Model::addSCMain(FunctionDecl *fnDecl) { scmain_function_decl_ = fnDecl; }

void Model::addNetlist(FindNetlist &n) {
  instance_module_map_ = n.getInstanceModuleMap();
  port_signal_map_ = n.getPortSignalMap();
  port_signal_instance_map_ = n.getInstancePortSignalMap();
  module_instance_list_ = n.getInstanceListModuleMap();

  updateModuleDecl();
}

void Model::updateModuleDecl() {

  for (moduleMapType::iterator it = modules_.begin(), eit = modules_.end();
       it != eit; it++) {
    string moduleName = it->first;
    ModuleDecl *md = it->second;
    vector<string> instanceList;

    llvm::errs() << "Finding instances for " << moduleName << " declaration: ";
    if (module_instance_list_.find(moduleName) != module_instance_list_.end()) {
      FindNetlist::instanceListModuleMapType::iterator
          instanceListModuleMapFind = module_instance_list_.find(moduleName);
      md->addInstances(instanceListModuleMapFind->second);

      // Print the names of all the instances
      for (auto instance : instanceListModuleMapFind->second) {
        llvm::errs() << instance << " ";
      }
      llvm::errs() << "\n";

      for (size_t i = 0; i < instanceListModuleMapFind->second.size(); i++) {
        if (port_signal_instance_map_.find(instanceListModuleMapFind->second.at(
                i)) != port_signal_instance_map_.end()) {
          FindNetlist::instancePortSignalMapType::iterator portSignalMapFound =
              port_signal_instance_map_.find(
                  instanceListModuleMapFind->second.at(i));
          FindNetlist::portSignalMapType portSignalMap =
              portSignalMapFound->second;

          md->addSignalBinding(portSignalMap);
        } else {
          llvm::errs() << "\n Could not find instance and signal";
        }
      }
    } else {
      llvm::errs() << "NONE.";
    }
  }
}

void Model::addSCModules(FindSCModules *m) {
  FindSCModules::moduleMapType mods = m->getSystemCModulesMap();

  for (FindSCModules::moduleMapType::iterator mit = mods.begin(); mit != mods.end();
       mit++) {
    addModuleDecl(new ModuleDecl(mit->first, mit->second));
  }
}

Model::moduleMapType Model::getModuleDecl() const { return modules_; }

Model::entryFunctionGPUMacroMapType Model::getEntryFunctionGPUMacroMap() {
  llvm::errs() << "\n return Size : " << entry_function_gpu_macro_map_.size();
  return entry_function_gpu_macro_map_;
}

Model::moduleInstanceMapType Model::getModuleInstanceMap() {
  return module_instance_map_;
}

Model::eventMapType Model::getEventMapType() { return event_map_; }

unsigned int Model::getNumEvents() { return (event_map_.size() - 3); }

void Model::dump(llvm::raw_ostream &os) {

  os << "\n" << "# Number of modules: " << modules_.size();

  for (Model::moduleMapType::iterator mit = modules_.begin();
       mit != modules_.end(); mit++) {
    // Second is the ModuleDecl type.

    // Get all the instances, and start to go through each of those. 
    vector<ModuleDecl *> instanceVec = module_instance_map_[mit->second];
    os << "\n# Module " << mit->first << ": " << instanceVec.size()
       << " instances.";
    for (size_t i = 0; i < instanceVec.size(); i++) {
      //			os <<", instance: " << i + 1 << " ";
      instanceVec.at(i)->dump(os);
    }
  }
  os << "\n\n";
  os << "# Global events:\n";
  for (Model::eventMapType::iterator it = event_map_.begin(),
                                     ite = event_map_.end();
       it != ite; it++) {
    os << "   Event: " << it->first << "  VarDecl: " << it->second << "\n";
  }

  os << "\n";
  os << "# Simulation time: ";
  for (FindSimTime::simulationTimeMapType::iterator
           it = simulation_time_.begin(),
           eit = simulation_time_.end();
       it != eit; it++) {
    os << it->first << " " << it->second;
  }

  os << "\n\n";
  os << "# Netlist: ";
  for (FindNetlist::instanceModuleMapType::iterator
           it = instance_module_map_.begin(),
           eit = instance_module_map_.end();
       it != eit; it++) {
    os << "\n";
    os << "Instance Name: " << it->first << ", module name : " << it->second;
    string instanceName = it->first;
    if (port_signal_instance_map_.find(instanceName) !=
        port_signal_instance_map_.end()) {
      FindNetlist::instancePortSignalMapType::iterator
          instancePortSignalMapFound =
              port_signal_instance_map_.find(instanceName);
      FindNetlist::portSignalMapType portSignalMap =
          instancePortSignalMapFound->second;
      for (FindNetlist::portSignalMapType::iterator pit = portSignalMap.begin(),
                                                    pite = portSignalMap.end();
           pit != pite; pit++) {
        os << "\n";
        os << "Port: " << pit->first << " bound to signal " << pit->second;
      }
    } else {
      os << "\n No instance name found ////// weird.";
    }
    os << "\n ------------------------------------------------------\n";
  }
}
