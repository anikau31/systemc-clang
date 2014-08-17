#ifndef _NOTIFY_CONTAINER_H_
#define _NOTIFY_CONTAINER_H_

#include <map>
#include <string>
#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/PrettyPrinter.h"

namespace scpar {
	using namespace clang;
	using namespace std;

	class NotifyContainer {
	public:
		// typedefs
		typedef vector < string > waitArgVectorType;

		  NotifyContainer ();
		  NotifyContainer (CXXMethodDecl *, CallExpr *);

		 ~NotifyContainer ();

		// Copy constructor.
		  NotifyContainer (const NotifyContainer &);

		unsigned int getNumArgs ();
		CXXMethodDecl *getEntryMethod ();
		CallExpr *getASTNode ();
		waitArgVectorType getArgList ();

		void dump (raw_ostream &, int tabn = 0);

	private:
		void populateArgMap ();
		string getArgString (Expr * e);

	private:
		  CXXMethodDecl * _entryMethodDecl;
		CallExpr *_astNode;
		unsigned int _numArgs;

		waitArgVectorType _args;
	};
}
#endif
