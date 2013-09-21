#ifndef _FIND_SOCKET_BINDING_H_
#define _FIND_SOCKET_BINDING_H_

#include <vector>
#include <string>
#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include <map>
namespace scpar {
 
  using namespace clang;
  using namespace std;
  
  class FindSocketBinding: public RecursiveASTVisitor<FindSocketBinding> {
  public:
		
		FindSocketBinding(CXXRecordDecl*, llvm::raw_ostream&);
		
		typedef pair <string, string> initiatorTargetPairType;
		typedef pair <initiatorTargetPairType, string> 
		initiatorTargetSocketPairType;
		typedef map <initiatorTargetPairType, string> initiatorTargetSocketMapType;

		
		/// Virtual methods from RecursiveASTVisitor
		virtual bool VisitCXXMemberCallExpr(CXXMemberCallExpr *ce);
		virtual bool VisitCXXMethodDecl(CXXMethodDecl *md);		
		initiatorTargetSocketMapType getInitiatorTargetSocketMap();
		// Access Methods

    void dump();
   
  private:
  
    CXXRecordDecl *_d;
    llvm::raw_ostream &_os;
    Stmt* _constructorStmt;
    int pass;		
		CXXRecordDecl* _entryCXXRecordDecl;
		CXXMethodDecl* _entryMethodDecl;
		initiatorTargetSocketMapType _initiatorTargetSocketMap;
    // Disallow constructor with no argument
    FindSocketBinding(llvm::raw_ostream &os);
		string _initiator;
		string _bindObject;
		string _target;

  };
}
#endif
