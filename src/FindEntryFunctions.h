#ifndef _FIND_ENTRY_FUNCTIONS_H_
#define _FIND_ENTRY_FUNCTIONS_H_

#include <vector>
#include <string>
#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "EntryFunctionContainer.h"
#include <map>
namespace scpar {

	using namespace clang;
	using namespace std;

	class FindEntryFunctions : public RecursiveASTVisitor < FindEntryFunctions > {
	public:

		/// Typedefs
		typedef vector < EntryFunctionContainer * > entryFunctionVectorType;

		typedef pair < string, vector < string > > entryFunctionLHSPairType;
		typedef map < string, vector < string > > entryFunctionLHSMapType;

    FindEntryFunctions(CXXRecordDecl * d, llvm::raw_ostream & os);
    virtual ~FindEntryFunctions();

		/// Virtual methods from RecursiveASTVisitor
		virtual bool VisitStringLiteral (StringLiteral * l);
		virtual bool VisitCXXMethodDecl (CXXMethodDecl * d);
		virtual bool VisitMemberExpr (MemberExpr * e);

		// Access Methods
		CXXRecordDecl *getEntryCXXRecordDecl();
		CXXMethodDecl *getEntryMethodDecl();
		string getEntryName();
		entryFunctionVectorType *getEntryFunctions();
    vector < CXXMethodDecl * > getOtherFunctions();

		void dump ();

	private:

    llvm::raw_ostream & _os;
    CXXRecordDecl * _d;
		bool _isEntryFunction;
		PROCESS_TYPE _procType;
		string _entryName;
		CXXRecordDecl *_entryCXXRecordDecl;
		CXXMethodDecl *_entryMethodDecl;
		bool _foundEntryDeclaration;
		Stmt *_constructorStmt;
		EntryFunctionContainer *ef;
		int pass;
		entryFunctionVectorType _entryFunctions;
		entryFunctionLHSMapType _entryFunctionLHSMap;
    vector < CXXMethodDecl * >_otherFunctions;
  
		// Disallow constructor with no argument
    FindEntryFunctions (llvm::raw_ostream & os
                        );

	};
}
#endif
