#ifndef _XLAT_H_
#define _XLAT_H_

#include "SystemCClang.h"

#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/FileSystem.h"
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
  void xlattype(string prefix, FindTemplateTypes * tt, hNodep & h_typeinfo);
  void xlattype(string prefix, FieldDecl *fieldd, hNodep & h_typeinfo);
  //  void xlatproc(ModuleDecl::processMapType pmap, hNodep & h_top, llvm::raw_ostream &os );
  void xlatproc(vector<EntryFunctionContainer *>efv, hNodep & h_top, llvm::raw_ostream &os );
  void xlatprocesstype(string prefix, Tree<TemplateType> * treep, TreeNode<TemplateType> * nodep, hNodep &h_typeinfo);
  private:

    hNodep h_top;
    std::unordered_map<string, CXXMethodDecl *> allmethodecls;  //  all methods called
    util lutil;
};

#endif
