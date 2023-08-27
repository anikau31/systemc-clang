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
#include <llvm/Support/Error.h>
#include "llvm/Support/Debug.h"

#include "SystemCClang.h"

namespace systemc_clang {

using namespace llvm;

static llvm::cl::OptionCategory category("systemc-clang options");
// static llvm::cl::opt<std::string> topModule(
// "top-module",
// llvm::cl::desc("Specify top-level module declaration for entry point"),
// llvm::cl::cat(category));
//
static llvm::cl::opt<bool> debug_mode(
    "debug", llvm::cl::desc("Enable debug output from systemc-clang"),
    llvm::cl::cat(category));

static llvm::cl::opt<std::string> debug_only(
    "debug-only",
    llvm::cl::desc("Enable debug only for the specified DEBUG_TYPE"),
    llvm::cl::cat(category));

class SystemCClangAXN : public ASTFrontendAction {
 public:
  SystemCClangAXN() : top_{""} {};

 private:
  std::string top_;

 public:
  virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(
      clang::CompilerInstance &Compiler, llvm::StringRef inFile) {
    return std::unique_ptr<ASTConsumer>(new SystemCConsumer(Compiler, top_));
  }
};

class PluginAction {
 public:
  PluginAction(int argc, const char **argv) {
    /// Specify the top-level module.
    llvm::Expected<clang::tooling::CommonOptionsParser> options_parser{
        clang::tooling::CommonOptionsParser::create(argc, argv, category)};

    llvm::dbgs() << "Options parser\n";
    if (auto err = options_parser.takeError() ) {
      llvm::logAllUnhandledErrors(std::move(err), llvm::errs(), "[PluginAction Error]");
      return;
    }

    ClangTool Tool(options_parser->getCompilations(),
                   options_parser->getSourcePathList());

    /// Setup the debug mode.
    //
    if (debug_mode || (debug_only != "")) {
      LLVM_DEBUG(llvm::dbgs() << "Debug mode enabled\n";);
      llvm::DebugFlag = true;
    }

    if (debug_only != "") {
#ifdef __clang__
      setCurrentDebugType(debug_only.c_str());
#else
      llvm::setCurrentDebugType(debug_only.c_str());
#endif
    }

    std::unique_ptr<FrontendActionFactory> FrontendFactory;
    FrontendFactory = newFrontendActionFactory<SystemCClangAXN>();

    llvm::dbgs() << "Start SCCL\n";
    Tool.run(FrontendFactory.get());
  };
};
};  // namespace systemc_clang

#endif /* _PLUGIN_ACTION_H_ */
