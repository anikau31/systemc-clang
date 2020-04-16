//===-- src/PluginAction.h - systec-clang class definition -------*- C++
//-*-===//
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

#include <clang/Tooling/Tooling.h>
#include "SystemCClang.h"


namespace scpar {
class PluginAction {
 public:
  PluginAction(int argc, const char **argv) {
    std::unique_ptr<clang::tooling::ToolAction> factory{nullptr};
    static llvm::cl::OptionCategory category("systemc-clang options");
    static llvm::cl::opt<std::string> topModule(
        "top-module",
        llvm::cl::desc("Specify top-level module declaration for entry point"),
        llvm::cl::cat(category));

    // Specify the top-level module.
    CommonOptionsParser OptionsParser(argc, argv, category);
    ClangTool Tool(OptionsParser.getCompilations(),
                   OptionsParser.getSourcePathList());
    runToolOnCode(make_unique<SystemCClangAXN>(topModule), argv[1] );
  };
};
};

#endif /* _PLUGIN_ACTION_H_ */
