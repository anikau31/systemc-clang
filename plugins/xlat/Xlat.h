#ifndef _XLAT_H_
#define _XLAT_H_

#include "SystemCClang.h"
//#include "PluginAction.h"

#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/FileSystem.h"
//#include "XlatEntryMethod.h"
#include "hNode.h"
#include "XlatEntryMethod.h"

using namespace clang;
using namespace scpar;
using namespace hnode;

class Xlat : public SystemCConsumer {

  public:
  Xlat( CompilerInstance& ci, std::string topModule = "!none" )
    : SystemCConsumer( ci, topModule ) {
    }
  Xlat ( ASTUnit *from_ast, std::string topModule = "!none" )
    : SystemCConsumer(from_ast,topModule) {
  }

  bool postFire();
  void xlatport(ModuleDecl::portMapType pmap, hNode::hdlopsEnum h_op, hNodep &h_info);
  void xlatsig(ModuleDecl::signalMapType pmap, hNode::hdlopsEnum h_op, hNodep &h_info);

  void xlattype(string prefix, Tree<TemplateType> *template_argtp,  hNode::hdlopsEnum h_op, hNodep &h_info);
  void makehpsv(string prefix, string typnae,  hNode::hdlopsEnum h_op, hNodep &h_info, bool needtypeinfo=true);

  void xlatproc(vector<EntryFunctionContainer *>efv, hNodep & h_top, llvm::raw_ostream &os );

  private:

    hNodep h_top;
    std::unordered_map<string, CXXMethodDecl *> allmethodecls;  //  all methods called
    util lutil;
};

 static llvm::cl::OptionCategory xlat_category("xlat-systemc-clang options");
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
    CommonOptionsParser OptionsParser(argc, argv, xlat_category);
    ClangTool Tool(OptionsParser.getCompilations(),
                   OptionsParser.getSourcePathList());

    std::unique_ptr<FrontendActionFactory> FrontendFactory;
    FrontendFactory = newFrontendActionFactory<XlatAXN>();
    Tool.run(FrontendFactory.get());
  };
};

#endif
