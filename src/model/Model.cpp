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
//
// Model::Model(const Model &from) { modules_ = from.modules_; }
//
void Model::addInstance(ModuleDecl *mod) { module_instances_.push_back(mod); }

void Model::addSimulationTime(FindSimTime::simulationTimeMapType simTime) {
  simulation_time_ = simTime;
}

void Model::addEntryFunctionGPUMacroMap(entryFunctionGPUMacroMapType e) {
  entry_function_gpu_macro_map_ = e;
  llvm::errs() << " \n Size : " << entry_function_gpu_macro_map_.size() << " "
               << e.size();
}

void Model::addGlobalEvents(FindGlobalEvents::globalEventMapType eventMap) {
  for (FindGlobalEvents::globalEventMapType::iterator it = eventMap.begin();
       it != eventMap.end(); it++) {
    std::string eventName = it->first;
    EventContainer *event{new EventContainer(eventName, it->second)};

    event_map_.insert(eventPairType(eventName, event));
  }
}

void Model::addSCMain(FunctionDecl *fnDecl) { scmain_function_decl_ = fnDecl; }

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
