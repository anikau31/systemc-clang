#ifndef _FIND_SENSITIVITY_H_
#define _FIND_SENSITIVITY_H_

#include <map>
#include <string>
#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
//#include "ReflectionContainerClass.h"

namespace scpar {
  using namespace clang;
  using namespace std;
  
  class FindSensitivity : public RecursiveASTVisitor<FindSensitivity> {
  public:
    /// Typedefs
    typedef map<string, MemberExpr*> senseMapType;
    typedef pair<string, MemberExpr*> kvType;
    
    FindSensitivity(Stmt* d, llvm::raw_ostream& os);

    virtual bool VisitMemberExpr(MemberExpr* );
    virtual bool VisitCXXOperatorCallExpr(CXXOperatorCallExpr* e);

    void dump();
    
  private:
    llvm::raw_ostream& _os;

    bool _foundSensitiveNode;
    senseMapType _sensitivePorts;

    //ReflectionContainerClass * _reflectionContainer;
  };

}
#endif
