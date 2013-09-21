#ifndef _FIND_PORT_BINDING_H_
#define _FIND_PORT_BINDING_H_

#include <map>
#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "PortBindContainer.h"
#include "enums.h"

namespace scpar {
  using namespace clang;
  using namespace std;
    
  class FindMemberExpr: public RecursiveASTVisitor<FindMemberExpr> {
  public:
    FindMemberExpr(Expr*, llvm::raw_ostream& );
    
    virtual bool VisitMemberExpr(MemberExpr* );
    string getName();
    string getModuleInstanceName();    
    
  private:
    llvm::raw_ostream& _os;
    string _name;
    string _moduleInstanceName;
  };
  
  class FindDeclRefExpr: public RecursiveASTVisitor<FindDeclRefExpr> {
  public:
    FindDeclRefExpr(Expr*, llvm::raw_ostream&);
    
    virtual bool VisitDeclRefExpr(DeclRefExpr* );
    string getName();
  private:
    llvm::raw_ostream& _os;
    string _name;    
  };
  
  class FindPortBinding: public RecursiveASTVisitor<FindPortBinding> {
  public:
    /// typedefs
    typedef pair<string, PortBindContainer* > portBindPairType;
    typedef multimap<string, PortBindContainer* > portBindMapType;

    FindPortBinding(FunctionDecl*, llvm::raw_ostream&);
    ~FindPortBinding();

    portBindMapType getPortBinds();
    
    virtual bool VisitCXXOperatorCallExpr(CXXOperatorCallExpr*);
    //virtual bool VisitMemberExpr(MemberExpr* );
    void dump(raw_ostream&);

  private:
    llvm::raw_ostream &_os;
    ASTSTATE _state;
    // Key is 
    portBindMapType _portBinds;
  };

}
#endif
