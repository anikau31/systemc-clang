#ifndef _FIND_SENSITIVITY_H_
#define _FIND_SENSITIVITY_H_

#include <map>
#include <string>
#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
// ANI : A module may have multiple threads and their corresponding 
// sensitivity lists. So, this function should of finding sensitivity list
// should take into account that aspect.
namespace scpar {
	using namespace clang;
	using namespace std;

	class FindSensitivity : public RecursiveASTVisitor < FindSensitivity > {
  public:

		typedef map < string, MemberExpr * > senseMapType;
		typedef pair < string, MemberExpr * > kvType;

		FindSensitivity(Stmt *d, llvm::raw_ostream &os);
    virtual ~FindSensitivity();

		virtual bool VisitMemberExpr(MemberExpr *);
		virtual bool VisitCXXOperatorCallExpr(CXXOperatorCallExpr * e);

		void dump();
    senseMapType getSenseMap();
    
  private:
    llvm::raw_ostream & _os;
    bool _foundSensitiveNode;
    senseMapType _sensitivePorts;
    
	};

}
#endif
