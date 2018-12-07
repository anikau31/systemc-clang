#ifndef _FIND_GLOBAL_EVENTS_H_
#define _FIND_GLOBAL_EVENTS_H_

#include <map>
#include <vector>
#include <string>
#include "clang/AST/Decl.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"

namespace scpar {
  using namespace clang;
  using namespace std;

  class FindGlobalEvents:public RecursiveASTVisitor < FindGlobalEvents > {
  public:
    
    typedef map < string, VarDecl * > globalEventMapType;
    typedef pair < string, VarDecl * > kvType;

    FindGlobalEvents(TranslationUnitDecl *, llvm::raw_ostream &);
    virtual ~FindGlobalEvents();
    
    virtual bool VisitVarDecl(VarDecl *);
    
    globalEventMapType getEventMap();
    vector < string > getEventNames();

    void dump();

  private:
    llvm::raw_ostream & _os;
    globalEventMapType _globalEvents;
  };
}
#endif
