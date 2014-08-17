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
#include "SuspensionAutomata.h"
#include "FindNetlist.h"
namespace scpar {
  using namespace clang;
  using namespace std;

  class Model {
  public:

    typedef pair < string, ModuleDecl * >modulePairType;
    typedef map < string, ModuleDecl * >moduleMapType;

    typedef pair < string, EventContainer * >eventPairType;
    typedef map < string, EventContainer * >eventMapType;
    

     Model();
    ~Model();

    void addSCModules(SCModules *);
    void addModuleDecl(ModuleDecl *);        
    void addSimulationTime(FindSimTime::simulationTimeMapType);
    void addGlobalEvents(FindGlobalEvents::globalEventMapType);    
    void addNetlist(FindNetlist &);
    void addSCMain(FunctionDecl*);
    void updateModuleDecl();

    moduleMapType getModuleDecl();
    eventMapType getEventMapType();
    unsigned int getNumEvents();

    void dump(raw_ostream &);

  private:
     Model(const Model &);

  protected:
     moduleMapType _modules;
     FindSimTime::simulationTimeMapType _simTime;
     eventMapType _eventMap;     
     FunctionDecl *_scmainFcDecl;
     FindNetlist::instanceModuleMapType _instanceModuleMap;
     FindNetlist::portSignalMapType _portSignalMap;
     FindNetlist::instancePortSignalMapType _instancePortSignalMap;
     FindNetlist::instanceListModuleMapType _instanceListModuleMap;
  };
}
#endif
