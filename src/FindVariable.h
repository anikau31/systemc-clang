#ifndef _FIND_VARIABLE_
#define _FIND_VARIABLE_

#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"

namespace scpar {
	using namespace clang;
	using namespace std;

	class FindVariable: public RecursiveASTVisitor<FindVariable> {
		
		public:
			FindVariable(Expr *ce);
			virtual bool VisitMemberExpr(MemberExpr *me);
			virtual bool VisitDeclRefExpr(DeclRefExpr *de);			

			string getArgumentName();
			bool getAccessType();
			private:
				Expr *_e;
				string _argumentName;
				bool _accessType;
	};
}
#endif
