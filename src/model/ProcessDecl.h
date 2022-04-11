#ifndef _PROCESS_DECL_H_
#define _PROCESS_DECL_H_

#include <sstream>
#include <string>

/// clang forward declarations.
namespace clang {
class CXXMethodDecl;
};

namespace systemc_clang {

/// Forward declarations.
class EntryFunctionContainer;

template <typename T>
std::string to_string(T* pointer) {
  std::ostringstream address;
  address << pointer;
  return address.str();
}

class ProcessDecl {
 public:
  /// Constructor.
  ///
  /// \param process_type Specified whether it is an SC_METHOD, SC_THREAD or
  ///  SC_CTHREAD.
  /// \param entry_name The name of the entry function (function associated with
  /// the process).
  /// entry_method_decl The CXXMethodDecl for the entry function.
  /// \param entry_fn The container that holds entry function information.
  ProcessDecl(std::string process_type, std::string entry_name,
              clang::CXXMethodDecl *entry_method_decl,
              EntryFunctionContainer *entry_fn);

  /// Copy constructor.
  ProcessDecl(const ProcessDecl &);

  /// Destructor.
  virtual ~ProcessDecl();

  /// Get methods.
  std::string getType() const;
  std::string getName() const;
  const clang::CXXMethodDecl *getEntryMethodDecl() const;
  EntryFunctionContainer *getEntryFunction();

  /// Dump.
  std::string asString() const;

  void dump();

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
