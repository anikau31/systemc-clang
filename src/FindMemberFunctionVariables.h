#ifndef _MEMBER_FUNCTION_VARIABLES_H_
#define _MEMBER_FUNCTION_VARIABLES_H_

#include <map>
#include "clang/AST/ExprCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "FindMemberFunctions.h"
#include "clang/AST/DeclCXX.h"
namespace scpar {
  using namespace clang;
  using namespace std;
  class FindMemberFunctionVariables: public RecursiveASTVisitor<FindMemberFunctionVariables> { 
    public:

    FindMemberFunctionVariables(CXXMethodDecl*,  llvm::raw_ostream&);

    ~FindMemberFunctionVariables();
    
    // getting the variables for the functions
    typedef pair <string, string> varTypePair;
    typedef map <string, string> varTypeMap;
    typedef pair<string, vector<string> > functionVarPairType;
    typedef map<string, vector<string> > functionVarMapType;  
    
    virtual bool VisitVarDecl(VarDecl *v);
    functionVarMapType returnFunctionVarMap();  
		void dump(); 
    private:
		CXXMethodDecl *_func;
    llvm::raw_ostream& _os;
    functionVarMapType _functionVarMap;
		varTypeMap _varTypeMap;

//    ReflectionContainerClass * _reflectionContainer;
    };
}


#endif
