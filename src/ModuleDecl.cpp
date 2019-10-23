#include <string>
#include <algorithm>
#include <tuple>

#include "ModuleDecl.h"

using namespace scpar;
using namespace std;

ModuleDecl::ModuleDecl() : module_name_{"NONE"},
  class_decl_{nullptr},
  constructor_stmt_{nullptr} {}

ModuleDecl::ModuleDecl(const string &name, CXXRecordDecl *decl)
  : module_name_{name}, class_decl_{decl} {}

ModuleDecl::ModuleDecl(const std::tuple< const std::string& , CXXRecordDecl* >& element )
  : module_name_{get<0>(element)}, class_decl_{get<1>(element)} {}


  ModuleDecl::~ModuleDecl() {

    class_decl_ = nullptr;
    constructor_stmt_ = nullptr;
    // Delete all pointers in ports.
    for (auto input_port : in_ports_) {
      // It is a tuple 
      // 0. string, 1. PortDecl*
      // Only one to delete is (3)
      //
    //  delete input_port.second;
      delete get<1>( input_port );
    }
    in_ports_.clear();

    for (auto output_port : out_ports_) {
      delete get<1>( output_port );
    }
    out_ports_.clear();

    for (auto io_port : inout_ports_) {
      // Second is the PortDecl*.
      delete get<1>( io_port );
    }
    inout_ports_.clear();

    for (auto other : other_fields_ ) {
      // Second is the PortDecl*.
      delete get<1>( other );
    }
    other_fields_.clear();


  }

void ModuleDecl::setTemplateParameters(const vector<string> & parm_list) {
  template_parameters_ = parm_list;
}

vector<string> ModuleDecl::getTemplateParameters() const {
  return template_parameters_; }

  void ModuleDecl::setModuleName(const string &name) { module_name_ = name; }

  void ModuleDecl::addInstances(const vector<string> & instanceList) {
    _instanceList = instanceList;
  }

void ModuleDecl::addSignalBinding(map<string, string> portSignalMap) {
  _portSignalMap.insert(portSignalMap.begin(), portSignalMap.end());
}

void ModuleDecl::addSignals(const FindSignals::signalMapType & signal_map) {
  for (auto sit : signal_map) {
    string name = sit.first;

    // It is important to create new objects.
    // This is because the objects created during Find*
    // may go outside scope, and free up allocated memory.
    SignalContainer *sc{ new SignalContainer(*sit.second) };
    Signal *sig { new Signal(name, sc) };

    _signals.insert( ModuleDecl::signalPairType(name, sig) );
  }
}

void ModuleDecl::addOtherVars(const FindPorts::PortType &p) {
  for (auto mit : p) {
    string n = mit.first;
    FindTemplateTypes *tt = new FindTemplateTypes(mit.second);
    PortDecl *pd = new PortDecl(n, tt);

    other_fields_.push_back(portPairType(n, pd));
  }
}

void ModuleDecl::addPorts( const ModuleDecl::PortType &found_ports, const std::string &port_type ) {
  /*
  if ( port_type == "sc_in" ) { std::copy( begin(found_ports), end(found_ports), back_inserter(in_ports_ )); }
  if ( port_type == "sc_out" ) { std::copy( begin(found_ports), end(found_ports), back_inserter(out_ports_ )); }
  if ( port_type == "sc_inout" ) {  std::copy( begin(found_ports), end(found_ports), back_inserter(inout_ports_ )); }
  if ( port_type == "others" ) {  std::copy( begin(found_ports), end(found_ports), back_inserter(other_fields_ )); }
  */

}

void ModuleDecl::addInputPorts(const FindPorts::PortType &foundPorts) {
  for ( auto mit : foundPorts ) {
    auto name { mit.first };
    FindTemplateTypes *template_type { new FindTemplateTypes(mit.second) };
    PortDecl *portDecl { new PortDecl(name, template_type) };
    in_ports_.push_back( portPairType( mit.first, portDecl ) );
  }

 }

void ModuleDecl::addOutputPorts(const FindPorts::PortType &p) {
  for (auto mit : p) {
    string n = mit.first;
    FindTemplateTypes *tt = new FindTemplateTypes(mit.second);
    PortDecl *pd = new PortDecl(n, tt);

    out_ports_.push_back(portPairType(n, pd));
  }
}

