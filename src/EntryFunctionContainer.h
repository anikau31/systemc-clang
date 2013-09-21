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
#include "enums.h"
#include "FindWait.h"
#include "Utility.h"
#include "FindNotify.h"
#include "FindPortTracing.h"
#include "FindMethodLocalVariables.h"
#include "FindThreadHierarchy.h"
#include "FindLoopTime.h"
#include "FindSocketTransportType.h"
#include "FindPayloadCharacteristics.h"
namespace scpar {
  
  using namespace clang;
  using namespace std;
  
  class EntryFunctionContainer {
  public:
    // typedefs
    typedef vector<WaitContainer* > waitContainerListType;
    
    EntryFunctionContainer();
    EntryFunctionContainer(string, PROCESS_TYPE, CXXMethodDecl*, Stmt* );
    EntryFunctionContainer(const EntryFunctionContainer& );
    ~EntryFunctionContainer();
    
    // Accessors.
    string getName();
    CXXMethodDecl* getEntryMethod();
    Stmt* getConstructorStmt();
    PROCESS_TYPE getProcessType();
		FindWait::processWaitEventMapType getEventWaitMap();
		FindNotify::processNotifyEventMapType getEventNotifyMap();
		FindWait::waitTimesMapType getWaitTimesMap();
    waitContainerListType getWaitCalls();
		FindMethodLocalVariables::classMemberMapType getMethodLocalVariables();
		FindWait::dataTimesMapType getDataTimesMap();
		FindWait::waitNestLoopMapType getWaitNestLoopMap();
		FindWait::dataNestLoopMapType getDataNestLoopMap();
		FindThreadHierarchy::forStmtThreadHierarchyMapType getThreadHierarchyMap();
    FindLoopTime::forStmtLoopTimeMapType getLoopTimeMap();
		FindSocketTransportType::socketTransportMapType getSocketTransportMap();
		FindPayloadCharacteristics::payloadCharacteristicsMapType 
		getPayloadCharacteristicsMap();

		// Add waits.
    void addWaits(FindWait& );
		void addNotifys(FindNotify&);   
		void addPortTrace(FindPortTracing&);
    void setConstructorStmt(Stmt* );
    void setName(string );
    void setProcessType(PROCESS_TYPE );
    void setEntryMethod(CXXMethodDecl* ); 
    void dump(raw_ostream& , int );
		void addLocalVariables(FindMethodLocalVariables&);
		void addThreadHierarchy(FindThreadHierarchy&);
    void addLoopTime(FindLoopTime&);
		void addSocketTransportMap(FindSocketTransportType&);
		void addPayloadCharacteristicsMap(FindPayloadCharacteristics&);


  //private:    
    string _entryName;
    PROCESS_TYPE _procType;
    CXXMethodDecl* _entryMethodDecl;
    Stmt* _constructorStmt;
		FindWait::processWaitEventMapType _processWaitEventMap;
		FindNotify::processNotifyEventMapType _processNotifyEventMap;
		FindWait::waitTimesMapType _waitTimesMap;
		FindPortTracing::portTracingMapType _portTraceMap;
		// Hold all the waits.
    waitContainerListType _waitCalls;
		FindWait::dataTimesMapType _dataTimesMap;
		FindWait::waitNestLoopMapType _waitNestLoopMap;
		FindWait::dataNestLoopMapType _dataNestLoopMap;		
		FindMethodLocalVariables::classMemberMapType _inMethodLocalVariables;
		FindThreadHierarchy::forStmtThreadHierarchyMapType _forStmtThreads;
		FindLoopTime::forStmtLoopTimeMapType _forStmtLoopTimeMap;
		FindSocketTransportType::socketTransportMapType _socketTransportMap;
		FindPayloadCharacteristics::payloadCharacteristicsMapType 
		_payloadCharacteristicsMap;
  };
} 

#endif
