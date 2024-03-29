#include "EntryFunctionContainer.h"
#include "NotifyContainer.h"

using namespace systemc_clang;

EntryFunctionContainer::~EntryFunctionContainer() {
  // DO NOT free anything.
}

EntryFunctionContainer::EntryFunctionContainer()
    : entry_name_("NONE"),
      process_type_(PROCESS_TYPE::NONE),
      entry_method_decl_(nullptr) {}

EntryFunctionContainer::EntryFunctionContainer(string n, PROCESS_TYPE p,
                                               clang::CXXMethodDecl *d, clang::Stmt *s)
    : entry_name_(n), process_type_(p), entry_method_decl_(d) {}

EntryFunctionContainer::EntryFunctionContainer(
    const EntryFunctionContainer &from) {
  entry_name_ = from.entry_name_;
  process_type_ = from.process_type_;
  entry_method_decl_ = from.entry_method_decl_;

  reset_type_async_ = from.reset_type_async_;
  reset_edge_ = from.reset_edge_;
  reset_signal_ = from.reset_signal_;

}

///////////////////////////////////////////////////////////////////////////////////////////////

void EntryFunctionContainer::addResetSignal(
    std::pair<std::string, const clang::Expr *> reset_signal) {
  reset_signal_ = reset_signal;
}

void EntryFunctionContainer::addResetEdge(
    std::pair<std::string, const clang::Expr *> reset_edge) {
  reset_edge_ = reset_edge;
}

void EntryFunctionContainer::addResetType(bool reset_type) {
    reset_type_async_ = reset_type;
}


const std::pair<std::string, const clang::Expr *> EntryFunctionContainer::getResetEdge()
    const {
  return reset_edge_;
}

const std::pair<std::string, const clang::Expr *>
EntryFunctionContainer::getResetSignal() const {
  return reset_signal_;
}

bool EntryFunctionContainer::isResetAsync() const { return reset_type_async_; }

string EntryFunctionContainer::getName() { return entry_name_; }

EntryFunctionContainer::SenseMapType EntryFunctionContainer::getSenseMap() {
  return senseMap_;
}

clang::CXXMethodDecl *EntryFunctionContainer::getEntryMethod() {
  return entry_method_decl_;
}

EntryFunctionContainer::waitContainerListType
EntryFunctionContainer::getWaitCalls() {
  return _waitCalls;
}

EntryFunctionContainer::notifyContainerListType
EntryFunctionContainer::getNotifyCalls() {
  return _notifyCalls;
}

PROCESS_TYPE EntryFunctionContainer::getProcessType() { return process_type_; }

SuspensionAutomata::susCFGVectorType EntryFunctionContainer::getSusCFG() {
  return _susCFG;
}

SuspensionAutomata::transitionVectorType EntryFunctionContainer::getSusAuto() {
  return _susAuto;
}
///////////////////////////////////////////////////////////////////////////////////////////////
void EntryFunctionContainer::setName(string n) { entry_name_ = n; }

void EntryFunctionContainer::setProcessType(PROCESS_TYPE p) { process_type_ = p; }

void EntryFunctionContainer::setEntryMethod(clang::CXXMethodDecl *d) {
  entry_method_decl_ = d;
}

void EntryFunctionContainer::addSensitivityInfo(SenseMapType &sm) {
  senseMap_ = sm;
}

void EntryFunctionContainer::addWaits(FindWait &f) {
  FindWait::waitListType wcalls = f.getWaitCalls();
  for (FindWait::waitListType::iterator it = wcalls.begin(), eit = wcalls.end();
       it != eit; it++) {
    // 'it' points to CXXMembercallExpr type.
    WaitContainer *wc{
        new WaitContainer(f.getEntryMethod(), (*it)->getASTNode())};
    _waitCalls.push_back(wc);
  }
  //_processWaitEventMap = f.getProcessWaitEventMap();
}

void EntryFunctionContainer::addSusCFGAuto(SuspensionAutomata &s) {
#ifdef USE_SAUTO
  _susCFG = s.getSusCFG();
  _susAuto = s.getSauto();

  /*
  if (_instanceSusCFGMap.find(numInstance) == _instanceSusCFGMap.end() &&
  _instanceSautoMap.find(numInstance) == _instanceSautoMap.end()) {

                 _instanceSusCFGMap.insert(instanceSusCFGPairType(numInstance,
  s.getSusCFG())); _instanceSautoMap.insert(instanceSautoPairType(numInstance,
  s.getSauto()));
  }
  */
#endif
}

