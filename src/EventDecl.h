#ifndef _EVENT_DECL_H_
#define _EVENT_DECL_H_

#include "clang/AST/DeclCXX.h"
#include <string>
#include <map>
#include "Utility.h"

namespace scpar {
	using namespace clang;
	using namespace std;

	class EventDecl {
	public:
		// Typedefs.
		typedef pair < string, FieldDecl * >eventPairType;
		typedef map < string, FieldDecl * >eventMapType;

		// Constructors.
		  EventDecl (
		);
		  EventDecl (const string &, FieldDecl *
		);

		// Copy constructor.
		  EventDecl (const EventDecl &
		);

		// Destructor.
		 ~EventDecl (
		);

		/// Get parameters
		string getName (
		);
		FieldDecl *getASTNode (
		);
		// Print
		void dump (raw_ostream &, int tabn = 0);

	private:
		void ptrCheck (
		);
	private:
		  string _name;
		FieldDecl *_astNode;
	};
}
#endif
