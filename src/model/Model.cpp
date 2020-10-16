#include "Model.h"

#include <string>

using namespace systemc_clang;

Model::Model() {}

Model::~Model() {
  LLVM_DEBUG(llvm::dbgs() << "\n"
                          << "~Model\n";);
  for (auto &inst : module_instances_) {
    // auto incomplete_decl{inst.first};
    // auto instance_list{inst.second};
    LLVM_DEBUG(llvm::dbgs() << "Delete instances for " << inst->getName(););
    // for (ModuleDecl *inst_in_list : instance_list) {
    // This is a ModuleDecl*
    LLVM_DEBUG(llvm::dbgs() << "- delete instance: " << inst->getInstanceName()
                            << ", pointer: " << inst << "\n";);
    //
    // IMPORTANT
    // The current design creates an incomplete ModuleDecl in Matchers. The
    // ports are populated there.  Then, for each instance that is recognized,
    // a new ModuleDecl is created, and information from the incomplete
    // ModuleDecl is copied into the new instance-specific ModuleDecl. When
    // deleting an instance of ModuleDecl, we have to be careful.  This is
    // because we do not want to delete the instance-specific ModuleDecl,
    // which has structures with pointers in it (PortDecl), and then delete
    // the incomplete ModuleDecl because the latter will cause a double free
    // memory error.  This is because the deletion of the instance-specific
    // ModuleDecl will free the objects identified in the incomplete
    // ModuleDecl.
    //
    // The current solution is to clear the information for the
    // instance-specific ModuleDecl before deleting it.  Then, the deletion of
    // the incomplete ModuleDecl will free the other objects such as PortDecl.
    // clearOnlyGlobal does exactly this.
    //
    // TODO: ENHANCEMENT: This is one major refactoring that should be done at
    // some point.
    //
    inst->clearOnlyGlobal();
    delete inst;
  }

  LLVM_DEBUG(llvm::dbgs() << "Done with delete\n";);
}

Model::Model(const Model &from) { modules_ = from.modules_; }

void Model::addInstance(ModuleDecl *mod) { module_instances_.push_back(mod); }

void Model::addModuleDecl(ModuleDecl *md) {
  // class name, instance name.
  modules_.push_back(Model::modulePairType(md->getName(), md));
}

void Model::addModuleDeclInstances(ModuleDecl *md, vector<ModuleDecl *> mdVec) {
  modules_.push_back(Model::modulePairType(md->getName(), md));

  llvm::outs() << "[HDP] To add instances: " << md << "=" << mdVec.size()
               << "\n";
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
    std::string eventName = it->first;
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
    std::string moduleName{it->first};
    ModuleDecl *md = it->second;
    std::vector<std::string> instanceList;

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

// void Model::addSCModules(FindSCModules *m) {
// FindSCModules::moduleMapType mods = m->getSystemCModulesMap();
//
// for (FindSCModules::moduleMapType::iterator mit = mods.begin();
// mit != mods.end(); mit++) {
// addModuleDecl(new ModuleDecl(mit->first, mit->second));
// }
// }
//
const Model::moduleMapType &Model::getModuleDecl() { return modules_; }
//
// ModuleDecl *Model::getModuleDeclByInstance(const std::string &inst_name) {
// for (auto mod : modules_) {
// ModuleDecl *decl{mod.second};
// if (decl->getInstanceName() == inst_name) {
// return decl;
// }
// }
// return nullptr;
// }
//

// ModuleDecl *Model::getModuleDecl(const std::string &decl_name) {
// for (auto mod : modules_) {
// ModuleDecl *decl{mod.second};
// if (decl->getName() == decl_name) {
// return decl;
// }
// }
// return nullptr;
// }
//
// Must specify the instance name.
ModuleDecl *Model::getInstance(const std::string &instance_name) {
  llvm::outs() << "getInstance\n";
  llvm::outs() << "- Looking for " << instance_name << "\n";
  auto test_module_it = std::find_if(
      module_instances_.begin(), module_instances_.end(),
      [instance_name](const auto &instance) {
        llvm::outs() << "- instance name: " << instance->getInstanceName()
                     << "\n";
        return (instance->getInstanceName() == instance_name);
      });

  if (test_module_it != module_instances_.end()) {
    return *test_module_it;
  }
  return nullptr;
}

// Must provide the Instance decl.
ModuleDecl *Model::getInstance(Decl *instance_decl) {
  llvm::outs() << "getInstance Decl to find : " << instance_decl << "\n";
  // std::map<ModuleDecl *, std::vector<ModuleDecl *>> moduleInstanceMapType;
  // for (auto const &element : module_instance_map_) {
  // auto instance_list{element.second};
  //
  for (auto const &inst : module_instances_) {
    llvm::outs() << inst->getInstanceDecl() << ": " << inst->getInstanceName()
                 << "\n";
  }

  auto test_module_it =
      std::find_if(module_instances_.begin(), module_instances_.end(),
                   [instance_decl](const auto &instance) {
                     return (instance->getInstanceDecl() == instance_decl);
                   });
  if (test_module_it != module_instances_.end()) {
    llvm::outs() << "Found\n";
    return *test_module_it;
  }
  return nullptr;
}

Model::entryFunctionGPUMacroMapType Model::getEntryFunctionGPUMacroMap() {
  llvm::errs() << "\n return Size : " << entry_function_gpu_macro_map_.size();
  return entry_function_gpu_macro_map_;
}

std::vector<ModuleDecl *> &Model::getInstances() { return module_instances_; }

Model::eventMapType Model::getEventMapType() { return event_map_; }

unsigned int Model::getNumEvents() { return (event_map_.size() - 3); }

void Model::dump(llvm::raw_ostream &os) {
  os << "-- Number of sc_module instances: " << module_instances_.size()
     << "\n";

  for (const auto &inst : module_instances_) {
    os << "-- Instance name: " << inst->getInstanceName() << "\n";
    inst->dump(os);
  }
  os << "\n";
}