void EntryFunctionContainer::addNotifys(FindNotify &f) {
  FindNotify::NotifyCallListType ncalls = f.getNotifyCallList();
  for (FindNotify::NotifyCallListType::iterator it = ncalls.begin(),
                                                eit = ncalls.end();
       it != eit; it++) {
    // 'it' points to CXXMembercallExpr type.
    NotifyContainer *nc = new NotifyContainer(f.getEntryMethod(), *it);
    _notifyCalls.push_back(nc);
  }
  //_processWaitEventMap = f.getProcessWaitEventMap();
}

///////////////////////////////////////////////////////////////////////////////////////////////
void EntryFunctionContainer::dumpSusCFG(raw_ostream &os) {
  os << "\n#############################################";
#ifdef USE_SAUTO
  SuspensionAutomata::susCFGVectorType susCFGVector = _susCFG;

  for (unsigned int i = 0; i < susCFGVector.size(); i++) {
    os << "\n Block ID : " << susCFGVector.at(i)->getBlockID();
    os << "\n Is Wait Block : " << susCFGVector.at(i)->isWaitBlock();
    if (susCFGVector.at(i)->getParentBlockID()) {
      os << "\n Parent ID : " << susCFGVector.at(i)->getParentBlockID();
      SusCFG *parentBlock = susCFGVector.at(i)->getParentSusCFGBlock();

      os << "\n Parent Block ID : " << parentBlock->getBlockID();
      os << "\n Size of Children : " << parentBlock->getChildBlockList().size();
    }
    vector<SusCFG *> predBlocks = susCFGVector.at(i)->getPredBlocks();
    vector<SusCFG *> succBlocks = susCFGVector.at(i)->getSuccBlocks();
    os << "\n Predecessor Blocks : ";
    for (unsigned int j = 0; j < predBlocks.size(); j++) {
      if (predBlocks.at(j)) {
        os << predBlocks.at(j)->getBlockID() << " ";
      }
    }
    os << "\n Successor Blocks : ";
    for (unsigned int j = 0; j < succBlocks.size(); j++) {
      if (succBlocks.at(j)) {
        os << succBlocks.at(j)->getBlockID() << " ";
      }
    }
  }
#endif
}

void EntryFunctionContainer::dumpSauto(raw_ostream &os) {
#ifdef USE_SAUTO
  vector<Transition *> transitionVector = _susAuto;
  os << "\n Size of transitionVector : " << transitionVector.size();
  for (unsigned int i = 0; i < transitionVector.size(); i++) {
    Transition *t = transitionVector.at(i);
    t->dump(os);
  }
#endif
}

void EntryFunctionContainer::dump(llvm::raw_ostream &os = llvm::dbgs()) {
  os << "\n";

  os << "EntryFunctionContainer '" << getName() << "' processType '";
  switch (getProcessType()) {
    case PROCESS_TYPE::THREAD:
      os << "SC_THREAD' ";
      break;
    case PROCESS_TYPE::METHOD:
      os << "SC_METHOD' ";
      break;
    case PROCESS_TYPE::CTHREAD:
      os << "SC_CTHREAD' ";
      break;
    default:
      os << "NONE' ";
      break;
  }

  // Print the sensitivity map.
  // for (auto const &sense : _senseMap) {
    // os << "sensitivity_signal: " << sense.first << ", "
       // << "edge: " << get<0>(sense.second)
       // << ", MemeberExpr*: " << get<1>(sense.second) << "\n";
  // }
//
  os << " CXXMethodDecl '" << getEntryMethod() << "\n";
  int newTabn = 0;

  os << " Wait Calls \n";
  for (waitContainerListType::iterator it = _waitCalls.begin(),
                                       eit = _waitCalls.end();
       it != eit; it++) {
    (*it)->dump(os, newTabn);
  }
  os << " Notify Calls \n";
  for (notifyContainerListType::iterator it = _notifyCalls.begin(),
                                         eit = _notifyCalls.end();
       it != eit; it++) {
    (*it)->dump(os, newTabn);
  }

  os << "\nReset signals\n";
  os << "reset_signal " << reset_signal_.first << "\n";
  os << "reset_edge   " << reset_edge_.first << "\n";
  os << "reset_type_async " << reset_type_async_ << "\n";

  // os << "\n Suspension CFG";
  // os << "\n ###############";
  // dumpSusCFG(os);
  // os << "\n ###############";
  // os << "\n Suspension Automaton";
  // os << "\n @@@@@@@@@@@@@@@@";
  // dumpSauto(os);
  // os << "\n @@@@@@@@@@@@@@@@";
}
