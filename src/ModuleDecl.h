#ifndef _MODULE_DECL_H_
#define _MODULE_DECL_H_
#include <map>
#include <string>

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
#include "json.hpp"
#include "systemc-clang.h"

namespace scpar {
using namespace clang;
using namespace std;
using json = nlohmann::json;

class ModuleDecl {
 public:
  typedef pair<string, Signal *> signalPairType;
  typedef map<string, Signal *> signalMapType;

  // Maps the name of the port with a pointer to a structure that holds
  // information about the port.
  typedef pair<string, PortDecl *> portPairType;
  // typedef map<string, PortDecl* > portMapType;

  typedef pair<string, InterfaceDecl *> interfacePairType;
  typedef map<string, InterfaceDecl *> interfaceMapType;

  // Maps the name of the process with a pointer to a structure that holds
  // information about the process.
  typedef pair<string, ProcessDecl *> processPairType;
  typedef map<string, ProcessDecl *> processMapType;

  typedef pair<string, string> moduleProcessPairType;

  typedef pair<string, string> portSignalPairType;
  typedef map<string, string> portSignalMapType;

  // Why is this a not a Type?
  typedef vector<string> instanceName;

  // PortType
  typedef std::vector<std::tuple<std::string, PortDecl *> > PortType;
  typedef std::vector<std::tuple<std::string, PortDecl *> > portMapType;

 public:
  ModuleDecl();
  ModuleDecl(const string &, CXXRecordDecl *);
  ModuleDecl(const std::tuple<const std::string &, CXXRecordDecl *> &);

  // Copy constructor.
  ModuleDecl(const ModuleDecl &from);

  // Assignment operator.
  ModuleDecl &operator=(const ModuleDecl &from);
  ~ModuleDecl();

  void addSignals(const FindSignals::signalMapType &);
  void addInputPorts(const FindPorts::PortType &);
  void addOutputPorts(const FindPorts::PortType &);
  void addInputOutputPorts(const FindPorts::PortType &);

  void addInputStreamPorts(FindPorts::PortType);
  void addOutputStreamPorts(FindPorts::PortType);
  void addOtherVars(const FindPorts::PortType &);
  void addPorts(const PortType &found_ports, const std::string &port_type);

  void addConstructor(Stmt *);
  void addInputInterfaces(FindTLMInterfaces::interfaceType);
  void addOutputInterfaces(FindTLMInterfaces::interfaceType);
  void addInputOutputInterfaces(FindTLMInterfaces::interfaceType);
  void addProcess(FindEntryFunctions::entryFunctionVectorType *);
  void addInstances(const vector<string> &);
  void addSignalBinding(map<string, string>);

  void setInstanceName(const string &);
  void setModuleName(const string &);
  void setTemplateParameters(const vector<string> &);
  vector<string> getTemplateParameters() const;

  string getName() const;
  string getInstanceName() const;

  CXXRecordDecl *getModuleClassDecl();
  FieldDecl *getInstanceFieldDecl();
  VarDecl *getInstanceVarDecl();
  bool isInstanceFieldDecl() const;

  bool isModuleClassDeclNull();
  portMapType getOPorts();
  portMapType getIPorts();
  portMapType getIOPorts();
  portMapType getOtherVars();
  portMapType getInputStreamPorts();
  portMapType getOutputStreamPorts();

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
  string instance_name_;

  // Declaration
  CXXRecordDecl *class_decl_;
  // Constructor statement
  Stmt *constructor_stmt_;
  // Instance fieldDecl or varDecl
  FieldDecl *instance_field_decl_;
  VarDecl *instance_var_decl_;

  processMapType process_map_;
  portMapType in_ports_;
  portMapType out_ports_;
  portMapType inout_ports_;
  portMapType other_fields_;

  portMapType istreamports_;
  portMapType ostreamports_;

  interfaceMapType iinterfaces_;
  interfaceMapType ointerfaces_;
  interfaceMapType iointerfaces_;
  signalMapType signals_;

  vector<string> instance_list_;
  portSignalMapType port_signal_map_;
  vector<EntryFunctionContainer *> vef_;

  // Class template parameters.
  vector<string> template_parameters_;
};
}  // namespace scpar
#endif
