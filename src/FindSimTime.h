#ifndef _FIND_SIM_TIME_H_
#define _FIND_SIM_TIME_H_

#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/PrettyPrinter.h"
#include <map>
#include "Utility.h"
namespace scpar {

	using namespace clang;
	using namespace std;

	class FindSimTime:public RecursiveASTVisitor < FindSimTime >, public Utility {
	public:

		typedef pair < string, string > simulationTimePairType;
		typedef map < string, string > simulationTimeMapType;

    FindSimTime (FunctionDecl *, llvm::raw_ostream &);
		virtual ~FindSimTime();
		virtual bool VisitCallExpr(CallExpr * C);

		simulationTimeMapType returnSimTime ();

	private:
    llvm::raw_ostream & _os;
		FunctionDecl *_sigInst;
		simulationTimeMapType _simTime;
		CallExpr *_callexpr;
	};
}
#endif
