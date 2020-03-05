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

#include "SystemCClang.h"
#include <clang/Tooling/Tooling.h>

using namespace scpar;

// We need to find a way to pass the top-level module as an argument. 
//
// Source: 
// https://github.com/facebook/facebook-clang-plugins/blob/master/libtooling/ast_exporter_bin.cpp
//
template <typename PluginConsumer>
class SimpleFrontendActionFactory : public clang::tooling::FrontendActionFactory {
  public:
    explicit SimpleFrontendActionFactory( std::string topModule ): top_{topModule} {};
    clang::FrontendAction *create() override { 
      return new PluginConsumer(top_); }

  private:
    std::string top_;
};

template <typename A> class PluginAction {
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
      factory.reset( new SimpleFrontendActionFactory< LightsCameraAction<A>>( topModule ));
      Tool.run(factory.get());

   };
};

#endif /* _PLUGIN_ACTION_H_ */
