#ifndef _MODULE_DECL_H_
#define _MODULE_DECL_H_

/* clang-format off */
#include <map>
#include <string>

/* clang-format on */

#include "FindConstructor.h"
#include "FindEntryFunctions.h"
#include "FindTLMInterfaces.h"
#include "InterfaceDecl.h"
#include "PortBinding.h"
#include "PortDecl.h"
#include "ProcessDecl.h"
#include "SignalDecl.h"
#include "clang/AST/DeclCXX.h"

#include "ModuleInstanceType.h"

namespace scpar {
using namespace clang;

// Forward declarations
//

class ModuleDecl {
 public:
  typedef std::pair<std::string, SignalDecl *> signalPairType;
  typedef std::map<std::string, SignalDecl *> signalMapType;

  typedef std::pair<std::string, InterfaceDecl *> interfacePairType;
  typedef std::map<std::string, InterfaceDecl *> interfaceMapType;

  // Maps the name of the process with a pointer to a structure that holds
  // information about the process.
  typedef std::pair<std::string, ProcessDecl *> processPairType;
  typedef std::map<std::string, ProcessDecl *> processMapType;

  typedef std::pair<std::string, std::string> moduleProcessPairType;

  typedef std::pair<std::string, std::string> portSignalPairType;
  typedef std::map<std::string, std::string> portSignalMapType;

  // Why is this a not a Type?
  typedef std::vector<std::string> instanceName;

  // string: name of bound port, PortBinding*: structure with more info.
  typedef std::pair<std::string, PortBinding *> portBindingPairType;
  typedef std::map<std::string, PortBinding *> portBindingMapType;

  // PortType
  typedef std::vector<std::tuple<std::string, PortDecl *> > PortType;
  typedef std::vector<std::tuple<std::string, PortDecl *> > portMapType;

 public:
  ModuleDecl();
  ModuleDecl(const std::string &, CXXRecordDecl *);
  ModuleDecl(const std::tuple<const std::string &, CXXRecordDecl *> &);

  // Copy constructor.
  ModuleDecl(const ModuleDecl &from);

  // Assignment operator.
  ModuleDecl &operator=(const ModuleDecl &from);
  ~ModuleDecl();

  void addPorts(const PortType &found_ports, const std::string &port_type);

  void addConstructor(FindConstructor *);
  void addConstructor(Stmt *);
  void addInputInterfaces(FindTLMInterfaces::interfaceType);
  void addOutputInterfaces(FindTLMInterfaces::interfaceType);
  void addInputOutputInterfaces(FindTLMInterfaces::interfaceType);
  void addProcess(FindEntryFunctions::entryFunctionVectorType *);
  void addInstances(const vector<string> &);

  void addPortBinding(const std::string &port_name, PortBinding *pb);
  void addSignalBinding(map<std::string, std::string>);

  void addNestedModule(ModuleDecl *submodule);

  void setInstanceInfo(const sc_ast_matchers::ModuleInstanceType &info);
  void setInstanceName(const std::string &);
  void setInstanceDecl(Decl *);
  void setModuleName(const std::string &);
  void setTemplateParameters(const vector<std::string> &);
  void setTemplateArgs(const vector<std::string> &);
  vector<std::string> getTemplateParameters() const;
  vector<std::string> getTemplateArgs() const;

  std::string getName() const;
  std::string getInstanceName() const;

  CXXRecordDecl *getModuleClassDecl();
  FieldDecl *getInstanceFieldDecl();
  VarDecl *getInstanceVarDecl();
  Decl *getInstanceDecl();
  bool isInstanceFieldDecl() const;

  bool isModuleClassDeclNull();
  portMapType getOPorts();
  portMapType getIPorts();
  portMapType getIOPorts();
  portMapType getOtherVars();
  portMapType getSubmodules();
  portMapType getInputStreamPorts();
  portMapType getOutputStreamPorts();

  portBindingMapType getPortBindings();

  processMapType getProcessMap();

  Stmt *getConstructorStmt() const;
  CXXConstructorDecl *getConstructorDecl() const;

  interfaceMapType getIInterfaces();
  interfaceMapType getOInterfaces();
  interfaceMapType getIOInterfaces();
  vector<std::string> getInstanceList();
  vector<EntryFunctionContainer *> getEntryFunctionContainer();
  int getNumInstances();
  const signalMapType &getSignals() const;

  const std::vector<ModuleDecl*> &getNestedModuleDecl() const ;

  void dumpPorts(raw_ostream &, int);
  void dumpPortBinding();
  void dumpInterfaces(raw_ostream &, int);
  void dumpProcesses(raw_ostream &, int);
  void dumpSignals(raw_ostream &, int);
  void dump(raw_ostream &);
  void dumpInstances(raw_ostream &, int);
  void dumpSignalBinding(raw_ostream &, int);

  void clearOnlyGlobal();

  json dump_json();

 private:
  std::string module_name_;
  std::string instance_name_;
  sc_ast_matchers::ModuleInstanceType instance_info_;

  // Declaration
  CXXRecordDecl *class_decl_;
  // Constructor statement
  Stmt *constructor_stmt_;
  CXXConstructorDecl *constructor_decl_;
  // Instance fieldDecl or varDecl
  Decl *instance_decl_;

  processMapType process_map_;
  portMapType in_ports_;
  portMapType out_ports_;
  portMapType inout_ports_;
  portMapType other_fields_;
  portMapType submodules_;

  portMapType istreamports_;
  portMapType ostreamports_;

  portBindingMapType port_bindings_;

  interfaceMapType iinterfaces_;
  interfaceMapType ointerfaces_;
  interfaceMapType iointerfaces_;
  signalMapType signals_;

  vector<std::string> instance_list_;
  portSignalMapType port_signal_map_;
  vector<EntryFunctionContainer *> vef_;

  // Nested modules
  std::vector<ModuleDecl*> nested_modules_;

  // Class template parameters.
  vector<std::string> template_parameters_;
  vector<std::string> template_args_;
};
}  // namespace scpar
#endif
