//===-- src/PluginAction.h - systec-clang class definition -------*- C++ -*-===//
//
//                     systemc-clang: SystemC Parser
//
// This file is distributed under the TBD
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the class for defining basic plugin actions.
///
//===----------------------------------------------------------------------===//
#ifndef _PLUGIN_ACTION_H_
#define _PLUGIN_ACTION_H_

#include "SystemCClang.h"

using namespace scpar;

template < typename A > class PluginAction
{
public:
	PluginAction (int argc, const char **argv
	) {
		CommonOptionsParser OptionsParser (argc, argv
		);
		ClangTool Tool (OptionsParser.getCompilations (),
										OptionsParser.getSourcePathList ()
		);
		FrontendActionFactory *fe =
			newFrontendActionFactory < LightsCameraAction < A > >();
		Tool.run (fe);

	};


private:

};

#endif /* _PLUGIN_ACTION_H_ */
