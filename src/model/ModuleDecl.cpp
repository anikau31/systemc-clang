#include <algorithm>
#include <string>
#include <tuple>

#include "ModuleDecl.h"

using namespace systemc_clang;
using namespace std;

ModuleDecl::ModuleDecl()
    : module_name_{"NONE"},
      instance_name_{"NONE"},
      class_decl_{nullptr},
      constructor_stmt_{nullptr},
      constructor_decl_{nullptr},
      instance_decl_{nullptr} {}

ModuleDecl::ModuleDecl(const string &name, CXXRecordDecl *decl)
    : module_name_{name},
      instance_name_{"NONE"},
      class_decl_{decl},
      instance_decl_{nullptr} {}

ModuleDecl::ModuleDecl(
    const std::tuple<const std::string &, CXXRecordDecl *> &element)
    : module_name_{get<0>(element)}, class_decl_{get<1>(element)} {}

ModuleDecl::ModuleDecl(const ModuleDecl &from) {
  module_name_ = from.module_name_;
  instance_name_ = from.instance_name_;
  instance_info_ = from.instance_info_;

  class_decl_ = from.class_decl_;
  constructor_stmt_ = from.constructor_stmt_;
  constructor_decl_ = from.constructor_decl_;
  instance_decl_ = from.instance_decl_;

  process_map_ = from.process_map_;
  in_ports_ = from.in_ports_;
  out_ports_ = from.out_ports_;
  inout_ports_ = from.inout_ports_;
  other_fields_ = from.other_fields_;
  //submodules_ = from.submodules_;

  istreamports_ = from.istreamports_;
  ostreamports_ = from.ostreamports_;
  port_bindings_ = from.port_bindings_;

  iinterfaces_ = from.iinterfaces_;
  ointerfaces_ = from.ointerfaces_;
  iointerfaces_ = from.iointerfaces_;
  signals_ = from.signals_;

  instance_list_ = from.instance_list_;
  port_signal_map_ = from.port_signal_map_;
  vef_ = from.vef_;

  // Class template parameters.
  template_parameters_ = from.template_parameters_;

  // Nested submodules
  nested_modules_ = from.nested_modules_;
}

ModuleDecl &ModuleDecl::operator=(const ModuleDecl &from) {
  module_name_ = from.module_name_;
  instance_name_ = from.instance_name_;
  instance_info_ = from.instance_info_;

  class_decl_ = from.class_decl_;
  constructor_stmt_ = from.constructor_stmt_;
  constructor_decl_ = from.constructor_decl_;
  instance_decl_ = from.instance_decl_;

  process_map_ = from.process_map_;
  in_ports_ = from.in_ports_;
  out_ports_ = from.out_ports_;
  inout_ports_ = from.inout_ports_;
  other_fields_ = from.other_fields_;
  //submodules_ = from.submodules_;

  istreamports_ = from.istreamports_;
  ostreamports_ = from.ostreamports_;

  port_bindings_ = from.port_bindings_;

  iinterfaces_ = from.iinterfaces_;
  ointerfaces_ = from.ointerfaces_;
  iointerfaces_ = from.iointerfaces_;
  signals_ = from.signals_;

  instance_list_ = from.instance_list_;
  port_signal_map_ = from.port_signal_map_;
  vef_ = from.vef_;

  // Class template parameters.
  template_parameters_ = from.template_parameters_;

  // Nested submodules
  nested_modules_ = from.nested_modules_;

  return *this;
}

void ModuleDecl::clearOnlyGlobal() {
  // This only clears the globally (not instance-specific) allocated structures.
  // these are AST nodes
  class_decl_ = nullptr;
  constructor_stmt_ = nullptr;
  constructor_decl_ = nullptr;
  instance_decl_;

  // Ports are globally allocated.
  in_ports_.clear();
  out_ports_.clear();
  inout_ports_.clear();
  other_fields_.clear();
  //submodules_.clear();
  istreamports_.clear();
  ostreamports_.clear();
  signals_.clear();
}

