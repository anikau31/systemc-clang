// ===-- src/SystemCClang.h - systec-clang class definition -------*- C++
// -*-===//
//
// systemc-clang: SystemC Parser
//
// This file is distributed under the University of Illinois License.
// See LICENSE.mkd for details.
//
// ===----------------------------------------------------------------------===//
// /
// / \file
// / \brief This file contains the class declaration for main SystemC parser.
// /
// ===----------------------------------------------------------------------===//
#ifndef _SYSTEMC_CLANG_H_
#define _SYSTEMC_CLANG_H_

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Parse/Parser.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

using namespace clang::driver;
using namespace clang::tooling;

// / This is the include files we add to parse SystemC
#include "FindArgument.h"
#include "FindConstructor.h"
#include "FindEntryFunctions.h"
#include "FindEvents.h"
#include "FindGlobalEvents.h"
#include "FindModule.h"
#include "FindNetlist.h"
#include "FindNotify.h"
#include "FindPorts.h"
#include "FindSCMain.h"
#include "FindSensitivity.h"
#include "FindSignals.h"
#include "FindSimTime.h"
#include "FindTLMInterfaces.h"
#include "FindWait.h"
#include "Model.h"
#include "FindSCModules.h"
#include "SCuitable/FindGPUMacro.h"
#include "SCuitable/GlobalSuspensionAutomata.h"
#include "SuspensionAutomata.h"
#include "Utility.h"
#include "FindTemplateParameters.h"             \


using namespace clang;

namespace scpar {

class SystemCConsumer : public ASTConsumer,
                        public RecursiveASTVisitor<SystemCConsumer> {
public:
  llvm::raw_ostream &_os;
  SourceManager &_sm;

  SystemCConsumer(CompilerInstance &);
  ~SystemCConsumer();

  // Virtual methods that plugins may override.
  virtual bool preFire();
  virtual bool postFire();
  virtual bool fire();

  Model *getSystemCModel();

  virtual void HandleTranslationUnit(ASTContext &context);
  ASTContext &_context;

private:
  CompilerInstance &_ci;
  // ASTContext& _context;
  Model *_systemcModel;

  // Rewriter _rewrite;
}; // End class SystemCConsumer

class SystemCClang : public SystemCConsumer {
public:
  SystemCClang(CompilerInstance &ci) : SystemCConsumer(ci) {}
};

template <typename A>
class LightsCameraAction : public clang::ASTFrontendAction {
protected:
  virtual std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(CompilerInstance &ci, llvm::StringRef inFile) {
    return std::unique_ptr<clang::ASTConsumer>(new A(ci));
  };
}; // End class LightsCameraAction
} // End namespace scpar

#endif
