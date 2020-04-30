#ifndef _PROCESS_DECL_H_
#define _PROCESS_DECL_H_

#include <string>
//#include "EntryFunctionContainer.h"
//#include "FindEntryFunctions.h"
#include "clang/AST/DeclCXX.h"
#include "json.hpp"
#include "systemc-clang.h"

namespace scpar {

using namespace clang;
using json = nlohmann::json;

class EntryFunctionContainer;

class ProcessDecl {
 public:
  // typedef

  ProcessDecl(std::string, std::string, CXXMethodDecl *,
              EntryFunctionContainer *);

  ProcessDecl(const ProcessDecl &);
  ~ProcessDecl();

  /// Accessor methods.
  std::string getType() const;
  std::string getName() const;
  CXXMethodDecl *getEntryMethodDecl() const;

  // Dump.
  void dump(llvm::raw_ostream &);
  json dump_json(llvm::raw_ostream &) const;

 protected:
  // Process information
  std::string process_type_;
  // Name of the entry function
  std::string entry_name_;
  // Each process can have 1 entry function.
  CXXMethodDecl *entry_method_decl_;
  // This is a container that holds information about the
  // entry function.
  EntryFunctionContainer *entry_function_ptr_;
};  // End class ProcessDecl

}  // End namespace scpar
#endif
