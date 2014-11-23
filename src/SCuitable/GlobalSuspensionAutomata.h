#ifndef _GLOBAL_SUSPENSION_AUTOMATA_H_
#define _GLOBAL_SUSPENSION_AUTOMATA_H_

#include "Model.h"
#include "FindGPUMacro.h"
namespace scpar {
 using namespace clang;
 using namespace std;

 class GlobalSuspensionAutomata {
  public:
    GlobalSuspensionAutomata (Model *, raw_ostream &, ASTContext *);
    ~GlobalSuspensionAutomata();

    typedef pair<float, int> timePairType;
    typedef vector<Transition*> transitionVectorType;

    typedef pair <string, string> instanceEntryFunctionPairType;
    
    typedef pair <instanceEntryFunctionPairType, transitionVectorType> instanceFunctionSautoPairType;
    typedef map <instanceEntryFunctionPairType, transitionVectorType> instanceFunctionSautoMapType;
    
    typedef pair <Transition *, timePairType> transitionTimePairType;
    typedef map <Transition*, timePairType> transitionTimeMapType;
    
    typedef pair<string, timePairType> eventNotificationTimePairType;
    typedef map <string, timePairType> eventNotificationTimeMapType;

    typedef pair<State*, vector<Transition*> > stateTransitionsPairType;
    typedef map<State*, vector<Transition*> > stateTransitionsMapType; 
   
    typedef pair<Transition*, bool> visitTransitionPairType;
    typedef map<Transition*, bool> visitTransitionMapType;
 
    typedef pair<EntryFunctionContainer*, FindGPUMacro::forStmtGPUMacroMapType> entryFunctionMacroPairType;
    typedef map<EntryFunctionContainer*, FindGPUMacro::forStmtGPUMacroMapType> entryFunctionMacroMapType;
	
    typedef pair<SusCFG*, GPUMacro*> susCFGBlockGPUMacroPairType;
    typedef map<SusCFG*, GPUMacro*> susCFGBlockGPUMacroMapType;
    
    typedef pair<timePairType, vector<SusCFG*> > commonTimeDPPairType;
    typedef map<timePairType, vector<SusCFG*> > commonTimeDPMapType;

    bool updateTransitionTime(Transition*);
    void updateEventNotificationTime(Transition*);    
    void getTransportType();
    void annotateTransitionsDPSeg(Transition*);
    void initializeGpuMap();
    bool GPUMap(float, vector<SusCFG*>, float&);
		vector<SusCFG*> arrangeGPUSusCFGBlocks(vector<SusCFG*>);
		float maxTime(vector<SusCFG*>, unsigned int);
		int max(int, int);
		vector<SusCFG*> merge_sort(vector<SusCFG*>, vector<SusCFG*>);
    void initialise();
    string getArgumentName(Expr *);
    bool isNotifyCall(const CFGStmt*);
    void genGSauto(); 
    string getNotifyEventName(const CFGStmt *);
    void dump();
    

    transitionVectorType getGlobalSauto();
    transitionVectorType getTransitionsAtTime(timePairType);
    transitionVectorType getOutgoingTransitions(State*);
    transitionVectorType getIncomingTransitions(State*);
    timePairType getTimeForTransition(Transition*);    
    instanceFunctionSautoMapType getInstanceFunctionSautoMap();
    transitionTimeMapType getTransitionTimeMap();
    susCFGBlockGPUMacroMapType getSusCFGBlockGPUMacroMap();

  private: 
    raw_ostream &_os;
    ASTContext *_a;
    transitionVectorType _globalSauto;
    Model * _systemcModel;    
    Model::moduleMapType _moduleMap;    
    stateTransitionsMapType _incomingTransitionsMap;
    stateTransitionsMapType _outgoingTransitionsMap;
    instanceFunctionSautoMapType _instanceFunctionSautoMap;
    eventNotificationTimeMapType _eventNotificationTimeMap;
    transitionTimeMapType _transitionTimeMap;
    FindNotify::notifyCallListType _notifyCalls;
    visitTransitionMapType _visitTransitionMap;     
    entryFunctionMacroMapType _entryFunctionGPUMacroMap;
    susCFGBlockGPUMacroMapType _susCFGBlockGPUMacroMap;
    commonTimeDPMapType _commonTimeDPMap;

 };
}
#endif
