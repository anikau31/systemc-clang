#include "FindSCMain.h"
using namespace scpar;

FindSCMain::FindSCMain (TranslationUnitDecl * tuDecl, llvm::raw_ostream & os):
  _os (os),
  _scmainFunctionDecl(nullptr) {
	assert (!(tuDecl == nullptr));
	TraverseDecl (tuDecl);
}

FindSCMain::~FindSCMain() {
  _scmainFunctionDecl = nullptr;
}

bool FindSCMain::VisitFunctionDecl(FunctionDecl *fdecl) {

	//_os << "Print the name: " << fdecl->getNameInfo().getAsString() << "has body: " << fdecl->hasBody() << "\n";
	//  _os << "Is first declaration: " << fdecl->isFirstDeclaration() << "\n";

	/// Find sc_main.
	/// There are three conditions to satisfy this:
	/// 1. Must have sc_main in its name.
	/// 2. Must have a body
	/// 3. Must *not* be a first declaration. (This is becuase systemc.h includes a null definition of sc_main.
	if ((fdecl->getNameInfo ().getAsString () != "sc_main")
			|| (!fdecl->hasBody ()) || (fdecl->isMain ()))	{
			return true;
		}

//  _os << "Found the sc_main\n";
	//fdecl->dump();

	_scmainFunctionDecl = fdecl;
	return true;
}

FunctionDecl *FindSCMain::getSCMainFunctionDecl() {
	assert (!(_scmainFunctionDecl == nullptr));

	return _scmainFunctionDecl;
}

bool FindSCMain::isSCMainFound() {
	return (_scmainFunctionDecl != nullptr);
}
