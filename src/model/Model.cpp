#include "Model.h"

#include <string>

using namespace systemc_clang;

Model::Model() : root_module_inst_{nullptr} {}

void Model::populateNestedModules() {
  for (ModuleInstance *inst : module_instances_) {
    ModuleInstanceType instance_info{inst->getInstanceInfo()};
    instance_info.dump();

    ModuleInstance *parent{getInstance(instance_info.getParentDecl())};

    if ((parent != nullptr) && (parent != inst)) {
      parent->addNestedModule(inst);
    } else {
      /// This is a ModuleInstance that has no parent. This means that it is a
      /// root note.
      if (root_module_inst_) {
        llvm::errs() << "Multiple root module instances are not allowed\n";
      }
      root_module_inst_ = inst;
    }
  }
}

ModuleInstance *Model::getRootModuleInstance() const {
  return root_module_inst_;
}

Model::~Model() {
  for (auto &inst : module_instances_) {
    // auto incomplete_decl{inst.first};
    // auto instance_list{inst.second};
    LLVM_DEBUG(llvm::dbgs() << "Delete instances for " << inst->getName(););
    // for (ModuleInstance *inst_in_list : instance_list) {
    // This is a ModuleInstance*
    LLVM_DEBUG(llvm::dbgs() << "- delete instance: " << inst->getInstanceName()
                            << ", pointer: " << inst << "\n";);
    //
    // IMPORTANT
    // The current design creates an incomplete ModuleInstance in Matchers. The
    // ports are populated there.  Then, for each instance that is recognized,
    // a new ModuleInstance is created, and information from the incomplete
    // ModuleInstance is copied into the new instance-specific ModuleInstance.
    // When deleting an instance of ModuleInstance, we have to be careful.  This
    // is because we do not want to delete the instance-specific ModuleInstance,
    // which has structures with pointers in it (PortDecl), and then delete
    // the incomplete ModuleInstance because the latter will cause a double free
    // memory error.  This is because the deletion of the instance-specific
    // ModuleInstance will free the objects identified in the incomplete
    // ModuleInstance.
    //
    // The current solution is to clear the information for the
    // instance-specific ModuleInstance before deleting it.  Then, the deletion
    // of the incomplete ModuleInstance will free the other objects such as
    // PortDecl. clearOnlyGlobal does exactly this.
    //
    // TODO: ENHANCEMENT: This is one major refactoring that should be done at
    // some point.
    //
    inst->clearOnlyGlobal();
    delete inst;

    // This is deleted in the vector.
    root_module_inst_ = nullptr;
  }

  LLVM_DEBUG(llvm::dbgs() << "Done with delete\n";);
}
//
// Model::Model(const Model &from) { modules_ = from.modules_; }
//
void Model::addInstance(ModuleInstance *mod) {
  module_instances_.push_back(mod);
}

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

ModuleInstance *Model::getInstance(const std::string &instance_name) {
  auto test_module_it =
      std::find_if(module_instances_.begin(), module_instances_.end(),
                   [instance_name](const auto &instance) {
                     return (instance->getInstanceName() == instance_name);
                   });

  if (test_module_it != module_instances_.end()) {
    return *test_module_it;
  }
  return nullptr;
}

// Must provide the Instance decl.
ModuleInstance *Model::getInstance(Decl *instance_decl) {

  auto test_module_it =
      std::find_if(module_instances_.begin(), module_instances_.end(),
                   [instance_decl](const auto &instance) {
                     return (instance->getInstanceDecl() == instance_decl);
                   });
  if (test_module_it != module_instances_.end()) {
    return *test_module_it;
  }
  return nullptr;
}

Model::entryFunctionGPUMacroMapType Model::getEntryFunctionGPUMacroMap() {
  return entry_function_gpu_macro_map_;
}

std::vector<ModuleInstance *> &Model::getInstances() {
  return module_instances_;
}

Model::eventMapType Model::getEventMapType() { return event_map_; }

unsigned int Model::getNumEvents() { return (event_map_.size() - 3); }

void Model::dump(llvm::raw_ostream &os) {
  os << "sc_module instances [" << module_instances_.size() << "]: ";

  for (const auto &inst : module_instances_) {
    os << inst->getInstanceName() << "   ";
  }
  os << "\n";

  os << "Print out instance information";
  for (const auto &inst : module_instances_) {
    inst->dump(os);
  }
}
