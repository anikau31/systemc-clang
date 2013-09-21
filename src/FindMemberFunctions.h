#ifndef _MEMBER_FUNCTION_H_
#define _MEMBER_FUNCTION_H_

#include <map>
#include "clang/AST/ExprCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"

namespace scpar {
  using namespace clang;
  using namespace std;
  
  class MemberFunctionType  {
    public:
    MemberFunctionType(); // be smart and forward a child of _reflectionContainer instead of _reflectionContainer
    MemberFunctionType(const MemberFunctionType&);
    string getName();
    CXXMemberCallExpr* getEntryMethod();
    
    string _name;
    CXXMemberCallExpr* _entry;
  };

  class FindMemberFunctions : public RecursiveASTVisitor<FindMemberFunctions>  {
    public:
    FindMemberFunctions(CXXMethodDecl*, llvm::raw_ostream&);
		~FindMemberFunctions();

    virtual bool VisitCXXMemberCallExpr(CXXMemberCallExpr* expr);

    typedef pair <string, MemberFunctionType> functionPairType;
    typedef map <string, MemberFunctionType> functionMapType; 

    typedef pair <string, string> varTypePair;
    typedef map <string, string> varTypeMap;
    // getting the variables for the functions
    
    void dump();
    functionMapType returnFunctionMap();  

 
    private:
    CXXMethodDecl* _memberFunctCall;
    llvm::raw_ostream& _os;
    functionMapType _functionMap;

//    ReflectionContainerClass * _reflectionContainer;
  };
}

#endif
