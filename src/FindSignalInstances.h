#ifndef _FIND_SIGNAL_INSTANCES_H_
#define _FIND_SIGNAL_INSTANCES_H_

#include <map>
#include <string>
#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "FindTemplateTypes.h"
#include "SignalInstanceContainer.h"
#include "enums.h"

namespace scpar {
  using namespace clang;
  using namespace std;
  
  class FindSignalInstances: public RecursiveASTVisitor<FindSignalInstances> {
  public:
    /// typedefs
    typedef map<string, SignalInstanceContainer* > signalInstanceMapType;
    typedef pair<string, SignalInstanceContainer* > signalInstancePairType;
    
    FindSignalInstances(FunctionDecl*);

    FindSignalInstances(const FindSignalInstances& );
    ~FindSignalInstances();

    virtual bool VisitDeclStmt(DeclStmt* );
    virtual bool VisitCXXConstructExpr(CXXConstructExpr* );

    /// Access methods
    
    signalInstanceMapType getSignalInstances();
    
    /// Print methods
    void dump(raw_ostream& os, int tabn = 0);

  private:
    ASTSTATE _state;
    
    string _tempName;
    signalInstanceMapType _signalInstances;
    

  };
}
#endif
