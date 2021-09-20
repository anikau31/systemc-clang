#ifndef _MODULE_INSTANCE_H_
#define _MODULE_INSTANCE_H_

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

namespace systemc_clang {
using namespace sc_ast_matchers;

/// Forward declarations
//

class ModuleInstance {
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
  /// Default constructor.
  ModuleInstance();

  /// Overloaded constructors.
  ModuleInstance(const std::string &, const clang::CXXRecordDecl *);
  ModuleInstance(
      const std::tuple<const std::string &, clang::CXXRecordDecl *> &);

  // Copy constructor.
  ModuleInstance(const ModuleInstance &from);

  // Assignment operator.
  ModuleInstance &operator=(const ModuleInstance &from);
  virtual ~ModuleInstance();

  void addPorts(const PortType &found_ports, const std::string &port_type);

  void addConstructor(FindConstructor *);
  void addConstructor(clang::Stmt *);
  void addInputInterfaces(FindTLMInterfaces::interfaceType);
  void addOutputInterfaces(FindTLMInterfaces::interfaceType);
  void addInputOutputInterfaces(FindTLMInterfaces::interfaceType);
  void addProcess(FindEntryFunctions::entryFunctionVectorType *);
  void addInstances(const std::vector<std::string> &);

  void addPortBinding(const std::string &port_name, PortBinding *pb);
  void addSignalBinding(std::map<std::string, std::string>);
  void addBaseInstance(ModuleInstance *);

  void addNestedModule(ModuleInstance *submodule);

  void addResetSignal(std::pair<std::string, const clang::Expr*> reset_signal);
  void addResetEdge(std::pair<std::string, const clang::Expr*> reset_edge);
  void addResetType(bool reset_type);

  void setInstanceInfo(const sc_ast_matchers::ModuleInstanceType &info);
  void setInstanceName(const std::string &);
  void setInstanceDecl(Decl *);
  void setModuleName(const std::string &);
  void setTemplateParameters(const vector<std::string> &);
  void setTemplateArgs(const vector<std::string> &);
  const std::vector<std::string> &getTemplateParameters() const;

  std::string getName() const;
  std::string getInstanceName() const;

  const clang::CXXRecordDecl *getModuleClassDecl();
  clang::FieldDecl *getInstanceFieldDecl();
  clang::VarDecl *getInstanceVarDecl();
  const clang::Decl *getInstanceDecl();
  bool isInstanceFieldDecl() const;

  ModuleInstanceType getInstanceInfo();

  bool isModuleClassDeclNull();
  const portMapType &getOPorts();
  const portMapType &getIPorts();
  const portMapType &getIOPorts();
  const portMapType &getOtherVars();
  const portMapType &getSubmodules();
  const portMapType &getInputStreamPorts();
  const portMapType &getOutputStreamPorts();

  const std::vector<ModuleInstance *> &getBaseInstances();
  const portBindingMapType &getPortBindings();

  const processMapType &getProcessMap();

  const clang::Stmt *getConstructorStmt() const;
  const clang::CXXConstructorDecl *getConstructorDecl() const;

  const interfaceMapType &getIInterfaces();
  const interfaceMapType &getOInterfaces();
  const interfaceMapType &getIOInterfaces();
  const std::vector<std::string> &getInstanceList();
  const std::vector<EntryFunctionContainer *> &getEntryFunctionContainer();
  int getNumInstances() const;
  const signalMapType &getSignals() const;

  const std::vector<ModuleInstance *> &getNestedModuleInstances() const;

  const std::pair<std::string, const clang::Expr*> getResetSignal() const;
  const std::pair<std::string, const clang::Expr*> getResetEdge() const;
  bool isResetAsync() const;

  void dumpPorts(llvm::raw_ostream &);
  std::string dumpPortBinding();
  void dumpInterfaces(llvm::raw_ostream &, int);
  void dumpProcesses(llvm::raw_ostream &, int);
  void dumpSignals(llvm::raw_ostream &, int);
  void dump(llvm::raw_ostream &);
  void dumpInstances(llvm::raw_ostream &, int);
  void dumpSignalBinding(llvm::raw_ostream &, int);
  void dump_base_instances(llvm::raw_ostream &os);

  void clearOnlyGlobal();

  std::string dump_json();

 private:
  std::string module_name_;
  std::string instance_name_;
  ModuleInstanceType instance_info_;

  std::vector<ModuleInstance *> base_instances_;

  // Declaration
  clang::CXXRecordDecl *class_decl_;
  // Constructor statement
  clang::Stmt *constructor_stmt_;
  clang::CXXConstructorDecl *constructor_decl_;
  // Instance fieldDecl or varDecl
  clang::Decl *instance_decl_;

  processMapType process_map_;
  portMapType in_ports_;
  portMapType out_ports_;
  portMapType inout_ports_;
  portMapType other_fields_;

  portMapType istreamports_;
  portMapType ostreamports_;

  portBindingMapType port_bindings_;

  interfaceMapType iinterfaces_;
  interfaceMapType ointerfaces_;
  interfaceMapType iointerfaces_;
  signalMapType signals_;

  std::vector<std::string> instance_list_;
  portSignalMapType port_signal_map_;
  std::vector<EntryFunctionContainer *> vef_;

  /// Nested modules
  std::vector<ModuleInstance *> nested_modules_;

  /// Class template parameters.
  std::vector<std::string> template_parameters_;
  std::vector<std::string> template_args_;

  std::pair<std::string, const clang::Expr*> reset_signal_;
  std::pair<std::string, const clang::Expr*> reset_edge_;
  bool reset_type_async_;


};
}  // namespace systemc_clang
#endif
