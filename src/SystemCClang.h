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
#include "clang/AST/ASTImporter.h"
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
//#include "FindPorts.h"
#include "FindSCMain.h"
#include "FindSensitivity.h"
//#include "FindSignals.h"
#include "FindSimTime.h"
#include "FindTLMInterfaces.h"
#include "FindWait.h"
#include "Model.h"
//#include "FindSCModules.h"
#include "SCuitable/FindGPUMacro.h"
#include "SCuitable/GlobalSuspensionAutomata.h"
#include "SuspensionAutomata.h"
#include "Utility.h"
#include "FindTemplateParameters.h"             
#include "FindModuleInstance.h"


using namespace clang;

namespace scpar {

  class SystemCConsumer : public ASTConsumer,
  public RecursiveASTVisitor<SystemCConsumer> {

    // TODO: This should be made private at some point.
    public:
      llvm::raw_ostream& os_;

    public: 
      SystemCConsumer( CompilerInstance &, std::string top = "!none" );
      SystemCConsumer(ASTUnit *from_ast, std::string top = "!none");
      virtual ~SystemCConsumer();

      Model *getSystemCModel();
      const std::string & getTopModule() const;
      void setTopModule( const std::string & top_module_decl );
      ASTContext& getContext() const; 
      SourceManager& getSourceManager() const;

      // Virtual methods that plugins may override.
      virtual bool fire();
      virtual bool preFire();
      virtual bool postFire();

      virtual void HandleTranslationUnit(ASTContext &context);

    private:
      std::string top_;
      Model* systemcModel_;
      ASTContext& context_;
      SourceManager& sm_;
  }; // End class SystemCConsumer

  //
  // SystemCClang
  //
  //

  class SystemCClang : public SystemCConsumer {
    public:
      SystemCClang( CompilerInstance &ci, std::string top ) : SystemCConsumer( ci, top ) {}
  };

  template <typename A>
    class LightsCameraAction : public clang::ASTFrontendAction {
      public:
        LightsCameraAction( std::string topModule ):top_{topModule} { };

      private: 
        std::string top_;
      protected:
        virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer ( CompilerInstance &ci, 
            llvm::StringRef inFile ) {
          return std::unique_ptr<clang::ASTConsumer>( new A(ci, top_) );
        };
    }; 

} // End namespace scpar

#endif
