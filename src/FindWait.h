#ifndef _FIND_WAIT_H_
#define _FIND_WAIT_H_

#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include <map>
namespace scpar {

	using namespace clang;
	using namespace std;

	class FindWait:public RecursiveASTVisitor < FindWait > {
	public:

		typedef vector < CallExpr * >waitListType;

		typedef pair < CXXMethodDecl *,vector < string > >processWaitEventPairType;
		typedef map < CXXMethodDecl *, vector < string > >processWaitEventMapType;

    FindWait (CXXMethodDecl *, llvm::raw_ostream &);
    virtual ~FindWait();

		virtual bool VisitCallExpr(CallExpr *expr);


		CXXMethodDecl* getEntryMethod();
		waitListType getWaitCalls();
		void dump();

	private:

    CXXMethodDecl *_entryMethodDecl;
    llvm::raw_ostream & _os;
    CallExpr *_waitCall;
    Expr *_firstArg;
		bool _foundWait;
		waitListType _waitCalls;

	};
}
#endif
