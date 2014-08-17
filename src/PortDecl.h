#ifndef _PORT_DECL_H_
#define _PORT_DECL_H_

#include "clang/AST/DeclCXX.h"
#include <string>
#include <map>
#include "FindTemplateTypes.h"

namespace scpar {
  using namespace clang;
  using namespace std;

  class PortDecl {
  public:
    PortDecl();
    PortDecl(const string &, FindTemplateTypes *);

    PortDecl(const PortDecl &);

    ~PortDecl();
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
