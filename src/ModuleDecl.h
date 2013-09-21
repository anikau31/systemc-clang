#ifndef _MODULE_DECL_H_
#define _MODULE_DECL_H_

#include "clang/AST/DeclCXX.h"
#include <string>
#include <map>

#include "PortDecl.h"
#include "ProcessDecl.h"
#include "Signal.h"
#include "EventDecl.h"
#include "ClassDataMembers.h"
#include "FindClassDataMembers.h"
#include "FindPorts.h"
#include "FindEntryFunctions.h"
#include "FindEvents.h"
#include "FindMemberFunctions.h"
#include "FindSocketBinding.h"
#include "ClassMemberFunctions.h"
#include "Utility.h"
#include "WaitCalls.h"
#include "NotifyCalls.h"
#include "Sockets.h"
namespace scpar {
  using namespace clang;
  using namespace std;
  
  class ModuleDecl {
  public:

    // Types.
    // Signal typedefs.
    typedef pair<string, EventDecl*> eventPairType;
    typedef map<string, EventDecl*> eventMapType;
    
    // Signal typedefs.
    typedef pair<string, Signal*> signalPairType;
    typedef map<string, Signal*> signalMapType;
    
    // Port typedefs.
    typedef pair<string, PortDecl*> portPairType;
    typedef map<string, PortDecl*> portMapType;
    
    // ProcessDecl types.
    typedef pair<string, ProcessDecl*> processPairType;
    typedef map<string, ProcessDecl*> processMapType;

    // Class members
    typedef pair<string, ClassDataMembers*> dataMemberPairType;
    typedef map <string, ClassDataMembers*> dataMemberMapType;

    // Class member functions
    typedef pair<string, ClassMemberFunctions*> classMemberFunctionsPairType;
    typedef map<string, ClassMemberFunctions*> classMemberFunctionsMapType;
    
		typedef pair <string, string> moduleProcessPairType;

		// Wait Calls
		typedef pair <moduleProcessPairType, WaitCalls*> processWaitCallsPairType;
		typedef map <moduleProcessPairType, WaitCalls*> processWaitCallsMapType;

		// Notify Calls
		typedef pair <moduleProcessPairType, NotifyCalls*> processNotifyCallsPairType;
		typedef map <moduleProcessPairType, NotifyCalls*> processNotifyCallsMapType; 

		// Sockets
		typedef pair <string, Sockets*> socketsPairType;
		typedef map <string, Sockets*> socketsMapType;

  public:
    // Constructors.
    ModuleDecl();
    ModuleDecl(const string&, CXXRecordDecl* );

    // Destructor.
    ~ModuleDecl();
    
    // Set parameters
    void addEvents(FindEvents::classEventMapType );    
    void addSignals(FindSignals::signalMapType*);    
    void addInputPorts(FindPorts::portType);
    void addOutputPorts(FindPorts::portType);
    void addInputOutputPorts(FindPorts::portType);        
    void addProcess(FindEntryFunctions::entryFunctionVectorType*);
    void setModuleName(const string &);    
    
		void addDataMembers(const string&, FindClassDataMembers::classMemberMapType, 
		FindClassDataMembers::classMemberSizeMapType, raw_ostream&);

		void addSockets(FindClassDataMembers::socketCharacteristicsMapType);

    void addMemberFunctions(FindMemberFunctions::functionMapType);
		void addWaitCalls(const string&, const string&, FindWait::waitListType, FindWait::waitTimesMapType, FindWait::processWaitEventMapType);		
		void addNotifyCalls(const string&, const string&, FindNotify::notifyCallListType, FindNotify::processNotifyEventMapType);
		void addInitiatorTargetSocketBind(FindSocketBinding::initiatorTargetSocketMapType);	

    /// Get parameters
    string getName();
    CXXRecordDecl* getModuleClassDecl();
    bool isModuleClassDeclNull();
    portMapType getOPorts();     
    processWaitCallsMapType getProcessWaitCallsMap();
		dataMemberMapType getDataMemberMap();
		processMapType getProcessMap();
		eventMapType getEventMap();
		processNotifyCallsMapType getProcessNotifyCallsMap();
		socketsMapType getSocketsMap();
		FindSocketBinding::initiatorTargetSocketMapType getInitiatorTargetSocketMap();

    void dumpPorts(raw_ostream&, int );
    void dumpProcesses(raw_ostream&, int );
    void dumpSignals(raw_ostream&, int );
    void dumpEvents(raw_ostream&, int );
    void dumpClassMembers(raw_ostream&, int);
		void dumpSockets(raw_ostream&, int);
    void dumpMemberFunctions(raw_ostream&, int);
		void dumpWaitCalls(raw_ostream&, int);
		void dumpNotifyCalls(raw_ostream&, int);
    void dumpBindMap(raw_ostream&, int);
		void dump(raw_ostream&);

		    
  private:
    string _moduleName;
    CXXRecordDecl* _classdecl;

    // ProcessDecls in the module.
    processMapType _processes;
    
    // Ports
    portMapType _iports;
    portMapType _oports;
    portMapType _ioports;
    
    // Signals
    signalMapType _signals;

    // sc_event members.
    eventMapType _events;
    
    // Data members
    dataMemberMapType _dataMembers; 

    // Member functions. 
    classMemberFunctionsMapType _memberFunctions;

		// Wait Calls
		processWaitCallsMapType _processWaitCall;

		// Notify Calls
		processNotifyCallsMapType _processNotifyCall;

		// Sockets
		socketsMapType _socketsMap;

		FindSocketBinding::initiatorTargetSocketMapType _initiatorTargetSocketMap;

  };
}
#endif
