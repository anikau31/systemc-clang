#ifndef _FIND_METHOD_LOCAL_VARIABLES_H_
#define _FIND_METHOD_LOCAL_VARIABLES_H_

#include <map>
#include <string>
#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "FindTemplateTypes.h"
//#include "ReflectionContainerClass.h"

namespace scpar {
  using namespace clang;
  using namespace std;
  
  class VariableTypeInfo {
  public:
    VariableTypeInfo(llvm::raw_ostream& os);
    VariableTypeInfo(const VariableTypeInfo& );
    ~VariableTypeInfo();
    
    bool isArrayType();
    void print();
    
  public:
    Expr* _expr;
    string _type;
    string _name;
    FindTemplateTypes _tempArgs;
    
    /// Array information.
    llvm::APInt _arraySize;
    bool _isArray;
    llvm::raw_ostream &_os;
  };
  
  class FindMethodLocalVariables: public RecursiveASTVisitor<FindMethodLocalVariables> {
  public:
    /// typedefs
    //n    typedef map<string, Expr*> classMemberMapType;
    //    typedef pair<string, Expr*> kvType;
    typedef map<string, VariableTypeInfo> classMemberMapType;
    typedef pair<string, VariableTypeInfo> kvType;
    
    FindMethodLocalVariables(CXXMethodDecl*, llvm::raw_ostream&); 
    //, ReflectionContainerClass*  );
    
    /// RecursiveASTVisitor methods
    //virtual bool VisitArraySubscriptExpr(ArraySubscriptExpr* );
    virtual bool VisitDeclRefExpr(DeclRefExpr* );
    virtual bool VisitBinaryOperator(BinaryOperator *);
    virtual bool VisitMemberExpr(MemberExpr* );

		/// Access methods
    string getVariableName(Expr* );
    string getDataMembertype(Expr* );
    classMemberMapType getInClassMembers();
    
    /// String return methods.
    map<string,string> getMethodVariables();
    map<string,string> getLHSVariables();
    
    /// Print methods
    void dump();
    
  private:
    enum STATE {
      LHS, RHS
    };
    
    enum RHS_STATE {
      NO, YES
    };
  private:
    llvm::raw_ostream &_os;
    classMemberMapType _inMethodVariables;
    classMemberMapType _lhsVariables;
   	CXXMethodDecl* _d; 
    STATE _state;
    RHS_STATE _rhsState;
  };

}
#endif
