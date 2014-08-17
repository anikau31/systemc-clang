#ifndef _INTERFACE_DECL_H_
#define _INTERFACE_DECL_H_

#include "clang/AST/DeclCXX.h"
#include <string>
#include <map>
#include "FindTemplateTypes.h"

namespace scpar {
  using namespace clang;
  using namespace std;

  class InterfaceDecl {
  public:
    InterfaceDecl();
    InterfaceDecl(const string &, FindTemplateTypes *);

    InterfaceDecl(const InterfaceDecl &);

    ~InterfaceDecl();
    // Set parameters
    void setModuleName(const string &);

    /// Get parameters
    string getName();
    FindTemplateTypes *getTemplateType();

    // Print
    void dump(raw_ostream &, int tabn = 0);

  private:
    string _name;
    FindTemplateTypes *_templateType;
  };
}
#endif
