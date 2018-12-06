#ifndef _FIND_NOTIFY_H_
#define _FIND_NOTIFY_H_

#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include <map>
namespace scpar {

	using namespace clang;
	using namespace std;

	class FindNotify:public RecursiveASTVisitor < FindNotify > {
	public:
		typedef vector < CallExpr * > notifyCallListType;


    FindNotify(CXXMethodDecl *, raw_ostream &);
    virtual ~FindNotify();
		virtual bool VisitCallExpr (CallExpr *expr);

		notifyCallListType getNotifyCallList ();
    CXXMethodDecl * getEntryMethod();
		void dump();

	private:
    CXXMethodDecl * _entryMethodDecl;
    raw_ostream & _os;
		CallExpr *_notifyCall;
		notifyCallListType _notifyCalls;
	};
}
#endif
