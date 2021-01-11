#ifndef _PROCESS_DECL_H_
#define _PROCESS_DECL_H_

#include <string>
#include "json.hpp"
#include "systemc-clang.h"

/// clang forward declarations.
namespace clang {
class CXXMethodDecl;
};

namespace systemc_clang {

using json = nlohmann::json;

/// Forward declarations.
class EntryFunctionContainer;

class ProcessDecl {
 public:
  ProcessDecl(std::string, std::string, clang::CXXMethodDecl *,
              EntryFunctionContainer *);

  ProcessDecl(const ProcessDecl &);
  virtual ~ProcessDecl();

  /// Get methods.
  std::string getType() const;
  std::string getName() const;
  const clang::CXXMethodDecl *getEntryMethodDecl() const;
  EntryFunctionContainer *getEntryFunction();

  /// Dump.
  void dump();
  json dump_json() const;

 protected:
  /// Process information
  std::string process_type_;
  /// Name of the entry function
  std::string entry_name_;
  /// Each process can have 1 entry function.
  const clang::CXXMethodDecl *entry_method_decl_;
  /// This is a container that holds information about the entry function.
  EntryFunctionContainer *entry_function_ptr_;
};  // End class ProcessDecl

}  // End namespace systemc_clang
#endif
