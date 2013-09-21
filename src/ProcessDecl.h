#ifndef _PROCESSDECL_H_
#define _PROCESSDECL_H_

#include <string>
#include <map>
#include "clang/AST/DeclCXX.h"
#include "FindEntryFunctions.h"
#include "EntryFunctionContainer.h"

namespace scpar {

  using namespace std;
  using namespace clang;
  
  class ProcessDecl {
  public:
    // typedef
//    typedef map<string, EntryFunctionContainer* > entryFunctionContainerMapType;
//    typedef pair<string, EntryFunctionContainer* > entryFunctionContainerPairType;    

    ProcessDecl(string , string, CXXMethodDecl*, Stmt*, EntryFunctionContainer* );
    
    ProcessDecl(const ProcessDecl& );
    ~ProcessDecl();
    
    /// Accessor methods.
    string getType();
    string getName();
    CXXMethodDecl* getEntryMethodDecl();
    Stmt* getConstructorStmt();

    
    // Dump.
    void dump(raw_ostream&, int );
  protected:
    // Process information
    string  _type;
    string _entryName;
    // Each process can have 1 entry function.
    CXXMethodDecl* _entryMethodDecl;
    Stmt* _constructorStmt;

    EntryFunctionContainer* _ef;
  }; // End class ProcessDecl
  
} // End namespace scpar

#endif
