#ifndef _FIND_EVENTS_H_
#define _FIND_EVENTS_H_

#include <map>
#include <string>
#include <vector>
#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
//#include "ReflectionContainerClass.h"

namespace scpar {
	using namespace clang;
	using namespace std;

	class FindEvents : public RecursiveASTVisitor < FindEvents > {
	public:
		/// typedefs
		typedef map < string, FieldDecl * > classEventMapType;
		typedef pair < string, FieldDecl * > kvType;

    FindEvents(CXXRecordDecl *, llvm::raw_ostream &);
    FindEvents(const FindEvents &);
    virtual ~FindEvents();
		/// RecursiveASTVisitor methods
		virtual bool VisitFieldDecl (FieldDecl *);

		/// Access methods
		classEventMapType getInClassEvents();
    vector < string > getEventNames();

		/// Print methods
		void dump();

	private:
    llvm::raw_ostream & _os;
		classEventMapType _inClassEvents;
    //    reflectionDataStructure * _reflectionMap;
	};

}
#endif
