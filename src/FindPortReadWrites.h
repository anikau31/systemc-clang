#ifndef _FIND_PORT_READ_WRITE_H_
#define _FIND_PORT_READ_WRITE_H_

#include <map>
#include <string>
#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/PrettyPrinter.h"
#include "enums.h"

namespace scpar {
  using namespace clang;
  using namespace std;  

  class FindPortReadWrites: public RecursiveASTVisitor<FindPortReadWrites> {
  public:
    /// typedefs
    typedef map<string, Expr*> classMemberMapType;
    typedef pair<string, Expr*> kvType;

    FindPortReadWrites(CXXMethodDecl*, llvm::raw_ostream& );
    
    /// RecursiveASTVisitor methods
    virtual bool VisitMemberExpr(MemberExpr* );

    /// Access methods
    string getVariableName(Expr* );
    string getDataMembertype(Expr* fd);
    classMemberMapType getInClassMembers();

    /// Print methods
    void dump();

  private:
    llvm::raw_ostream &_os;
    classMemberMapType _portReads;
    classMemberMapType _portWrites;        

    int _rw;
    int _state;
    
  };

}
#endif
