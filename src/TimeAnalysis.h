//===-- src/TimeAnalysis.h - systec-clang class definition -------*- C++ -*-===//
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
#ifndef _TIME_ANALYSIS_H_
#define _TIME_ANALYSIS_H_
#include "SystemCClang.h"

using namespace clang;
using namespace scpar;

class TimeAnalysis:public SystemCConsumer {

public:
	TimeAnalysis (CompilerInstance & ci):
   SystemCConsumer (ci) {

	}
	
	GlobalSuspensionAutomata *_gsauto;

	bool postFire () {
		Model *model = getSystemCModel ();
  
		// Just dump the model traits.
		llvm::errs () << "\n TimeAnalysis plugin is active\n";
		//model->dump(llvm::errs());
  
  	_gsauto = new GlobalSuspensionAutomata(model, _os, &_context);
  	_gsauto->initialise(); 
  	_gsauto->genGSauto();
  	_gsauto->initializeGpuMap();
  	_gsauto->dump();

		return true;
	}

	GlobalSuspensionAutomata* getGSauto(){
		return _gsauto;
	}
};

#endif /* _OoO_ANALYSIS_H_ */
