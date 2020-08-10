#ifndef _XLAT_H_
#define _XLAT_H_

#include "SystemCClang.h"
//#include "PluginAction.h"

#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Debug.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/FileSystem.h"
//#include "XlatEntryMethod.h"
#include "hNode.h"
#include "XlatEntryMethod.h"
#include "XlatType.h"

using namespace clang;
using namespace systemc_clang;
using namespace hnode;
using namespace llvm;
//static llvm::cl::OptionCategory xlat_category("systemc-clang options");


class Xlat : public SystemCConsumer {

  public:
  Xlat( CompilerInstance& ci, std::string topModule = "!none" )
    : SystemCConsumer( ci, topModule ) {
    }
  Xlat ( ASTUnit *from_ast, std::string topModule = "!none" )
    : SystemCConsumer(from_ast,topModule) {
  }

  bool postFire();
  void xlatmodule(ModuleDecl *mod, hNodep &h_module, llvm::raw_fd_ostream &xlatout );
  void xlatport(ModuleDecl::portMapType pmap, hNode::hdlopsEnum h_op, hNodep &h_info);
  void xlatsig(ModuleDecl::signalMapType pmap, hNode::hdlopsEnum h_op, hNodep &h_info);
  void xlatproc(ModuleDecl::processMapType pm, hNodep & h_top, llvm::raw_ostream &os );
  void xlatportbindings(systemc_clang::ModuleDecl::portBindingMapType portbindingmap, hNodep &h_pb);
  private:

    hNodep h_top;
    std::unordered_map<string, CXXMethodDecl *> allmethodecls;  //  all methods called
    
    XlatType xlatt;

    hname_map_t mod_name_map;
    name_serve mod_newn{"_sc_module_"};
};


// static llvm::cl::opt<std::string> xlat_top(
    // "top",
    // llvm::cl::desc("Specify top-level module declaration for entry point"),
    // llvm::cl::cat(xlat_category));
//
//

class XlatAXN : public ASTFrontendAction {
 public:
  XlatAXN() : top_{} {};

 private:
  std::string top_;

 public:
  virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(
      clang::CompilerInstance &Compiler, llvm::StringRef inFile) {
    return std::unique_ptr<ASTConsumer>(new Xlat(Compiler, top_));
  }
};

class XlatPluginAction {
 public:
  XlatPluginAction(int argc, const char **argv) {
    // Specify the top-level module.
    //
    //
    //
llvm::cl::OptionCategory xlat_category("xlat options");
llvm::cl::opt<std::string> topModule(
    "top-module",
    llvm::cl::desc("Specify top-level module declaration for entry point"),
    llvm::cl::cat(xlat_category));

llvm::cl::opt<bool> debug_mode(
    "debug",
    llvm::cl::desc("Enable debug output from systemc-clang"),
    llvm::cl::cat(xlat_category));

llvm::cl::opt<std::string> debug_only(
    "debug-only",
    llvm::cl::desc("Enable debug only for the specified DEBUG_TYPE"),
    llvm::cl::cat(xlat_category));

    CommonOptionsParser OptionsParser(argc, argv, xlat_category);
    ClangTool Tool(OptionsParser.getCompilations(),
                   OptionsParser.getSourcePathList());

    /// Setup the debug mode.
    //
    if (debug_mode || (debug_only != "") ) {
      LLVM_DEBUG(llvm::dbgs() << "Debug mode enabled\n";);
      llvm::DebugFlag = true;
    }

    if (debug_only != "") {
#ifdef  __clang__
      setCurrentDebugType(debug_only.c_str());
#else
      llvm::setCurrentDebugType(debug_only.c_str());
#endif
    }


    std::unique_ptr<FrontendActionFactory> FrontendFactory;
    FrontendFactory = newFrontendActionFactory<XlatAXN>();
    Tool.run(FrontendFactory.get());
  };
};

#endif