void ModuleDecl::addInputOutputPorts(const FindPorts::PortType &p) {
  for (auto mit : p) {
    string n = mit.first;
    FindTemplateTypes *tt = new FindTemplateTypes(mit.second);
    PortDecl *pd = new PortDecl(n, tt);
    inout_ports_.push_back( portPairType(n , pd));
  }
}

void ModuleDecl::addInputInterfaces(FindTLMInterfaces::interfaceType p) {
  for (auto mit : p) {
    string n = mit.first;
    FindTemplateTypes *tt = new FindTemplateTypes(mit.second);
    InterfaceDecl *pd = new InterfaceDecl(n, tt);

    _iinterfaces.insert(interfacePairType(mit.first, pd));
  }
}

void ModuleDecl::addOutputInterfaces(FindTLMInterfaces::interfaceType p) {
  for (auto mit : p) {
    string name = mit.first;
    FindTemplateTypes *tt = new FindTemplateTypes(*mit.second);
    InterfaceDecl *pd = new InterfaceDecl(name, tt);

    _ointerfaces.insert(interfacePairType(name, pd));
  }
}

void ModuleDecl::addInputOutputInterfaces(FindTLMInterfaces::interfaceType p) {
  //  for (FindTLMInterfaces::interfaceType::iterator mit = p.begin(), mite =
  //  p.end();    mit != mite; mit++) {
  for (auto mit : p) {
    _iointerfaces.insert(
        interfacePairType(mit.first, new InterfaceDecl(mit.first, mit.second)));
  }
}

void ModuleDecl::addConstructor(Stmt *constructor) {
  constructor_stmt_ = constructor;
}

void ModuleDecl::addProcess(FindEntryFunctions::entryFunctionVectorType *efv) {
  _vef = *efv;
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
    process_map_.insert(
        processPairType(entryName, new ProcessDecl(entryType, entryName,
            ef->_entryMethodDecl, ef)));
  }
}

vector<string> ModuleDecl::getInstanceList() { return _instanceList; }

vector<EntryFunctionContainer *> ModuleDecl::getEntryFunctionContainer() {
  return _vef;
}

int ModuleDecl::getNumInstances() { return _instanceList.size(); }

ModuleDecl::signalMapType ModuleDecl::getSignals() { return _signals; }

ModuleDecl::processMapType ModuleDecl::getProcessMap() { return process_map_; }

ModuleDecl::portMapType ModuleDecl::getOPorts() { return out_ports_; }

ModuleDecl::portMapType ModuleDecl::getIPorts() { return in_ports_; }

ModuleDecl::portMapType ModuleDecl::getIOPorts() { return inout_ports_; }

ModuleDecl::interfaceMapType ModuleDecl::getOInterfaces() {
  return _ointerfaces;
}

ModuleDecl::interfaceMapType ModuleDecl::getIInterfaces() {
  return _iinterfaces;
}

ModuleDecl::interfaceMapType ModuleDecl::getIOInterfaces() {
  return _iointerfaces;
}

string ModuleDecl::getName() { return module_name_; }

bool ModuleDecl::isModuleClassDeclNull() { return (class_decl_ == nullptr); }

CXXRecordDecl *ModuleDecl::getModuleClassDecl() {
  assert(!(class_decl_ == nullptr));
  return class_decl_;
}

void ModuleDecl::dumpInstances(raw_ostream &os, int tabn) {

  if (_instanceList.empty()) {
    os << " none \n";
  }

  for (size_t i = 0; i < _instanceList.size(); i++) {
    os << _instanceList.at(i) << " ";
  }
}

void ModuleDecl::dumpSignalBinding(raw_ostream &os, int tabn) {
  if (_portSignalMap.empty()) {
    os << " none\n";
    return;
  }

  for (auto it : _portSignalMap) {
    os << "\nPort : " << it.first << " bound to signal : " << it.second;
  }
}

void ModuleDecl::dumpProcesses(raw_ostream &os, int tabn) {

  json process_j;
  process_j["number_of_processes"] = process_map_.size();
  for (auto pit : process_map_) {
    ProcessDecl *pd { pit.second };
    process_j[pit.first] = pd->dump_json( os );
  }

  os << "Processes\n";
  os << process_j.dump(4) << "\n";
}

