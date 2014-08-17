//===-- src/SAPlugin.h - systec-clang class definition -------*- C++ -*-===//
//
//                     systemc-clang: SystemC Parser
//
// This file is distributed under the University of Illinois License.
// See LICENSE.mkd for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Plugin consumer for generating suspension automata.
///
//===----------------------------------------------------------------------===//
#ifndef _SA_PLUGIN_H_
#define _SA_PLUGIN_H_

#include "SystemCClang.h"

using namespace clang;
using namespace scpar;

class SAPlugin:public SystemCConsumer {

public:
	SAPlugin (CompilerInstance & ci
	):SystemCConsumer (ci
	) {

	}
	// Virtual methods.// void preFire();// void fire();// void postFire();
		bool postFire (
	) {
		llvm::errs () << "\n New plugin for SAPlugin\n";
		Model *model = getSystemCModel ();

		model->dump (llvm::errs ());
		return true;
	}
};

#endif /* _SA_PLUGIN_H_ */
