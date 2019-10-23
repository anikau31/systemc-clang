#ifndef _MODULE_DECL_H_
#define _MODULE_DECL_H_
#include "systemc-clang.h"
#include "json.hpp"

#include "FindConstructor.h"
#include "FindEntryFunctions.h"
#include "FindPorts.h"
#include "FindTLMInterfaces.h"
#include "InterfaceDecl.h"
#include "PortDecl.h"
#include "ProcessDecl.h"
#include "Signal.h"
#include "Utility.h"
#include "clang/AST/DeclCXX.h"
#include <map>
#include <string>

namespace scpar {
  using namespace clang;
  using namespace std;
  using json = nlohmann::json;

class ModuleDecl {
public:
  typedef pair<string, Signal* > signalPairType;
  typedef map<string, Signal* > signalMapType;

  // Maps the name of the port with a pointer to a structure that holds
  // information about the port.
  typedef pair<string, PortDecl* > portPairType;
  //typedef map<string, PortDecl* > portMapType;

  typedef pair<string, InterfaceDecl* > interfacePairType;
  typedef map<string, InterfaceDecl* > interfaceMapType;

  // Maps the name of the process with a pointer to a structure that holds
  // information about the process.
  typedef pair<string, ProcessDecl* > processPairType;
  typedef map<string, ProcessDecl* > processMapType;

  typedef pair<string, string> moduleProcessPairType;

  typedef pair<string, string> portSignalPairType;
  typedef map<string, string> portSignalMapType;

  // Why is this a not a Type?
  typedef vector<string> instanceName;

  // PortType
    typedef std::vector< std::tuple<std::string, PortDecl*> > PortType;
    typedef std::vector< std::tuple<std::string, PortDecl*> > portMapType;
public:
  ModuleDecl();
  ModuleDecl(const string &, CXXRecordDecl *);
  ModuleDecl(const std::tuple< const std::string &, CXXRecordDecl* > & );

  ~ModuleDecl();

  void addSignals(const FindSignals::signalMapType & );
  void addInputPorts(const FindPorts::PortType& );
  void addOutputPorts(const FindPorts::PortType& );
  void addInputOutputPorts(const FindPorts::PortType& );
  void addOtherVars(const FindPorts::PortType&);
  void addPorts( const PortType& found_ports, const std::string & port_type );

  void addInputInterfaces(FindTLMInterfaces::interfaceType);
  void addOutputInterfaces(FindTLMInterfaces::interfaceType);
  void addInputOutputInterfaces(FindTLMInterfaces::interfaceType);
  void addProcess(FindEntryFunctions::entryFunctionVectorType *);
  void addInstances(const vector<string> & );
  void addSignalBinding(map<string, string>);
  void setModuleName(const string &);
  void setTemplateParameters(const vector<string> &);
    vector<string> getTemplateParameters() const;
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
  vector<EntryFunctionContainer *> getEntryFunctionContainer();
  int getNumInstances();
  signalMapType getSignals();

  void dumpPorts(raw_ostream &, int);
  void dumpInterfaces(raw_ostream &, int);
  void dumpProcesses(raw_ostream &, int);
  void dumpSignals(raw_ostream &, int);
  void dump(raw_ostream &);
  void dumpInstances(raw_ostream &, int);
  void dumpSignalBinding(raw_ostream &, int);

  json dump_json();

private:
  string module_name_;
  CXXRecordDecl *class_decl_;
  Stmt *constructor_stmt_;

  processMapType process_map_;
  portMapType in_ports_;
  portMapType out_ports_;
  portMapType inout_ports_;
  portMapType other_fields_;

  interfaceMapType _iinterfaces;
  interfaceMapType _ointerfaces;
  interfaceMapType _iointerfaces;
  signalMapType _signals;

  vector<string> _instanceList;
  portSignalMapType _portSignalMap;
  vector<EntryFunctionContainer *> _vef;

    // Class template parameters.
    vector<string> template_parameters_;
};
} // namespace scpar
#endif
