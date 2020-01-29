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
  void xlattype(FindTemplateTypes * tt, hNodep & h_typeinfo);
  //  void xlatproc(ModuleDecl::processMapType pmap, hNodep & h_top, llvm::raw_ostream &os );
  void xlatproc(vector<EntryFunctionContainer *>efv, hNodep & h_top, llvm::raw_ostream &os );
  
  private:
    typedef vector < pair < string, const Type *> > tavector_t;
    hNodep h_top;
};

#endif
