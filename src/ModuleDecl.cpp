#include <string>
#include "ModuleDecl.h"

using namespace scpar;

using std::string;

ModuleDecl::ModuleDecl():
  _moduleName{"NONE"},
  _classdecl{nullptr} {
  }


ModuleDecl::ModuleDecl( const string &name, CXXRecordDecl *decl ):
  _moduleName{name},
  _classdecl{decl} {
  }

ModuleDecl::~ModuleDecl() {

  // Delete all pointers in ports.
  for ( auto input_port: _iports ) {
    // Second is the PortDecl*.
    delete input_port.second;
  }
  _iports.clear();

  for ( auto output_port: _oports ) {
    // Second is the PortDecl*.
    delete output_port.second;
  }
  _oports.clear();

  for ( auto io_port: _ioports ) {
    // Second is the PortDecl*.
    delete io_port.second;
  }
  _ioports.clear();

}

void ModuleDecl::setModuleName( const string &name ) {
  _moduleName = name;
}

void ModuleDecl::addInstances( vector<string> instanceList ) {
  _instanceList = instanceList;
}

void ModuleDecl::addSignalBinding( map<string, string> portSignalMap ) {
  _portSignalMap.insert(portSignalMap.begin(), portSignalMap.end());
}

void ModuleDecl::addSignals( FindSignals::signalMapType *signal_map ) {
  for ( auto sit: *signal_map ) {
    string name = sit.first;

    // It is important to create new objects. 
    // This is because the objects created during Find*
    // may go outside scope, and free up allocated memory.
    SignalContainer *sc = new SignalContainer(*sit.second);
    Signal *sig = new Signal(name, sc);

    _signals.insert( ModuleDecl::signalPairType( name, sig ) );
  }
}

void ModuleDecl::addInputPorts( FindPorts::PortType p ) {
  for ( auto mit: p ) {
    string name = mit.first;
    FindTemplateTypes *template_type = new FindTemplateTypes(mit.second);
    PortDecl *pd = new PortDecl( name, template_type );

    _iports.insert( portPairType( mit.first, pd ) );
  }
}

void ModuleDecl::addOutputPorts(FindPorts::PortType p) {
  for ( auto mit: p ) {
    string n = mit.first;
    FindTemplateTypes *tt = new FindTemplateTypes( mit.second );
    PortDecl *pd = new PortDecl( n, tt );

    _oports.insert(portPairType(n, pd));
  }
}

void ModuleDecl::addInputOutputPorts( FindPorts::PortType p ) {
  for ( auto mit: p ) {
    _ioports.insert(portPairType( mit.first, new PortDecl(mit.first, mit.second ) ) );
  }
}

void ModuleDecl::addInputInterfaces( FindTLMInterfaces::interfaceType p ) {
  for ( auto mit: p ) {
    string n = mit.first;
    FindTemplateTypes *tt = new FindTemplateTypes( mit.second );
    InterfaceDecl *pd = new InterfaceDecl( n, tt );

    _iinterfaces.insert( interfacePairType( mit.first, pd ) );
  }
}

void ModuleDecl::addOutputInterfaces( FindTLMInterfaces::interfaceType p ) {
  for ( auto mit: p ) {
    string name = mit.first;
    FindTemplateTypes *tt = new FindTemplateTypes(*mit.second);
    InterfaceDecl *pd = new InterfaceDecl(name, tt);

    _ointerfaces.insert(interfacePairType(name, pd));
  }
}

void ModuleDecl::addInputOutputInterfaces( FindTLMInterfaces::interfaceType p ) {
  //  for (FindTLMInterfaces::interfaceType::iterator mit = p.begin(), mite = p.end();    mit != mite; mit++) {
  for ( auto mit: p ) {
    _iointerfaces.insert(interfacePairType
                         (mit.first, new InterfaceDecl(mit.first, mit.second)));
  }
}


void ModuleDecl::addConstructor( Stmt *constructor ) {
  _constructorStmt = constructor;
}

void ModuleDecl::addProcess( FindEntryFunctions::entryFunctionVectorType * efv ) {
  _vef = *efv;
  for ( unsigned int i = 0; i < efv->size(); ++i ) {

    EntryFunctionContainer *ef = (*efv)[i];

    // Set the entry name.
    string entryName = ef->_entryName;
    string entryType = "";

    // Set the process type
    switch (ef->_procType) {
    case THREAD: {
      entryType = "SC_THREAD";
      break;
    }
    case METHOD: {
      entryType = "SC_METHOD";
      break;
    }
    case CTHREAD: {
      entryType = "SC_CTHREAD";
      break;
    }
    default:{
      entryType = "ERROR";
      break;
    }
 

      _processes.
        insert(processPairType
               (entryName,
                new ProcessDecl(entryType, entryName, ef->_entryMethodDecl, ef)));
    }
  }
}

vector<string> ModuleDecl::getInstanceList() {
  return _instanceList;
}

vector<EntryFunctionContainer*> ModuleDecl::getEntryFunctionContainer() {
  return _vef;
}

