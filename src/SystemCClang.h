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

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

// using namespace clang::driver;

// / This is the include files we add to parse SystemC
#include "matchers/FindArgument.h"
#include "matchers/FindConstructor.h"
#include "matchers/FindEntryFunctions.h"
#include "matchers/FindEvents.h"
#include "matchers/FindGlobalEvents.h"
#include "matchers/FindModule.h"
#include "matchers/FindModuleInstance.h"
#include "matchers/FindNetlist.h"
#include "matchers/FindNotify.h"
#include "matchers/FindSCMain.h"
//#include "matchers/FindSensitivity.h"
#include "matchers/FindSimTime.h"
#include "matchers/FindTLMInterfaces.h"
#include "matchers/FindTemplateParameters.h"
#include "matchers/FindWait.h"
#include "model/Model.h"
#include "matchers/InstanceMatcher.h"

//#include "SCuitable/FindGPUMacro.h"
//#include "SCuitable/GlobalSuspensionAutomata.h"
//#include "SuspensionAutomata.h"

using namespace clang::tooling;

namespace systemc_clang {
/** @brief This is the main consumer class that beings the parsing of SystemC.

    This class drives the AST consumer for parsing SystemC constructs.

    
    */
class SystemCConsumer : public ASTConsumer,
                        public RecursiveASTVisitor<SystemCConsumer> {
  // TODO: This should be made private at some point.
 public:
  llvm::raw_ostream &os_;

 public:
  SystemCConsumer(CompilerInstance &, std::string top = "!none");
  SystemCConsumer(ASTUnit *from_ast, std::string top = "!none");
  virtual ~SystemCConsumer();

  Model *getSystemCModel();
  const std::string &getTopModule() const;
  void setTopModule(const std::string &top_module_decl);
  ASTContext &getContext() const;
  SourceManager &getSourceManager() const;

  // Virtual methods that plugins may override.
  virtual bool fire();
  virtual bool preFire();
  virtual bool postFire();

  virtual void HandleTranslationUnit(ASTContext &context);

 private:
void populateNestedModules(const InstanceMatcher::InstanceDeclarations &instance_map);

 private:
  std::string top_;
  Model *systemcModel_;
  ASTContext &context_;
  SourceManager &sm_;
};  // End class SystemCConsumer

//
// SystemCClang
//
//

/** 
 *
 * @mainpage 
 * systemc-clang is a parser for SystemC constructs built using clang. 
 *
 *
 */
class SystemCClang : public SystemCConsumer {
 public:
  SystemCClang(CompilerInstance &ci, const std::string &top)
      : SystemCConsumer(ci, top) {}
};


}  // End namespace systemc_clang
#endif
