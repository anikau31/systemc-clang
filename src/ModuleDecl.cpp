#include <string>
#include "ModuleDecl.h"

using namespace scpar;

using std::string;

ModuleDecl::ModuleDecl():
_moduleName("NONE"),
_classdecl(nullptr) {
}

ModuleDecl::ModuleDecl(const string &name,
                       CXXRecordDecl *decl):
  _moduleName(name),
  _classdecl(decl) {

}

ModuleDecl::~ModuleDecl() {

  // Delete all pointers in ports.
  for (ModuleDecl::portMapType::iterator mit = _iports.begin();
       mit != _iports.end(); mit++) {
    // Second is the PortDecl*.
    delete mit->second;
  }
  _iports.clear();

  for (ModuleDecl::portMapType::iterator mit = _oports.begin();
       mit != _oports.end(); mit++) {
    // Second is the PortDecl*.
    delete mit->second;
  }
  _oports.clear();

  for (ModuleDecl::portMapType::iterator mit = _ioports.begin();
       mit != _ioports.end(); mit++) {
    // Second is the PortDecl*.
    delete mit->second;
  }
  _ioports.clear();

}

void ModuleDecl::setModuleName(const string & name)
{
  _moduleName = name;
}

void ModuleDecl::addInstances(vector<string> instanceList) {
 _instanceList = instanceList; 
}

void ModuleDecl::addSignalBinding(map<string, string> portSignalMap) {
 _portSignalMap.insert(portSignalMap.begin(), portSignalMap.end());
}

void ModuleDecl::addSignals(FindSignals::signalMapType * s)
{
  for (FindSignals::signalMapType::iterator sit = s->begin();
       sit != s->end(); sit++) {
    string n = sit->first;

    // It is important to create new objects. 
    // This is because the objects created during Find*
    // may go outside scope, and free up allocated memory.
    SignalContainer *sc = new SignalContainer(*sit->second);
    Signal *sig = new Signal(n, sc);

    _signals.insert(ModuleDecl::signalPairType(n, sig));

  }
}

void ModuleDecl::addInputPorts(FindPorts::portType p)
{
  for (FindPorts::portType::iterator mit = p.begin(), mite = p.end();
       mit != mite; mit++) {
    string n = mit->first;
    FindTemplateTypes *tt = new FindTemplateTypes(*mit->second);
    PortDecl *pd = new PortDecl(n, tt);

    _iports.insert(portPairType(mit->first, pd));
  }
}

void ModuleDecl::addOutputPorts(FindPorts::portType p)
{
  for (FindPorts::portType::iterator mit = p.begin(), mite = p.end();
       mit != mite; mit++) {
    string n = mit->first;
    FindTemplateTypes *tt = new FindTemplateTypes(*mit->second);
    PortDecl *pd = new PortDecl(n, tt);

    _oports.insert(portPairType(n, pd));
  }
}

void ModuleDecl::addInputOutputPorts(FindPorts::portType p)
{
  for (FindPorts::portType::iterator mit = p.begin(), mite = p.end();
       mit != mite; mit++) {
    _ioports.insert(portPairType
                    (mit->first, new PortDecl(mit->first, mit->second)));
  }
}

void ModuleDecl::addInputInterfaces(FindTLMInterfaces::interfaceType p)
{
  for (FindTLMInterfaces::interfaceType::iterator mit = p.begin(), mite = p.end();
       mit != mite; mit++) {
    string n = mit->first;
    FindTemplateTypes *tt = new FindTemplateTypes(*mit->second);
    InterfaceDecl *pd = new InterfaceDecl(n, tt);

    _iinterfaces.insert(interfacePairType(mit->first, pd));
  }
}

void ModuleDecl::addOutputInterfaces(FindTLMInterfaces::interfaceType p)
{
  for (FindTLMInterfaces::interfaceType::iterator mit = p.begin(), mite = p.end();
       mit != mite; mit++) {
    string n = mit->first;
    FindTemplateTypes *tt = new FindTemplateTypes(*mit->second);
    InterfaceDecl *pd = new InterfaceDecl(n, tt);

    _ointerfaces.insert(interfacePairType(n, pd));
  }
}

void ModuleDecl::addInputOutputInterfaces(FindTLMInterfaces::interfaceType p)
{
  for (FindTLMInterfaces::interfaceType::iterator mit = p.begin(), mite = p.end();
       mit != mite; mit++) {
    _iointerfaces.insert(interfacePairType
                    (mit->first, new InterfaceDecl(mit->first, mit->second)));
  }
}


void ModuleDecl::addConstructor(Stmt * constructor)
{
  _constructorStmt = constructor;
}

