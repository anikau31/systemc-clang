#ifndef _ENTRY_FUNCTION_TYPE_H_
#define _ENTRY_FUNCTION_TYPE_H_

#include <iostream>
#include "clang/AST/DeclCXX.h"
#include "enums.h"

namespace scpar {
	using namespace clang;
	using namespace std;


	class EntryFunctionType {
	public:
		EntryFunctionType (
		);
		EntryFunctionType (const EntryFunctionType &
		);

		Stmt *getConstructorStmt (
		) {
			return _constructorStmt;
		} CXXMethodDecl *getEntryMethod (
		) {
			return _entryMethodDecl;
		} PROCESS_TYPE _procType;
		string _entryName;
		CXXMethodDecl *_entryMethodDecl;
		Stmt *_constructorStmt;
	};
}
#endif
