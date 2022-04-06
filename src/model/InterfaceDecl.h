#ifndef _INTERFACE_DECL_H_
#define _INTERFACE_DECL_H_

#include <string>
#include "llvm/Support/Debug.h"

namespace systemc_clang {

  /// Forward declarations
  class FindTemplateTypes;

class InterfaceDecl {
 public:
  InterfaceDecl();
  InterfaceDecl(const std::string &, FindTemplateTypes *);

  InterfaceDecl(const InterfaceDecl &);

  virtual ~InterfaceDecl();

  // Set parameters
  void setModuleName(const std::string &);

  /// Get parameters
  std::string getName();
  FindTemplateTypes *getTemplateType();

  // Print
  void dump(llvm::raw_ostream &, int tabn = 0);

 private:
  std::string name_;
  FindTemplateTypes *template_type_;
};
}  // namespace systemc_clang
#endif