ModuleDecl::~ModuleDecl() {
  DEBUG_WITH_TYPE("DebugDestructors", llvm::dbgs() << "\n~ModuleDecl\n";);
  class_decl_ = nullptr;
  constructor_stmt_ = nullptr;
  instance_decl_ = nullptr;
//
  // DEBUG_WITH_TYPE("DebugDestructors",
                  // llvm::dbgs() << "- name: " << getName()
                               // << ", inst name: " << getInstanceName()
                               // << " pointer: " << this << "\n";);

  // IMPORTANT: Only the instance-specific details should be deleted.
  // DO NOT delete the information collected through incomplete types.
  //

  // llvm::outs() << "- deleting entry function pointers\n";
  for (auto &v : vef_) {
    if (v != nullptr) {
      delete v;
    }
    v = nullptr;
  }
  // llvm::outs() << "Exit deleting ModuleDecl\n";
  // Delete all pointers in ports.
  for (auto &input_port : in_ports_) {
    // It is a tuple
    // 0. string, 1. PortDecl*
    // Only one to delete is (1)
    //
    //  delete input_port.second;
    auto iport{get<1>(input_port)};
    if (iport) {
      delete iport;
    }
  }
  in_ports_.clear();

  for (auto &output_port : out_ports_) {
    delete get<1>(output_port);
  }
  out_ports_.clear();

  for (auto &io_port : inout_ports_) {
    // Second is the PortDecl*.
    delete get<1>(io_port);
  }
  inout_ports_.clear();

  for (auto &other : other_fields_) {
    // Second is the PortDecl*.
    delete get<1>(other);
  }
  other_fields_.clear();
//
  // for (auto &sm : submodules_) {
    // delete get<1>(sm);
  // }
  // submodules_.clear();

  // Delete EntryFunction container
  for (auto &ef : vef_) {
    delete ef;
  }
  vef_.clear();

  for (auto &sig : signals_) {
    delete sig.second;
  }
}

void ModuleDecl::setInstanceInfo(
    const sc_ast_matchers::ModuleInstanceType &info) {
  instance_info_ = info;
}


ModuleInstanceType ModuleDecl::getInstanceInfo() { return instance_info_; }

// void ModuleDecl::setInstanceName(const string &name) { instance_name_ = name;
// }
//
// void ModuleDecl::setInstanceDecl(Decl *decl) { instance_decl_ = decl; }
//
void ModuleDecl::setTemplateParameters(const vector<string> &parm_list) {
  template_parameters_ = parm_list;
}

void ModuleDecl::setTemplateArgs(const vector<string> &parm_list) {
  template_args_ = parm_list;
}

vector<string> ModuleDecl::getTemplateParameters() const {
  return template_parameters_;
}

void ModuleDecl::setModuleName(const string &name) { module_name_ = name; }

void ModuleDecl::addInstances(const vector<string> &instanceList) {
  instance_list_ = instanceList;
}

void ModuleDecl::addPortBinding(const std::string &port_name, PortBinding *pb) {
  port_bindings_.insert(portBindingPairType(port_name, pb));
}

void ModuleDecl::addSignalBinding(
    std::map<std::string, std::string> portSignalMap) {
  port_signal_map_.insert(portSignalMap.begin(), portSignalMap.end());
}

void ModuleDecl::addPorts(const ModuleDecl::PortType &found_ports,
                          const std::string &port_type) {
  if (port_type == "sc_in") {
    std::copy(begin(found_ports), end(found_ports), back_inserter(in_ports_));
  }
  if (port_type == "sc_out") {
    std::copy(begin(found_ports), end(found_ports), back_inserter(out_ports_));
  }
  if (port_type == "sc_inout") {
    std::copy(begin(found_ports), end(found_ports),
              back_inserter(inout_ports_));
  }
  if (port_type == "others") {
    std::copy(begin(found_ports), end(found_ports),
              back_inserter(other_fields_));
  }

  // if (port_type == "submodules") {
    // std::copy(begin(found_ports), end(found_ports), back_inserter(submodules_));
  // }

  if (port_type == "sc_signal") {
    /// SignalDecl derived from PortDecl
    for (auto const &signal_port : found_ports) {
      auto port_decl{get<1>(signal_port)};
      auto name{port_decl->getName()};
      // auto templates{port_decl->getTemplateType()};
      // auto field_decl{port_decl->getFieldDecl()};

      // SignalContainer
      // auto signal_container{new SignalContainer{name, templates,
      // field_decl}};
      SignalDecl *signal_entry{new SignalDecl{
          *port_decl}};  // new SignalDecl{name, field_decl, templates}};
      //*signal_entry = *port_decl;
      signals_.insert(ModuleDecl::signalPairType(name, signal_entry));
    }
    // std::copy(begin(found_ports), end(found_ports), back_inserter(signals_));
  }

  if (port_type == "sc_stream_in") {
    std::copy(begin(found_ports), end(found_ports),
              back_inserter(istreamports_));
  }
  if (port_type == "sc_stream_out") {
    std::copy(begin(found_ports), end(found_ports),
              back_inserter(ostreamports_));
  }
}

