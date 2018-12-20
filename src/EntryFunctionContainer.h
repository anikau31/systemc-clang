//===-- src/EntryFunctionContainer.h - systec-clang class definition -------*- C++ -*-===//
//
//                     systemc-clang: SystemC Parser
//
// This file is distributed under the University of Illinois License.
// See LICENSE.mkd for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief A container class to hold entry function information.
///
//===----------------------------------------------------------------------===//
#ifndef _ENTRY_FUNCTION_CONTAINER_H_
#define _ENTRY_FUNCTION_CONTAINER_H_

#include <string>
#include <map>
#include "clang/AST/DeclCXX.h"
#include "WaitContainer.h"
#include "NotifyContainer.h"
#include "enums.h"
#include "FindWait.h"
#include "Utility.h"
#include "FindNotify.h"
#include "FindSensitivity.h"
#include "SuspensionAutomata.h"
namespace scpar {

  using namespace clang;
  using namespace std;

  class EntryFunctionContainer {
  public:
    // typedefs
    typedef vector < WaitContainer * >waitContainerListType;
    typedef vector < NotifyContainer *> notifyContainerListType;

    typedef pair<int, SuspensionAutomata::transitionVectorType> instanceSautoPairType;
    typedef map<int, SuspensionAutomata::transitionVectorType> instanceSautoMapType;

    typedef pair <int, SuspensionAutomata::susCFGVectorType> instanceSusCFGPairType;
    typedef map <int, SuspensionAutomata::susCFGVectorType> instanceSusCFGMapType;

    EntryFunctionContainer();
    EntryFunctionContainer(string, PROCESS_TYPE, CXXMethodDecl *, Stmt *);
    EntryFunctionContainer(const EntryFunctionContainer &);
    ~EntryFunctionContainer();

    // Accessors.
    string getName();
    CXXMethodDecl *getEntryMethod();
    //Stmt *getConstructorStmt ();
    PROCESS_TYPE getProcessType();

    waitContainerListType getWaitCalls();
    notifyContainerListType getNotifyCalls();
    FindSensitivity::senseMapType getSenseMap();
    SuspensionAutomata::susCFGVectorType getSusCFG();
    SuspensionAutomata::transitionVectorType getSusAuto();
    instanceSautoMapType getInstanceSautoMap();
    instanceSusCFGMapType getInstanceSusCFGMap();

    // Add waits.
    void addSensitivityInfo(FindSensitivity &);
    void addWaits (FindWait &);
    void addNotifys(FindNotify &);
    void addSusCFGAuto(SuspensionAutomata &);
    //void setConstructorStmt (Stmt *);
    void setName (string);
    void setProcessType (PROCESS_TYPE);
    void setEntryMethod (CXXMethodDecl *);
    void dumpSusCFG(raw_ostream &);
    void dumpSauto(raw_ostream &);
    void dump (raw_ostream &, int);


    //private:
    string _entryName;
    PROCESS_TYPE _procType;
    CXXMethodDecl *_entryMethodDecl;

    // Hold all the waits.
    waitContainerListType _waitCalls;
    notifyContainerListType _notifyCalls;
    FindSensitivity::senseMapType _senseMap;
    vector<Transition*> _susAuto;
    vector<SusCFG*> _susCFG;
  };
}
#endif
