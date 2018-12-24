#include "FindSCMain.h"

using namespace scpar;

FindSCMain::FindSCMain ( TranslationUnitDecl *tuDecl, llvm::raw_ostream &os ) :
    os_{os},
    sc_main_function_declaration_{nullptr} {
        assert (!( tuDecl == nullptr ));
        TraverseDecl( tuDecl );
    }

FindSCMain::~FindSCMain() {
    sc_main_function_declaration_ = nullptr;
}

bool FindSCMain::VisitFunctionDecl( FunctionDecl *function_declaration ) {

    /// Find sc_main.
    /// There are three conditions to satisfy this:
    /// 1. Must have sc_main in its name.
    /// 2. Must have a body
    /// 3. Must *not* be a first declaration. (This is becuase systemc.h includes a null definition of sc_main.
    if ((function_declaration->getNameInfo ().getAsString () != "sc_main")
        || (!function_declaration->hasBody ()) || (function_declaration->isMain ()))    {
        return true;
    }

    sc_main_function_declaration_ = function_declaration;
    return true;
}

FunctionDecl *FindSCMain::getSCMainFunctionDecl() const {
    assert ( sc_main_function_declaration_ != nullptr );

    return sc_main_function_declaration_;
}

bool FindSCMain::isSCMainFound() const {
    return ( sc_main_function_declaration_ != nullptr );
}
