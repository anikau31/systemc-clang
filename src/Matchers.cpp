#include <iostream>
#include "Matchers.h"
#include "clang/AST/DeclCXX.h"

using namespace std;
using namespace sc_ast_matchers;

//
// Helper functions that can be made private to this class.
//
// AST matcher to detect instances of sc_modules.
// auto makeInstanceInModuleMatcher(const std::string &name) {
  // auto match_module_instance = fieldDecl(
      // hasType(cxxRecordDecl(isDerivedFrom(hasName("::sc_core::sc_module")))));
  // return match_module_instance;
// }
//End of helper functions