void ModuleDecl::dumpInterfaces(raw_ostream &os, int tabn) {

  os << "Input interfaces: " << _iinterfaces.size() << "\n";

  if (_iinterfaces.size() == 0) {
    os << " none\n";
  } else {
    for (auto mit : _iinterfaces) {
      mit.second->dump(os, tabn);
      os << "\n ";
    }
    os << "\n";
  }

  os << "Output interfaces: " << _ointerfaces.size() << "\n";
  if (_ointerfaces.size() == 0) {
    os << "none \n";
  } else {
    for (auto mit : _ointerfaces) {
      mit.second->dump(os, tabn);
      os << "\n ";
    }
    os << "\n";
  }

  os << "Inout interfaces: " << _iointerfaces.size() << "\n";
  if (_iointerfaces.size() == 0) {
    os << "none \n";
  } else {
    for (auto mit : _iointerfaces) {
      mit.second->dump(os, tabn);
      os << "\n ";
    }
    os << "\n";
  }
}

void ModuleDecl::dumpPorts(raw_ostream &os, int tabn) {
    os << "\nInput ports: " << in_ports_.size() << "\n";

  json iport_j, oport_j, ioport_j, othervars_j;
  iport_j["number_of_in_ports"] = in_ports_.size();

  os <<"Start printing ports\n" ;
  for (auto mit : in_ports_) {
    auto name = get<0>(mit);
    auto pd = get<1>(mit);
    auto template_type = pd->getTemplateType();
    auto template_args{ template_type->getTemplateArgumentsType() };

    iport_j[name] = pd->dump_json( os );
  }


  os << "\nOutput ports: " << out_ports_.size() << "\n";
  oport_j["number_of_output_ports"] = out_ports_.size();
  for (auto mit : out_ports_) {
    auto name = get<0>(mit);
    auto pd = get<1>(mit);
    auto template_type = pd->getTemplateType();
    auto template_args{ template_type->getTemplateArgumentsType() };

    oport_j[name] = pd->dump_json(os);
  }


   os << "\nInout ports: " << inout_ports_.size() << "\n";
  ioport_j["number_of_inout_ports"] = inout_ports_.size();
  for (auto mit : inout_ports_) {
    auto name = get<0>(mit);
    auto pd = get<1>(mit);
    auto template_type = pd->getTemplateType();
    auto template_args{ template_type->getTemplateArgumentsType() };

    ioport_j[name] = pd->dump_json( os );
  }


   os << "\nother vars : " << other_fields_.size() << "\n";
   othervars_j["number_of_other_vars"] = other_fields_.size();
  for (auto mit : other_fields_ ) {
    auto name = get<0>(mit);
    auto pd = get<1>(mit);
    auto template_type = pd->getTemplateType();
    auto template_args{ template_type->getTemplateArgumentsType() };

    othervars_j[name] = pd->dump_json( os );
  }

   os << "Ports\n";
   os << iport_j.dump(4) << "\n" << oport_j.dump(4) << "\n" << ioport_j.dump(4) << "\n" << othervars_j.dump(4) << "\n";
}

void ModuleDecl::dumpSignals(raw_ostream &os, int tabn) {
  json signal_j;
  signal_j["number_of_signals"] = _signals.size();
  for (auto sit : _signals) {
    Signal *s = sit.second;
    signal_j[sit.first] = s->dump_json(os);
  }

  os << "Signals\n";
  os << signal_j.dump(4) << "\n";
}

void ModuleDecl::dump(raw_ostream &os) {
  os << "\n";
  os << "\n# Instances:\n";
  dumpInstances(os, 4);
  os << "# Port Declaration:\n";
  dumpPorts(os, 4);
  os << "\n# Signal Declaration:\n";
  dumpSignals(os, 4);
  os << "\n# Processes:\n";
  dumpProcesses(os, 4);
  os << "# Signal binding:\n";
  dumpSignalBinding(os, 4);

  dump_json();
  os << "\n=======================================================\n";

}

json ModuleDecl::dump_json() {

  json module_j;

  module_j["module_name"] = module_name_;
  // Template parameters.
  for (const auto & parm: template_parameters_) {
    module_j["template_parameters"].push_back( parm );
  }


  std::cout << module_j.dump(4);
  return module_j;
}
