#ifndef _ENTRY_FUNCTIONS_H_
#define _ENTRY_FUNCTIONS_H_
#include <iostream>
#include <map>
#include <vector>
#include "clang/AST/DeclCXX.h"

namespace scpar {

  using namespace clang;
  using namespace std;
  
  class EntryFunctionType {
  public:
    EntryFunctionType();
    EntryFunctionType(const EntryFunctionType& );
    
    PROCESS_TYPE _procType;
    string _entryName;
    CXXMethodDecl* _entryMethodDecl;
    Stmt* _constructorStmt;
  };

  class EntryFunctions : ReflectionContainerClass, EntryFunctionType
   {
  public:
    EntryFunctions(string, EntryFunctionType *);
    EntryFunctionType * getrecordInstance();
    vector<ReflectionContainerClass*> * getSCModuleEntryChildren();
  
  protected:
    EntryFunctionType * recordInstance;
  };
}

#endif
