//===-- src/SCuitable.h - systec-clang class definition -------*- C++ -*-===//
//
//                     systemc-clang: SystemC Parser
//
// This file is distributed under the TBD
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief A short example of how to use the plugin architecture.
///
//===----------------------------------------------------------------------===//
#ifndef _SCUITABLE_H_
#define _SCUITABLE_H_
#include "SystemCClang.h"

using namespace clang;
using namespace scpar;

class SCuitable
: public SystemCConsumer {

public:
  SCuitable( CompilerInstance& ci)
  : SystemCConsumer(ci) {
  
  }
  
  // Virtual methods.
  // void preFire();
  // void fire();
  // void postFire();
  
  bool postFire() {
    Model* model = getSystemCModel();
    
    // Just dump the model traits.
    llvm::errs() << "\n SCuitable plugin is active\n";
    //model->dump(llvm::errs());
		/*
		FindSuspensionWindow findSuspensionWindow(model->getEntryFunctionContainerVector(), &_context, 
		llvm::errs());
		findSuspensionWindow.traverseCFG();
		findSuspensionWindow.dump();

		TaskDependencyGraph tgraph(model->getEntryFunctionContainerVector(), 
		findSuspensionWindow.getWaitVariableMap(), 
		findSuspensionWindow.getNotifyVariableMap() , model->getModuleDecl(), llvm::errs());
		tgraph.genDependencyGraph();
    */
		return true;
  } 
};

#endif /* _SCUITABLE_H_ */
