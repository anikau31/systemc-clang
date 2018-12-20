#ifndef _MODEL_H_
#define _MODEL_H_

#include "clang/AST/DeclCXX.h"
#include <string>
#include <vector>
#include <map>
#include "ModuleDecl.h"
#include "EventContainer.h"
#include "SCModules.h"
#include "FindGlobalEvents.h"
#include "FindSimTime.h"
#include "EntryFunctionContainer.h"
#include "FindNetlist.h"
#include "SCuitable/FindGPUMacro.h"
namespace scpar {
  using namespace clang;
  using namespace std;

  class Model {
  public:

    typedef pair < string, ModuleDecl * >modulePairType;
    typedef map < string, ModuleDecl * >moduleMapType;

    typedef pair < string, EventContainer * >eventPairType;
    typedef map < string, EventContainer * >eventMapType;

    typedef pair<ModuleDecl*, vector<ModuleDecl*> >moduleInstancePairType;
    typedef map<ModuleDecl*, vector<ModuleDecl*> >moduleInstanceMapType;

    typedef pair<EntryFunctionContainer*, FindGPUMacro::forStmtGPUMacroMapType> entryFunctionGPUMacroPairType;
    typedef map<EntryFunctionContainer*, FindGPUMacro::forStmtGPUMacroMapType> entryFunctionGPUMacroMapType;

    Model();
    ~Model();

    void addSCModules(SCModules *);
    void addModuleDecl(ModuleDecl *);
    void addModuleDeclInstances(ModuleDecl*, vector<ModuleDecl*>);
    void addSimulationTime(FindSimTime::simulationTimeMapType);
    void addGlobalEvents(FindGlobalEvents::globalEventMapType);
    void addNetlist(FindNetlist &);
    void addSCMain(FunctionDecl*);
    void addEntryFunctionGPUMacroMap(entryFunctionGPUMacroMapType);
    void updateModuleDecl();

    moduleMapType getModuleDecl();
    entryFunctionGPUMacroMapType getEntryFunctionGPUMacroMap();
    eventMapType getEventMapType();
    moduleInstanceMapType getModuleInstanceMap();
    unsigned int getNumEvents();
    vector<Transition*> getGSauto();
    void dump(raw_ostream &);

  private:
    Model(const Model &);

  protected:
    moduleMapType _modules;
    moduleInstanceMapType _moduleInstanceMap;
    FindSimTime::simulationTimeMapType _simTime;
    eventMapType _eventMap;
    FunctionDecl *_scmainFcDecl;
    FindNetlist::instanceModuleMapType _instanceModuleMap;
    FindNetlist::portSignalMapType _portSignalMap;
    FindNetlist::instancePortSignalMapType _instancePortSignalMap;
    FindNetlist::instanceListModuleMapType _instanceListModuleMap; 
    entryFunctionGPUMacroMapType _entryFunctionGPUMacroMap;
	};
}
#endif
