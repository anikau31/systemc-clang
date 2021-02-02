//===-- src/EntryFunctionContainer.h - systec-clang class definition -------*-
// C++ -*-===//
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

#include <map>
#include <string>
#include "FindNotify.h"
#include "SensitivityMatcher.h"
#include "FindWait.h"
#include "NotifyContainer.h"
#include "SuspensionAutomata.h"
#include "Utility.h"
#include "WaitContainer.h"
#include "clang/AST/DeclCXX.h"

namespace systemc_clang {

enum class ASTSTATE {
  EMPTY,
  DECLSTMT,
  MEMBEREXPR,
  CXXOPERATORCALLEXPR,
  CXXCONSTRUCTEXPR
};  // End enum ASTSTATE

enum class ReadWrite { RWINIT, READ, WRITE };  // End enum ReadWrite

enum class PROCESS_TYPE {
  NONE,
  THREAD,
  CTHREAD,
  METHOD
};  // End enum PROCESS_TYPE

class EntryFunctionContainer {
 public:
  /// typedefs
  typedef std::vector<WaitContainer *> waitContainerListType;
  typedef std::vector<NotifyContainer *> notifyContainerListType;

  typedef std::pair<int, SuspensionAutomata::transitionVectorType>
      instanceSautoPairType;
  typedef std::map<int, SuspensionAutomata::transitionVectorType>
      instanceSautoMapType;

  typedef std::pair<int, SuspensionAutomata::susCFGVectorType>
      instanceSusCFGPairType;
  typedef std::map<int, SuspensionAutomata::susCFGVectorType>
      instanceSusCFGMapType;

  // Sensitivity information
  typedef std::tuple<std::string, clang::ValueDecl *, clang::MemberExpr *,
                     clang::DeclRefExpr *, clang::ArraySubscriptExpr *>
      SensitivityTupleType;
  typedef std::pair<std::string, std::vector<SensitivityTupleType>>
      SensitivityPairType;

  typedef std::map<std::string, std::vector<SensitivityTupleType>> SenseMapType;

  EntryFunctionContainer();
  EntryFunctionContainer(std::string, PROCESS_TYPE, clang::CXXMethodDecl *,
                         clang::Stmt *);
  EntryFunctionContainer(const EntryFunctionContainer &);

  /// Destructor.
  virtual ~EntryFunctionContainer();

  // Accessors.
  std::string getName();
  clang::CXXMethodDecl *getEntryMethod();
  // Stmt *getConstructorStmt ();
  PROCESS_TYPE getProcessType();

  waitContainerListType getWaitCalls();
  notifyContainerListType getNotifyCalls();
  SenseMapType getSenseMap();
  SuspensionAutomata::susCFGVectorType getSusCFG();
  SuspensionAutomata::transitionVectorType getSusAuto();
  instanceSautoMapType getInstanceSautoMap();
  instanceSusCFGMapType getInstanceSusCFGMap();

  // Add waits.
  // void addSensitivityInfo(FindSensitivity &);
  void addSensitivityInfo(SenseMapType &);
  void addWaits(FindWait &);
  void addNotifys(FindNotify &);
  void addSusCFGAuto(SuspensionAutomata &);
  // void setConstructorStmt (Stmt *);
  void setName(std::string);
  void setProcessType(PROCESS_TYPE);
  void setEntryMethod(clang::CXXMethodDecl *);
  void dumpSusCFG(llvm::raw_ostream &);
  void dumpSauto(llvm::raw_ostream &);
  void dump(llvm::raw_ostream &, int);

  // private:
  std::string entry_name_;
  PROCESS_TYPE process_type_;
  clang::CXXMethodDecl *entry_method_decl_;

  // Hold all the waits.
  waitContainerListType _waitCalls;
  notifyContainerListType _notifyCalls;

  // Sensitivity information
  SenseMapType senseMap_;

  std::vector<Transition *> _susAuto;
  std::vector<SusCFG *> _susCFG;
};
}  // namespace systemc_clang
#endif
