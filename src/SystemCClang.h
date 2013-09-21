//===-- src/SystemCClang.h - systec-clang class definition -------*- C++ -*-===//
//
//                     systemc-clang: SystemC Parser
//
// This file is distributed under the University of Illinois License.
// See LICENSE.mkd for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the class declaration for main SystemC parser.
///
//===----------------------------------------------------------------------===//
#ifndef _SYSTEMC_CLANG_H_
#define _SYSTEMC_CLANG_H_

#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Parse/Parser.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

using namespace clang::driver;
using namespace clang::tooling;

/// This is the include files we add to parse SystemC
#include "FindModule.h"
#include "FindPorts.h"
#include "FindEvents.h"
#include "FindGlobalEvents.h"
#include "FindEntryFunctions.h"
#include "FindSensitivity.h"
#include "FindSCMain.h"
#include "FindWait.h"
#include "FindNotify.h"
#include "FindClassDataMembers.h"
#include "FindMethodLocalVariables.h"
#include "FindPortReadWrites.h"
#include "SCModules.h"
#include "FindMemberFunctions.h"
#include "FindMemberFunctionVariables.h"
#include "FindSignals.h"
#include "FindSimTime.h"
#include "FindPortBinding.h"
#include "FindModuleInstances.h"
#include "FindSignalInstances.h"
#include "Model.h"
#include "NetlistGen.h"
#include "TaskDependencyGraph.h"
#include "DependencyGraph.h"
#include "FindThreadHierarchy.h"
#include "FindSuspensionWindow.h"
#include "FindPortTracing.h"
#include "FindLoopTime.h"
#include "FindPayloadCharacteristics.h"
#include "FindSocketTransportType.h"
#include "FindSocketRegisterMethods.h"
#include "FindSocketBinding.h"
#include "SocketBindGen.h"
#include "FindArgument.h"
#include "FindVariable.h"
#include "FindSuspensionTime.h"
#include "SplitWaitBlocks.h"

using namespace clang;

namespace scpar {
  
  class SystemCConsumer 
  : public ASTConsumer
  , public RecursiveASTVisitor<SystemCConsumer> {
    
  private:
    llvm::raw_ostream &_os;
    SourceManager &_sm;

    
  public:
    SystemCConsumer(CompilerInstance&);
    ~SystemCConsumer();
    
    // Virtual methods that plugins may override. 
    virtual bool preFire();
    virtual bool postFire();
    virtual bool fire();
    
    Model* getSystemCModel();
    
    virtual void HandleTranslationUnit(ASTContext &context);
  
	protected:
		ASTContext& _context; 
		
  private:
    CompilerInstance& _ci;
//    ASTContext& _context;
    Model* _systemcModel;
//    Rewriter _rewrite;   
  }; // End class SystemCConsumer
  
  template<typename A>
  class LightsCameraAction 
  : public clang::ASTFrontendAction {
  protected:

    virtual ASTConsumer* CreateASTConsumer(CompilerInstance& ci, StringRef) {
      return new A(ci);
    };

  }; // End class LightsCameraAction
  
} // End namespace scpar

#endif
