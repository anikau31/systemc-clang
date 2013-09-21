#ifndef _FIND_INITIATOR_TARGET_H_
#define _FIND_INITIATOR_TARGET_H_

#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include <map>
namespace scpar {

  using namespace clang;
  using namespace std;
  
  class FindInitiatorTarget: public RecursiveASTVisitor<FindInitiatorTarget> {
  public:
    
		FindInitiatorTarget(CXXMethodDecl*, llvm::raw_ostream&);
		~FindInitiatorTarget();
    
		virtual bool VisitStmt(Stmt* s);     
		virtual bool VisitMemberExpr(MemberExpr *ce);

  private:
		llvm::raw_ostream &_os;	
		CXXMethodDecl* _entryMethodDecl;
	};
}

#endif
