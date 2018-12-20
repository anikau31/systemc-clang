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

    FindEntryFunctions( CXXRecordDecl * d, llvm::raw_ostream & os );
    virtual ~FindEntryFunctions();

    /// Virtual methods from RecursiveASTVisitor
    virtual bool VisitStringLiteral( StringLiteral * l );
    virtual bool VisitCXXMethodDecl( CXXMethodDecl * d );
    virtual bool VisitMemberExpr( MemberExpr * e );

    // Access Methods
    CXXRecordDecl *getEntryCXXRecordDecl();
    CXXMethodDecl *getEntryMethodDecl();
    string getEntryName();
    entryFunctionVectorType *getEntryFunctions();
    vector < CXXMethodDecl * > getOtherFunctions();

    void dump();

  private:

    llvm::raw_ostream &os_;
    CXXRecordDecl * _d;
    bool is_entry_function_;
    PROCESS_TYPE proc_type_;
    string entry_name_;
    CXXRecordDecl *entry_cxx_record_decl_;
    CXXMethodDecl *entry_method_decl_;
    bool found_entry_decl_;
    Stmt *constructor_stmt_;
    EntryFunctionContainer *ef;
    int pass_;
    entryFunctionVectorType entry_function_list_;
    entryFunctionLHSMapType entry_function_map_;
    vector< CXXMethodDecl * > other_function_list_;

    // Disallow constructor with no argument
    FindEntryFunctions( llvm::raw_ostream &os );

  };
}
#endif
