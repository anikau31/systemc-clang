#ifndef _FIND_MODULE_INSTANCES_H_
#define _FIND_MODULE_INSTANCES_H_

#include <map>
#include <string>
#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/PrettyPrinter.h"
#include "FindTemplateTypes.h"
#include "ModuleInstanceContainer.h"
#include "enums.h"

namespace scpar {
  using namespace clang;
  using namespace std;

   class FindModuleInstances: public RecursiveASTVisitor<FindModuleInstances> {
  public:
    /// typedefs
    typedef map<string, ModuleInstanceContainer* > moduleInstanceContainerMapType;
    typedef pair<string, ModuleInstanceContainer* > moduleInstanceContainerPairType;
    
    FindModuleInstances(FunctionDecl*);

    FindModuleInstances(const FindModuleInstances& );
    ~FindModuleInstances();
     
     bool isState(ASTSTATE);

    virtual bool VisitDeclStmt(DeclStmt* );
     virtual bool VisitCXXConstructExpr(CXXConstructExpr* );

    /// Access methods
    
     //string strip(string, string );
     
    /// Print methods
    void dump(raw_ostream& os, int tabn = 0);

  private:
     ASTSTATE _state;
     
     // This map has the module instance name as the key.
     // This is necessary to keep it unique.
     // k = module name
     // v = container
    moduleInstanceContainerMapType _moduleInstances;
    

  };
}
#endif
