#ifndef _INTERFACE_DECL_H_
#define _INTERFACE_DECL_H_

#include <string>
#include "FindTemplateTypes.h"

namespace scpar {
using namespace clang;
using namespace std;

class InterfaceDecl {
 public:
  InterfaceDecl();
  InterfaceDecl(const std::string &, FindTemplateTypes *);

  InterfaceDecl(const InterfaceDecl &);

  ~InterfaceDecl();
  // Set parameters
  void setModuleName(const std::string &);

  /// Get parameters
  std::string getName();
  FindTemplateTypes *getTemplateType();

  // Print
  void dump(llvm::raw_ostream &, int tabn = 0);

 private:
  std::string _name;
  FindTemplateTypes *_templateType;
};
}  // namespace scpar
#endif
