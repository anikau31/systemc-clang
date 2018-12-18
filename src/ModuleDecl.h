#ifndef _MODULE_DECL_H_
#define _MODULE_DECL_H_

#include "clang/AST/DeclCXX.h"
#include <string>
#include <map>
#include "PortDecl.h"
#include "InterfaceDecl.h"
#include "ProcessDecl.h"
#include "Signal.h"
#include "FindPorts.h"
#include "FindTLMInterfaces.h"
#include "FindEntryFunctions.h"
#include "FindConstructor.h"
#include "Utility.h"

namespace scpar {
  using namespace clang;
  using namespace std;

  class ModuleDecl {
  public:

    typedef pair < string, Signal * > signalPairType;
    typedef map < string, Signal * > signalMapType;

    typedef pair < string, PortDecl * > portPairType;
    typedef map < string, PortDecl * > portMapType;

    typedef pair <string, InterfaceDecl *> interfacePairType;
    typedef map <string, InterfaceDecl *> interfaceMapType;

    typedef pair < string, ProcessDecl * > processPairType;
    typedef map < string, ProcessDecl * > processMapType;

    typedef pair < string, string > moduleProcessPairType;

    typedef pair<string, string> portSignalPairType;
    typedef map<string, string> portSignalMapType;

    // Why is this a not a Type?
    typedef vector<string> instanceName;

  public:
    ModuleDecl();
    ModuleDecl(const string &, CXXRecordDecl *);

    ~ModuleDecl();

    void addSignals( FindSignals::signalMapType * );
    void addInputPorts( FindPorts::PortType );
    void addOutputPorts( FindPorts::PortType );
    void addInputOutputPorts( FindPorts::PortType );
    void addInputInterfaces( FindTLMInterfaces::interfaceType );
    void addOutputInterfaces( FindTLMInterfaces::interfaceType );
    void addInputOutputInterfaces( FindTLMInterfaces::interfaceType );
    void addProcess(FindEntryFunctions::entryFunctionVectorType *);
    void addInstances(vector<string>);
    void addSignalBinding(map<string, string>);
    void setModuleName(const string &);
    void addConstructor(Stmt *);
    string getName();
    CXXRecordDecl *getModuleClassDecl();
    bool isModuleClassDeclNull();
    portMapType getOPorts();
    portMapType getIPorts();
    portMapType getIOPorts();
    processMapType getProcessMap();
    Stmt *getConstructorStmt();
    interfaceMapType getIInterfaces();
    interfaceMapType getOInterfaces();
    interfaceMapType getIOInterfaces();
    vector<string> getInstanceList();
    vector<EntryFunctionContainer*> getEntryFunctionContainer();
    int getNumInstances();

    void dumpPorts(raw_ostream &, int);
    void dumpInterfaces(raw_ostream &, int);
    void dumpProcesses(raw_ostream &, int);
    void dumpSignals(raw_ostream &, int);
    void dump(raw_ostream &);
    void dumpInstances(raw_ostream&, int);
    void dumpSignalBinding(raw_ostream&, int);

  private:
    string _moduleName;
    CXXRecordDecl *_classdecl;
    processMapType _processes;
    portMapType _iports;
    portMapType _oports;
    portMapType _ioports;
    interfaceMapType _iinterfaces;
    interfaceMapType _ointerfaces;
    interfaceMapType _iointerfaces;
    signalMapType _signals;
    Stmt *_constructorStmt;
    vector<string> _instanceList;
    portSignalMapType _portSignalMap;
    vector<EntryFunctionContainer*> _vef;
  };
}
#endif