void ModuleDecl::addProcess(FindEntryFunctions::entryFunctionVectorType * efv)
{
  _vef = *efv;
  for (unsigned int i = 0; i < efv->size(); i++) {

    EntryFunctionContainer *ef = (*efv)[i];

    // Set the entry name.
    string entryName = ef->_entryName;
    string entryType = "";

    // Set the process type
    switch (ef->_procType) {
      case THREAD:
        entryType = "SC_THREAD";
        break;
      case METHOD:
        entryType = "SC_METHOD";
        break;
      case CTHREAD:
        entryType = "SC_CTHREAD";
        break;
      default:
        entryType = "ERROR";
        break;
    }

    _processes.
      insert(processPairType
             (entryName,
              new ProcessDecl(entryType, entryName, ef->_entryMethodDecl, ef)));
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

ModuleDecl::processMapType ModuleDecl::getProcessMap()
{
  return _processes;
}

ModuleDecl::portMapType ModuleDecl::getOPorts()
{
  return _oports;
}

ModuleDecl::portMapType ModuleDecl::getIPorts()
{
  return _iports;
}

ModuleDecl::portMapType ModuleDecl::getIOPorts()
{
  return _ioports;
}

ModuleDecl::interfaceMapType ModuleDecl::getOInterfaces()
{
  return _ointerfaces;
}

ModuleDecl::interfaceMapType ModuleDecl::getIInterfaces()
{
  return _iinterfaces;
}

ModuleDecl::interfaceMapType ModuleDecl::getIOInterfaces()
{
  return _iointerfaces;
}


string ModuleDecl::getName()
{
  return _moduleName;
}

bool ModuleDecl::isModuleClassDeclNull()
{
  return (_classdecl == NULL);
}

CXXRecordDecl *ModuleDecl::getModuleClassDecl()
{
  assert(!(_classdecl == NULL));
  return _classdecl;
}

void ModuleDecl::dumpInstances(raw_ostream & os, int tabn) {

 for (size_t i = 0; i < _instanceList.size(); i++) {
  os <<_instanceList.at(i)<<" ";
 }
}

void ModuleDecl::dumpSignalBinding(raw_ostream & os ,int tabn) {
 for (portSignalMapType::iterator it = _portSignalMap.begin(), eit = _portSignalMap.end();
     it != eit;
     it++) {
   os <<"\n Port : " <<it->first<<" bound to signal : " <<it->second;
 }
}

void ModuleDecl::dumpProcesses(raw_ostream & os, int tabn)
{
  if (_processes.size() == 0) {
    os << "    <<<NULL>>>\n";
  } else {
    for (processMapType::iterator pit = _processes.begin();
         pit != _processes.end(); pit++) {

      ProcessDecl *pd = pit->second;

      pd->dump(os, tabn);
      os << "\n";
    }
  }
  os << "\n";
}

void ModuleDecl::dumpInterfaces(raw_ostream & os, int tabn)
{

  for (int i = 0; i < tabn; i++) {
    os << " ";
  }
  os << "Input interfaces:\n ";

  if (_iinterfaces.size() == 0) {
    os << "   <<<NULL>>>\n";
  } else {
    for (ModuleDecl::interfaceMapType::iterator mit = _iinterfaces.begin();
         mit != _iinterfaces.end(); mit++) {

      mit->second->dump(os, tabn);
      os << "\n ";
    }
    os << "\n";
  }

  for (int i = 0; i < tabn; i++) {
    os << " ";
  }
  os << "Output interfaces:\n ";
  if (_ointerfaces.size() == 0) {
    os << "   <<<NULL>>>\n";
  } else {
    for (ModuleDecl::interfaceMapType::iterator mit = _ointerfaces.begin();
         mit != _ointerfaces.end(); mit++) {

      mit->second->dump(os, tabn);
      os << "\n ";
      os << "\n";
    }
  }

  for (int i = 0; i < tabn; i++) {
    os << " ";
  }
  os << "Inout interfaces:\n ";

  if (_iointerfaces.size() == 0) {
    os << "    <<<NULL>>>\n";
  } else {
    for (ModuleDecl::interfaceMapType::iterator mit = _iointerfaces.begin();
         mit != _iointerfaces.end(); mit++) {

      mit->second->dump(os, tabn);
      os << "\n ";
    }
  }
  os << "\n";
}



void ModuleDecl::dumpPorts(raw_ostream & os, int tabn)
{

  for (int i = 0; i < tabn; i++) {
    os << " ";
  }
  os << "Input ports:\n ";

  if (_iports.size() == 0) {
    os << "   <<<NULL>>>\n";
  } else {
    for (ModuleDecl::portMapType::iterator mit = _iports.begin();
         mit != _iports.end(); mit++) {

      mit->second->dump(os, tabn);
      os << "\n ";
    }
    os << "\n";
  }

  for (int i = 0; i < tabn; i++) {
    os << " ";
  }
  os << "Output ports:\n ";
  if (_oports.size() == 0) {
    os << "   <<<NULL>>>\n";
  } else {
    for (ModuleDecl::portMapType::iterator mit = _oports.begin();
         mit != _oports.end(); mit++) {

      mit->second->dump(os, tabn);
      os << "\n ";
      os << "\n";
    }
  }

  for (int i = 0; i < tabn; i++) {
    os << " ";
  }
  os << "Inout ports:\n ";

  if (_ioports.size() == 0) {
    os << "    <<<NULL>>>\n";
  } else {
    for (ModuleDecl::portMapType::iterator mit = _ioports.begin();
         mit != _ioports.end(); mit++) {

      mit->second->dump(os, tabn);
      os << "\n ";
    }
  }
  os << "\n";
}

void ModuleDecl::dumpSignals(raw_ostream & os, int tabn)
{

  if (_signals.size() == 0) {
    os << "     <<<NULL>>>\n";
  } else {
    for (ModuleDecl::signalMapType::iterator sit = _signals.begin();
         sit != _signals.end(); sit++) {
      Signal *s = sit->second;

      s->dump(os, tabn);
      os << "\n";
    }
  }
  os << "\n";
}


void ModuleDecl::dump(raw_ostream & os)
{

  os << "\n=======================================================\n";
  os << " ModuleDecl " << this << " " << _moduleName << " CXXRecordDecl " <<
    _classdecl << "\n";

  os << "  Port Declaration:>\n";
  dumpPorts(os, 4);
  os << "  Signal Declaration:>\n";
  dumpSignals(os, 4);
  os << "  Processes:>\n";
  dumpProcesses(os, 4);
  os <<" Instances:>\n";
  dumpInstances(os, 4);
  os <<" Signal binding:>\n";
  dumpSignalBinding(os, 4);
  os << "\n=======================================================\n";
}
