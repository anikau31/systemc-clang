#ifndef _MODEL_H_
#define _MODEL_H_

#include "clang/AST/DeclCXX.h"
#include <string>
#include <vector>
#include <map>

// Headers for the data structure.
#include "ModuleDecl.h"
#include "PortBindContainer.h"
#include "EventContainer.h"

// Headers that do parsing.
#include "SCModules.h"
#include "FindSignalInstances.h"
#include "FindGlobalEvents.h"
#include "NetlistGen.h"
#include "FindSimTime.h"
#include "EntryFunctionContainer.h"
namespace scpar {
  using namespace clang;
  using namespace std;
  
  class Model {
  public:
    // Define types.
    typedef pair<string, ModuleDecl* > modulePairType;
    typedef map<string, ModuleDecl* > moduleMapType;
    
    // Signals
    typedef pair<string, SignalInstanceContainer* > signalInstanceContainerPairType;
    typedef map<string, SignalInstanceContainer* > signalInstanceContainerMapType;

    // Port bindings.
    typedef pair<string, PortBindContainer* > portBindPairType;
    typedef multimap<string, PortBindContainer* > portBindMapType;

    // Global Events.
    typedef pair <string, EventContainer*> eventPairType;
    typedef map <string, EventContainer* >eventMapType;

    // Constructors.
    Model();
    ~Model();

    // Set parameters
    void addSCModules(SCModules* );
    void addModuleDecl(ModuleDecl*);
    void addPortBindings(portBindMapType );
    void addSignalInstances(signalInstanceContainerMapType );
    void addSimulationTime(FindSimTime::simulationTimeMapType);
		void addNetlist(NetlistGen::instanceModuleMapType, NetlistGen::signalPortTypeMapType, NetlistGen::netlistMapType);
    void addGlobalEvents(FindGlobalEvents::globalEventMapType);
		void addEntryFunctionContainerVector(vector<EntryFunctionContainer*>);

		// Access functions
    moduleMapType getModuleDecl();
    NetlistGen::instanceModuleMapType getInstanceModuleMap();
    NetlistGen::signalPortTypeMapType getSignalPortTypeMap();
    NetlistGen::netlistMapType getNetlistMap();
		eventMapType getEventMapType();
		unsigned int getNumEvents();
		vector<EntryFunctionContainer*> getEntryFunctionContainerVector();
		// Dump. 
    void dump(raw_ostream&);
     
  private:
    Model(const Model&);
    
  protected:
    moduleMapType _modules;
    signalInstanceContainerMapType _signals;
    portBindMapType _portBinds;
		NetlistGen::instanceModuleMapType _instanceModuleMap;
		NetlistGen::signalPortTypeMapType _signalPortTypeMap;
		NetlistGen::netlistMapType _netlistMap;
    FindSimTime::simulationTimeMapType _simTime;
    eventMapType _eventMap;
		vector<EntryFunctionContainer*> _entryFunctionContainerVector;
  };
}
#endif
