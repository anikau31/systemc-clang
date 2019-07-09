#ifndef _PROCESSDECL_H_
#define _PROCESSDECL_H_

#include "systemc-clang.h"
#include "json.hpp"
#include "EntryFunctionContainer.h"
#include "FindEntryFunctions.h"
#include "clang/AST/DeclCXX.h"
#include <map>
#include <string>

namespace scpar {

using namespace std;
using namespace clang;
 using json = nlohmann::json;

class ProcessDecl {
public:
  // typedef

  ProcessDecl(string, string, CXXMethodDecl *, EntryFunctionContainer *);

  ProcessDecl(const ProcessDecl &);
  ~ProcessDecl();

  /// Accessor methods.
  string getType() const;
  string getName() const;
  CXXMethodDecl *getEntryMethodDecl() const;

  // Dump.
  void dump(raw_ostream &);
  json dump_json(raw_ostream &) const;

protected:
  // Process information
  string process_type_;
  // Name of the entry function
  string entry_name_;
  // Each process can have 1 entry function.
  CXXMethodDecl *entry_method_decl_;

  // This is a container that holds information about the
  // entry function. 
  EntryFunctionContainer *entry_function_ptr_;
}; // End class ProcessDecl

} // End namespace scpar
#endif