void ModuleDecl::addInputInterfaces(FindTLMInterfaces::interfaceType p) {
  for (auto mit : p) {
    string n = mit.first;
    FindTemplateTypes *tt = new FindTemplateTypes(mit.second);
    InterfaceDecl *pd = new InterfaceDecl(n, tt);

    iinterfaces_.insert(interfacePairType(mit.first, pd));
  }
}

void ModuleDecl::addOutputInterfaces(FindTLMInterfaces::interfaceType p) {
  for (auto mit : p) {
    string name = mit.first;
    FindTemplateTypes *tt = new FindTemplateTypes(*mit.second);
    InterfaceDecl *pd = new InterfaceDecl(name, tt);

    ointerfaces_.insert(interfacePairType(name, pd));
  }
}

void ModuleDecl::addInputOutputInterfaces(FindTLMInterfaces::interfaceType p) {
  //  for (FindTLMInterfaces::interfaceType::iterator mit = p.begin(), mite =
  //  p.end();    mit != mite; mit++) {
  for (auto mit : p) {
    iointerfaces_.insert(
        interfacePairType(mit.first, new InterfaceDecl(mit.first, mit.second)));
  }
}

void ModuleDecl::addConstructor(FindConstructor *ctor) {
  constructor_stmt_ = ctor->getConstructorStmt();
  constructor_decl_ = ctor->getConstructorDecl();
}

void ModuleDecl::addConstructor(Stmt *constructor) {
  constructor_stmt_ = constructor;
}

Stmt *ModuleDecl::getConstructorStmt() const { return constructor_stmt_; }

CXXConstructorDecl *ModuleDecl::getConstructorDecl() const {
  return constructor_decl_;
}

void ModuleDecl::addProcess(FindEntryFunctions::entryFunctionVectorType *efv) {
  vef_ = *efv;
  for (unsigned int i = 0; i < efv->size(); ++i) {
    EntryFunctionContainer *ef = (*efv)[i];

    // Set the entry name.
    string entryName = ef->_entryName;
    string entryType = "";

    // Set the process type
    switch (ef->_procType) {
      case PROCESS_TYPE::THREAD: {
        entryType = "SC_THREAD";
        break;
      }
      case PROCESS_TYPE::METHOD: {
        entryType = "SC_METHOD";
        break;
      }
      case PROCESS_TYPE::CTHREAD: {
        entryType = "SC_CTHREAD";
        break;
      }
      default: {
        entryType = "ERROR";
        break;
      }
    }
    process_map_.insert(processPairType(
        entryName,
        new ProcessDecl(entryType, entryName, ef->_entryMethodDecl, ef)));
  }
}

void ModuleDecl::addNestedModule(ModuleDecl *nested_module) {
  nested_modules_.push_back(nested_module);
}

vector<string> ModuleDecl::getInstanceList() { return instance_list_; }

std::vector<EntryFunctionContainer *> ModuleDecl::getEntryFunctionContainer() {
  return vef_;
}

int ModuleDecl::getNumInstances() { return instance_list_.size(); }

const ModuleDecl::signalMapType &ModuleDecl::getSignals() const {
  return signals_;
}

const std::vector<ModuleDecl *> &ModuleDecl::getNestedModules() const {
  return nested_modules_;
}

ModuleDecl::processMapType ModuleDecl::getProcessMap() { return process_map_; }

ModuleDecl::portMapType ModuleDecl::getOPorts() { return out_ports_; }

ModuleDecl::portMapType ModuleDecl::getIPorts() { return in_ports_; }

ModuleDecl::portMapType ModuleDecl::getIOPorts() { return inout_ports_; }

ModuleDecl::portMapType ModuleDecl::getOtherVars() { return other_fields_; }

//ModuleDecl::portMapType ModuleDecl::getSubmodules() { return submodules_; }

ModuleDecl::portMapType ModuleDecl::getInputStreamPorts() {
  return istreamports_;
}

ModuleDecl::portMapType ModuleDecl::getOutputStreamPorts() {
  return ostreamports_;
}

ModuleDecl::interfaceMapType ModuleDecl::getOInterfaces() {
  return ointerfaces_;
}

ModuleDecl::interfaceMapType ModuleDecl::getIInterfaces() {
  return iinterfaces_;
}

ModuleDecl::interfaceMapType ModuleDecl::getIOInterfaces() {
  return iointerfaces_;
}

ModuleDecl::portBindingMapType ModuleDecl::getPortBindings() {
  return port_bindings_;
}

string ModuleDecl::getName() const { return module_name_; }

std::string ModuleDecl::getInstanceName() const {
  return instance_info_.instance_name;
}

bool ModuleDecl::isModuleClassDeclNull() { return (class_decl_ == nullptr); }

