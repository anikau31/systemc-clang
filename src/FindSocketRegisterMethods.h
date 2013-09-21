#ifndef _FIND_SOCKET_REGISTER_METHODS_H_
#define _FIND_SOCKET_REGISTER_METHODS_H_

#include <vector>
#include <string>
#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include <map>
namespace scpar {
 
  using namespace clang;
  using namespace std;
  
  class FindSocketRegisterMethods: public RecursiveASTVisitor<FindSocketRegisterMethods> {
  public:
		
		FindSocketRegisterMethods(CXXRecordDecl*, llvm::raw_ostream&);
		~FindSocketRegisterMethods();

		typedef vector<string> callbackMethods;
		/// Virtual methods from RecursiveASTVisitor
		virtual bool VisitCXXMemberCallExpr(CXXMemberCallExpr *ce);
		virtual bool VisitCXXMethodDecl(CXXMethodDecl *md);		
		// Access Methods

  	callbackMethods getCallbackMethods();	

  private:
  
    CXXRecordDecl *_d;
    llvm::raw_ostream &_os;
    Stmt* _constructorStmt;
    int pass;	
		CXXRecordDecl* _entryCXXRecordDecl;
		CXXMethodDecl* _entryMethodDecl;
		callbackMethods _callbackMethods;
    // Disallow constructor with no argument
    FindSocketRegisterMethods(llvm::raw_ostream &os);

  };
}
#endif