int ModuleDecl::getNumInstances() {
  return _instanceList.size();
}

ModuleDecl::processMapType ModuleDecl::getProcessMap() {
  return _processes;
}

ModuleDecl::portMapType ModuleDecl::getOPorts() {
  return _oports;
}

ModuleDecl::portMapType ModuleDecl::getIPorts() {
  return _iports;
}

ModuleDecl::portMapType ModuleDecl::getIOPorts() {
  return _ioports;
}

ModuleDecl::interfaceMapType ModuleDecl::getOInterfaces() {
  return _ointerfaces;
}

ModuleDecl::interfaceMapType ModuleDecl::getIInterfaces() {
  return _iinterfaces;
}

ModuleDecl::interfaceMapType ModuleDecl::getIOInterfaces() {
  return _iointerfaces;
}


string ModuleDecl::getName() {
  return _moduleName;
}

bool ModuleDecl::isModuleClassDeclNull() {
  return ( _classdecl == nullptr );
}

CXXRecordDecl *ModuleDecl::getModuleClassDecl() {
  assert(!( _classdecl == nullptr ));
  return _classdecl;
}

void ModuleDecl::dumpInstances(raw_ostream & os, int tabn) {

  if ( _instanceList.empty() ) {
    os << " none \n";
  }

  for (size_t i = 0; i < _instanceList.size(); i++) {
    os <<_instanceList.at(i)<<" ";
  }
}

void ModuleDecl::dumpSignalBinding(raw_ostream & os ,int tabn) {
  if ( _portSignalMap.empty() ) {
    os << " none\n";
    return;
  }

  for ( auto it: _portSignalMap ) {
    os <<"\nPort : " <<it.first<<" bound to signal : " <<it.second;
  }
}

void ModuleDecl::dumpProcesses(raw_ostream & os, int tabn) {
  if ( _processes.size() == 0 ) {
    os << "none \n";
  } else {
    for ( auto pit: _processes ) {
      ProcessDecl *pd = pit.second;
      pd->dump(os, tabn);
      os << "\n";
    }
  }
  os << "\n";
}

void ModuleDecl::dumpInterfaces(raw_ostream & os, int tabn) {

  os << "Input interfaces: " << _iinterfaces.size() << "\n";

  if (_iinterfaces.size() == 0) {
    os << " none\n";
  } else {
    for ( auto mit: _iinterfaces ) {
      mit.second->dump(os, tabn);
      os << "\n ";
    }
    os << "\n";
  }

  os << "Output interfaces: " << _ointerfaces.size() << "\n";
  if (_ointerfaces.size() == 0) {
    os << "none \n";
  } else {
    for ( auto mit: _ointerfaces ) {
      mit.second->dump(os, tabn);
      os << "\n ";
    }
    os << "\n";
  }

  os << "Inout interfaces: " << _iointerfaces.size() << "\n";
  if (_iointerfaces.size() == 0) {
    os << "none \n";
  } else {
    for ( auto mit: _iointerfaces ) {
      mit.second->dump(os, tabn);
      os << "\n ";
    }
    os << "\n";
  }
}

void ModuleDecl::dumpPorts(raw_ostream & os, int tabn) {
  os << "Input ports: " << _iports.size();

  if (_iports.size() == 0) {
    os << "\n none \n";
  } else {
    os << "\n ";
    for ( auto mit: _iports ) {
      mit.second->dump(os);
      os << "\n ";
    }
    os << "\n";
  }

  os << "Output ports: " << _oports.size();
  if ( _oports.size() == 0 ) {
    os << "\n none \n";
  } else {
    os << "\n ";
    for ( auto mit: _oports ) {
      mit.second->dump(os, tabn);
      os << "\n";
    }
  }

  os << "Inout ports: " << _ioports.size();
  if ( _ioports.size() == 0 ) {
    os << "\n none \n";
  } else {
    os << "\n ";
    for ( auto mit: _oports ) {
      mit.second->dump(os, tabn);
      os << "\n ";
    }
  }
  os << "\n";
}

void ModuleDecl::dumpSignals( raw_ostream & os, int tabn ) {
  if ( _signals.size() == 0 ) {
    os << "none \n";
  } else {
    for ( auto sit: _signals ) {
      Signal *s = sit.second;
      s->dump(os, tabn);
      os << "\n";
    }
  }
  os << "\n";
}


void ModuleDecl::dump(raw_ostream & os) {
  //  os << "ModuleDecl " << this << " " << _moduleName
  //     << " CXXRecordDecl " << _classdecl << "\n";

  os << "\n";
  os << "# Port Declaration:\n";
  dumpPorts(os, 4);
  os << "# Signal Declaration:\n";
  dumpSignals(os, 4);
  os << "# Processes:\n";
  dumpProcesses(os, 4);
  os <<"# Instances:\n";
  dumpInstances(os, 4);
  os <<"# Signal binding:\n";
  dumpSignalBinding(os, 4);
  os << "\n=======================================================\n";
}