CXXRecordDecl *ModuleDecl::getModuleClassDecl() {
  assert(!(class_decl_ == nullptr));
  return class_decl_;
}

// FieldDecl *ModuleDecl::getInstanceFieldDecl() { return instance_field_decl_;
// } VarDecl *ModuleDecl::getInstanceVarDecl() { return instance_var_decl_; }

Decl *ModuleDecl::getInstanceDecl() { return instance_info_.getInstanceDecl(); }

// bool ModuleDecl::isInstanceFieldDecl() const {
// if ((instance_field_decl_ != nullptr) && (instance_var_decl_ == nullptr)) {
// return true;
// }
// if ((instance_field_decl_ == nullptr) && (instance_var_decl_ != nullptr)) {
// return false;
// }
//
// return false;
// }
//
void ModuleDecl::dumpInstances(raw_ostream &os, int tabn) {
  if (instance_list_.empty()) {
    os << " none \n";
  }

  for (size_t i = 0; i < instance_list_.size(); i++) {
    os << instance_list_.at(i) << " ";
  }

  // Dump the submodules
  //
  for (auto const &submod : nested_modules_) {
    os << "nested module  " << submod << " module type name "
       << submod->getName();
    
    if ( submod->getInstanceInfo().getInstanceNames().size() > 0 ) {
      os << "  instance name: ";
    }
    for (auto const &name: submod->getInstanceInfo().getInstanceNames() ) {
      os << name << "  ";
    }
   os << "\n";
  }
}

void ModuleDecl::dumpPortBinding() {
  json binding_j;

  binding_j["number_of_ports_bound"] = port_bindings_.size();
  for (auto const &pb : port_bindings_) {
    auto port_name{get<0>(pb)};
    auto binding{get<1>(pb)};

    json port_j;
    port_j["port_member_name"] = port_name;
    port_j["port_member_is_array"] = (binding->hasPortArrayParameter() ? "yes" : "no");
    if (binding->hasPortArrayParameter()) {
      port_j["port_member_array_index"] = binding->getPortArrayIndex()
                                     ->getNameInfo()
                                     .getName()
                                     .getAsString();
    }

    port_j["bound_to_name"] =  binding->getBoundToName();
    /// If it is an array.
    port_j["bound_to_is_array"] = (binding->hasBoundToArrayParameter() ? "yes" : "no");
    if (binding->hasBoundToArrayParameter()) {
      port_j["bound_to_array_index"] = binding->getBoundToArrayIndex()
                                     ->getNameInfo()
                                     .getName()
                                     .getAsString();
    }

    binding->dump();
  binding_j[port_name] = port_j;
  }
  llvm::outs() << binding_j.dump(4) << "\n";
}

void ModuleDecl::dumpSignalBinding(raw_ostream &os, int tabn) {
  if (port_signal_map_.empty()) {
    os << " none\n";
    return;
  }

  for (auto it : port_signal_map_) {
    os << "\nPort : " << it.first << " bound to signal : " << it.second;
  }
}

void ModuleDecl::dumpProcesses(raw_ostream &os, int tabn) {
  json process_j;
  process_j["number_of_processes"] = process_map_.size();
  for (auto pit : process_map_) {
    ProcessDecl *pd{pit.second};
    process_j[pit.first] = pd->dump_json();
  }

  os << "Processes\n";
  os << process_j.dump(4) << "\n";
}

void ModuleDecl::dumpInterfaces(raw_ostream &os, int tabn) {
  os << "Input interfaces: " << iinterfaces_.size() << "\n";

  if (iinterfaces_.size() == 0) {
    os << " none\n";
  } else {
    for (auto mit : iinterfaces_) {
      mit.second->dump(os, tabn);
      os << "\n ";
    }
    os << "\n";
  }

  os << "Output interfaces: " << ointerfaces_.size() << "\n";
  if (ointerfaces_.size() == 0) {
    os << "none \n";
  } else {
    for (auto mit : ointerfaces_) {
      mit.second->dump(os, tabn);
      os << "\n ";
    }
    os << "\n";
  }

  os << "Inout interfaces: " << iointerfaces_.size() << "\n";
  if (iointerfaces_.size() == 0) {
    os << "none \n";
  } else {
    for (auto mit : iointerfaces_) {
      mit.second->dump(os, tabn);
      os << "\n ";
    }
    os << "\n";
  }
}

