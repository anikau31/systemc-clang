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

#include "model/Model.h"
#include "matchers/FindSCMain.h"
#include "matchers/Matchers.h"

//#include "SCuitable/FindGPUMacro.h"
//#include "SCuitable/GlobalSuspensionAutomata.h"
//#include "SuspensionAutomata.h"

namespace systemc_clang {
using namespace clang::tooling;
using namespace sc_ast_matchers;
/** @brief This is the main consumer class that beings the parsing of SystemC.

    This class drives the AST consumer for parsing SystemC constructs.


    */

/// Forward declarations

class SystemCConsumer : public ASTConsumer,
                        public RecursiveASTVisitor<SystemCConsumer> {
  // TODO: This should be made private at some point.
 public:
  llvm::raw_ostream &os_;

 public:
  SystemCConsumer(clang::CompilerInstance &, std::string top = "!none");
  SystemCConsumer(clang::ASTUnit *from_ast, std::string top = "!none");
  virtual ~SystemCConsumer();

  void processModuleDeclaration(clang::CXXRecordDecl *cxx_decl, ModuleInstance *add_module_decl);

  void processNetlist(FindSCMain *scmain, ModuleDeclarationMatcher *module_declaration_handler);

  Model *getSystemCModel();
  const std::string &getTopModule() const;
  void setTopModule(const std::string &top_module_decl);
  clang::ASTContext &getContext() const;
  clang::SourceManager &getSourceManager() const;

  // Virtual methods that plugins may override.
  virtual bool fire();
  virtual bool preFire();
  virtual bool postFire();

  virtual void HandleTranslationUnit(clang::ASTContext &context);

 private:
  std::string top_;
  Model *systemc_model_;
  clang::ASTContext &context_;
  clang::SourceManager &sm_;
};  // End class SystemCConsumer

//
// SystemCClang
//
//

/*!
 *
 * \mainpage systemc-clang is a parser for SystemC constructs built using clang.
 * There is a HDL plugin that generates Verilog.
 *
 * \section Documentation
 *
 * There is a read the docs documentation available at: <a
 * href="https://systemc-clang.readthedocs.io/">https://systemc-clang.readthedocs.io/</a>
 *
 */

class SystemCClang : public SystemCConsumer {
 public:
  SystemCClang(clang::CompilerInstance &ci, const std::string &top)
      : SystemCConsumer(ci, top) {}
};

}  // End namespace systemc_clang
#endif
