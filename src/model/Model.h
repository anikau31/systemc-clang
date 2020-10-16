#ifndef _MODEL_H_
#define _MODEL_H_

#include "EntryFunctionContainer.h"
#include "EventContainer.h"
#include "FindGlobalEvents.h"
#include "FindNetlist.h"
#include "FindSimTime.h"
#include "ModuleDecl.h"
//#include "FindSCModules.h"
#include "SCuitable/FindGPUMacro.h"
#include "clang/AST/DeclCXX.h"
#include <map>
#include <string>
#include <vector>

#include "Matchers.h"

namespace systemc_clang {
using namespace clang;
using namespace std;
using namespace sc_ast_matchers; 

class Model {
public:
  typedef std::pair<std::string, ModuleDecl *> modulePairType;
  typedef std::vector< modulePairType > moduleMapType;

  typedef std::pair<std::string, EventContainer *> eventPairType;
  typedef std::map<std::string, EventContainer *> eventMapType;

  typedef std::pair<ModuleDecl *, std::vector<ModuleDecl *>> moduleInstancePairType;
  typedef std::map<ModuleDecl *, std::vector<ModuleDecl *>> moduleInstanceMapType;

  typedef pair<EntryFunctionContainer *, FindGPUMacro::forStmtGPUMacroMapType>
      entryFunctionGPUMacroPairType;
  typedef map<EntryFunctionContainer *, FindGPUMacro::forStmtGPUMacroMapType>
      entryFunctionGPUMacroMapType;

  Model();
  ~Model();

  // deprecated
  void addModuleDecl(ModuleDecl *);
  void addModuleDeclInstances(ModuleDecl *, vector<ModuleDecl *>);

  void addInstance( ModuleDecl * );
  void addSimulationTime(FindSimTime::simulationTimeMapType);
  void addGlobalEvents(FindGlobalEvents::globalEventMapType);
  void addNetlist(FindNetlist &);
  void addSCMain(FunctionDecl *);
  void addEntryFunctionGPUMacroMap(entryFunctionGPUMacroMapType);
  void updateModuleDecl();

  const moduleMapType & getModuleDecl();
  //ModuleDecl *getModuleDecl(const std::string &decl_name);
  //ModuleDecl *getModuleDeclByInstance(const std::string &inst_name);
  ModuleDecl *getInstance(const std::string &instance_name);
  ModuleDecl *getInstance(Decl *instance_decl);

  entryFunctionGPUMacroMapType getEntryFunctionGPUMacroMap();
  eventMapType getEventMapType();
  std::vector<ModuleDecl*> &getInstances();
  unsigned int getNumEvents();
  std::vector<Transition *> getGSauto();
  void dump(raw_ostream &);

private:
  Model(const Model &);

protected:
  // deprecated
  moduleMapType modules_;
  //moduleInstanceMapType module_instance_map_;

  std::vector<ModuleDecl*> module_instances_;

  FindSimTime::simulationTimeMapType simulation_time_;
  eventMapType event_map_;
  FunctionDecl *scmain_function_decl_;
  FindNetlist::instanceModuleMapType instance_module_map_;
  FindNetlist::portSignalMapType port_signal_map_;
  FindNetlist::instancePortSignalMapType port_signal_instance_map_;
  FindNetlist::instanceListModuleMapType module_instance_list_;
  entryFunctionGPUMacroMapType entry_function_gpu_macro_map_;
};
} // namespace systemc_clang
#endif