void ModuleDecl::dumpPorts(raw_ostream &os, int tabn) {
  json iport_j, oport_j, ioport_j, othervars_j, istreamport_j, ostreamport_j,
      submodules_j;

  iport_j["number_of_input_ports"] = in_ports_.size();
  for (auto mit : in_ports_) {
    auto name = get<0>(mit);
    auto pd = get<1>(mit);
    iport_j[name] = pd->dump_json();
  }

  oport_j["number_of_output_ports"] = out_ports_.size();
  for (auto mit : out_ports_) {
    auto name = get<0>(mit);
    auto pd = get<1>(mit);
    oport_j[name] = pd->dump_json();
  }

  ioport_j["number_of_inout_ports"] = inout_ports_.size();
  for (auto mit : inout_ports_) {
    auto name = get<0>(mit);
    auto pd = get<1>(mit);
    ioport_j[name] = pd->dump_json();
  }

  istreamport_j["number_of_instream_ports"] = istreamports_.size();
  for (auto mit : istreamports_) {
    auto name = get<0>(mit);
    auto pd = get<1>(mit);
    istreamport_j[name] = pd->dump_json();
  }

  ostreamport_j["number_of_outstream_ports"] = ostreamports_.size();
  for (auto mit : ostreamports_) {
    auto name = get<0>(mit);
    auto pd = get<1>(mit);
    ostreamport_j[name] = pd->dump_json();
  }

  othervars_j["number_of_other_vars"] = other_fields_.size();
  for (auto mit : other_fields_) {
    auto name = get<0>(mit);
    auto pd = get<1>(mit);
    othervars_j[name] = pd->dump_json();
  }

  submodules_j["number_of_submodule_fields"] = nested_modules_.size();
  for (auto mit : nested_modules_) {
    auto module_name {mit->getName()};
    // submodules_j[name].push_back(mit->getInstanceName());

    auto instance_info{ mit->getInstanceInfo() };
    for (auto const &inst_name: instance_info.getInstanceNames()) {
      submodules_j[module_name].push_back(inst_name);
    }
  }

  os << "Start printing ports\n";
  os << "\nInput ports: " << in_ports_.size() << "\n";
  os << "\nOutput ports: " << out_ports_.size() << "\n";
  os << "\nInout ports: " << inout_ports_.size() << "\n";
  os << "\nIstream ports: " << istreamports_.size() << "\n";
  os << "\nOstream ports: " << ostreamports_.size() << "\n";
  os << "\nOther fields: " << other_fields_.size() << "\n";
  os << "\nSubmodules: " << nested_modules_.size() << "\n";

  os << "Ports\n";
  os << iport_j.dump(4) << "\n"
     << oport_j.dump(4) << "\n"
     << ioport_j.dump(4) << "\n"
     << istreamport_j.dump(4) << "\n"
     << ostreamport_j.dump(4) << othervars_j.dump(4) << submodules_j.dump(4)
     << "\n";
}

void ModuleDecl::dumpSignals(raw_ostream &os, int tabn) {
  json signal_j;
  signal_j["number_of_signals"] = signals_.size();
  for (auto sit : signals_) {
    SignalDecl *s{sit.second};
    signal_j[sit.first] = s->dump_json();
  }

  os << "Signals\n";
  os << signal_j.dump(4) << "\n";
}

void ModuleDecl::dump(llvm::raw_ostream &os) {
  os << "Module declaration name: " << module_name_;
  os << "\n# Instances:\n";
  dumpInstances(os, 4);
  os << "# Port Declaration:\n";
  dumpPorts(os, 4);
  os << "\n# Signal Declaration:\n";
  dumpSignals(os, 4);
  os << "\n# Processes:\n";
  dumpProcesses(os, 4);
  os << "# Port binding:" << port_bindings_.size() << "\n";
  dumpPortBinding();  //(os, 4);
  os << "# Signal binding:\n";
  dumpSignalBinding(os, 4);

  dump_json();
  os << "\n=======================================================\n";
}

json ModuleDecl::dump_json() {
  json module_j;

  module_j["module_name"] = module_name_;
  module_j["instance_name"] = instance_name_;
  module_j["is_array"] = "false";
  if (instance_info_.isArrayType()) {
    module_j["is_array"] = "true";
    // Write out all the sizes.
    for (auto const &size: instance_info_.getArraySizes() ) {
      module_j["array_sizes"] += size.getLimitedValue();
    }
  }

  // Template parameters.
  for (auto const &parm : template_parameters_) {
    module_j["template_parameters"].push_back(parm);
  }

  for (auto const &parm : template_args_) {
    module_j["template_args"].push_back(parm);
  }

  for (auto const &submod : nested_modules_) {
    module_j["nested_modules"].push_back(submod->getInstanceInfo().getInstanceNames());
  }

  llvm::outs() << module_j.dump(4);
  return module_j;
}
