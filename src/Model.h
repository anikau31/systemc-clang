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

namespace scpar {
using namespace clang;
using namespace std;
using namespace sc_ast_matchers; 

class Model {
public:
  typedef pair<string, ModuleDecl *> modulePairType;
  typedef std::vector< modulePairType > moduleMapType;

  typedef pair<string, EventContainer *> eventPairType;
  typedef map<string, EventContainer *> eventMapType;

  typedef pair<ModuleDecl *, vector<ModuleDecl *>> moduleInstancePairType;
  typedef map<ModuleDecl *, vector<ModuleDecl *>> moduleInstanceMapType;

  typedef pair<EntryFunctionContainer *, FindGPUMacro::forStmtGPUMacroMapType>
      entryFunctionGPUMacroPairType;
  typedef map<EntryFunctionContainer *, FindGPUMacro::forStmtGPUMacroMapType>
      entryFunctionGPUMacroMapType;

  Model();
  ~Model();

  //void addSCModules(FindSCModules *);
  void addModuleDecl(ModuleDecl *);

  void addModuleDeclInstances(ModuleDecl *, vector<ModuleDecl *>);
  void addSimulationTime(FindSimTime::simulationTimeMapType);
  void addGlobalEvents(FindGlobalEvents::globalEventMapType);
  void addNetlist(FindNetlist &);
  void addSCMain(FunctionDecl *);
  void addEntryFunctionGPUMacroMap(entryFunctionGPUMacroMapType);
  void updateModuleDecl();

  moduleMapType getModuleDecl();
  entryFunctionGPUMacroMapType getEntryFunctionGPUMacroMap();
  eventMapType getEventMapType();
  moduleInstanceMapType & getModuleInstanceMap();
  unsigned int getNumEvents();
  vector<Transition *> getGSauto();
  void dump(raw_ostream &);

private:
  Model(const Model &);

protected:
  moduleMapType modules_;
  moduleInstanceMapType module_instance_map_;
  FindSimTime::simulationTimeMapType simulation_time_;
  eventMapType event_map_;
  FunctionDecl *scmain_function_decl_;
  FindNetlist::instanceModuleMapType instance_module_map_;
  FindNetlist::portSignalMapType port_signal_map_;
  FindNetlist::instancePortSignalMapType port_signal_instance_map_;
  FindNetlist::instanceListModuleMapType module_instance_list_;
  entryFunctionGPUMacroMapType entry_function_gpu_macro_map_;
};
} // namespace scpar
#endif
