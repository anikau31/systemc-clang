#include "FindNetlist.h"
using namespace systemc_clang;

FindNetlist::FindNetlist(FunctionDecl *fdecl) : _pass{1} {
  TraverseDecl(fdecl);
  _pass = 2;
}

FindNetlist::~FindNetlist() {
  inst_module_map_.clear();
  port_signal_map_.clear();
  inst_port_signal_map_.clear();
}

FindNetlist::FindNetlist(const FindNetlist &from) {
  _pass = from._pass;
  inst_module_map_ = from.inst_module_map_;
  port_signal_map_ = from.port_signal_map_;
  inst_port_signal_map_ = from.inst_port_signal_map_;
  inst_list_module_map_ = from.inst_list_module_map_;
}

void FindNetlist::updateInstanceListModuleMap(const std::string &instanceName,
                                              const std::string &moduleName) {
  if (inst_list_module_map_.find(moduleName) == inst_list_module_map_.end()) {
    vector<string> instanceList;
    instanceList.push_back(instanceName);
    inst_list_module_map_.insert(
        instanceListModulePairType(moduleName, instanceList));
  } else {
    instanceListModuleMapType::iterator instanceListModuleMapFound =
        inst_list_module_map_.find(moduleName);
    std::vector<std::string> instanceList{instanceListModuleMapFound->second};
    instanceList.push_back(instanceName);
    inst_list_module_map_.erase(moduleName);
    inst_list_module_map_.insert(
        instanceListModulePairType(moduleName, instanceList));
  }
}

bool FindNetlist::VisitCXXOperatorCallExpr(CXXOperatorCallExpr *ce) {
  bool foundME = false;

  std::string moduleName;
  std::string instanceName;
  std::string portName;
  std::string signalName;

  for (Stmt::child_iterator it = ce->IgnoreImpCasts()->child_begin(),
                            eit = ce->IgnoreImpCasts()->child_end();
       it != eit; it++) {
    if (Expr *e = dyn_cast<Expr>(*it)) {
      if (MemberExpr *me = dyn_cast<MemberExpr>(e->IgnoreImpCasts())) {
        if (DeclRefExpr *de =
                dyn_cast<DeclRefExpr>(me->getBase()->IgnoreImpCasts())) {
          moduleName =
              de->getDecl()->getType().getBaseTypeIdentifier()->getName();
          instanceName = de->getFoundDecl()->getNameAsString();
        }
        portName = me->getMemberDecl()->getNameAsString();
        foundME = true;
      }
      if (DeclRefExpr *de = dyn_cast<DeclRefExpr>(e->IgnoreImpCasts())) {
        if (foundME) {
          signalName = de->getNameInfo().getAsString();
          foundME = false;
        }
      }
    }
  }

  if (inst_module_map_.find(instanceName) == inst_module_map_.end()) {
    inst_module_map_.insert(instanceModulePairType(instanceName, moduleName));
    port_signal_map_.clear();
    updateInstanceListModuleMap(instanceName, moduleName);
  }

  port_signal_map_.insert(portSignalPairType(portName, signalName));

  if (inst_port_signal_map_.find(instanceName) ==
      inst_port_signal_map_.end()) {
    inst_port_signal_map_.insert(
        instancePortSignalPairType(instanceName, port_signal_map_));
  } else {
    inst_port_signal_map_.erase(instanceName);
    inst_port_signal_map_.insert(
        instancePortSignalPairType(instanceName, port_signal_map_));
  }

  return true;
}

int FindNetlist::getNumInstances(string moduleName) {
  int counter = 0;
  for (instanceModuleMapType::iterator it = inst_module_map_.begin(),
                                       eit = inst_module_map_.end();
       it != eit; it++) {
    string modName = it->second;
    if (modName == moduleName) {
      counter++;
    }
  }
  return counter;
}

const FindNetlist::portSignalMapType &FindNetlist::getPortSignalMap() {
  return port_signal_map_;
}

const FindNetlist::instanceListModuleMapType &
FindNetlist::getInstanceListModuleMap() {
  return inst_list_module_map_;
}

const FindNetlist::instanceModuleMapType &FindNetlist::getInstanceModuleMap() {
  return inst_module_map_;
}

const FindNetlist::instancePortSignalMapType &
FindNetlist::getInstancePortSignalMap() {
  return inst_port_signal_map_;
}

void FindNetlist::dump() {
  llvm::errs() << "\n ----------------- Netlist dump ----------------- \n";

  for (instanceModuleMapType::iterator it = inst_module_map_.begin(),
                                       eit = inst_module_map_.end();
       it != eit; it++) {
    llvm::errs() << "\n Instance Name : " << it->first
                 << " Module Name : " << it->second;
    string instanceName = it->first;

    if (inst_port_signal_map_.find(instanceName) !=
        inst_port_signal_map_.end()) {
      instancePortSignalMapType::iterator instancePortSignalMapFound =
          inst_port_signal_map_.find(instanceName);
      portSignalMapType portSignalMap = instancePortSignalMapFound->second;
      for (portSignalMapType::iterator pit = portSignalMap.begin(),
                                       pite = portSignalMap.end();
           pit != pite; pit++) {
        llvm::errs() << "\n Port : " << pit->first << " ------> "
                     << pit->second;
      }
    } else {
      llvm::errs() << "\n No instance name found ////// weird.";
    }
    llvm::errs()
        << "\n ------------------------------------------------------\n";
  }
}
