#ifndef _FIND_PORTS_H_
#define _FIND_PORTS_H_

#include <map>
#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "FindTemplateTypes.h"

namespace scpar {
	using namespace clang;
	using namespace std;

	class FindPorts:public RecursiveASTVisitor < FindPorts > {
	public:
		/// typedefs
		typedef map < string, FindTemplateTypes * >portType;
		typedef pair < string, FindTemplateTypes * >kvType;


    FindPorts(CXXRecordDecl *, llvm::raw_ostream &	);
    virtual ~FindPorts();
		virtual bool VisitFieldDecl (FieldDecl *);

		portType getInputPorts();
		portType getOutputPorts();
		portType getInputOutputPorts();
		void dump ();

	private:
    llvm::raw_ostream & _os;
		portType _inPorts;
		portType _outPorts;
		portType _inoutPorts;

	};

}
#endif
