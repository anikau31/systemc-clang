#ifndef _EVENT_DECL_H_
#define _EVENT_DECL_H_

#include "Utility.h"
#include "clang/AST/DeclCXX.h"
#include <map>
#include <string>

namespace systemc_clang {
using namespace clang;
using namespace std;

class EventDecl {
public:
  // Typedefs.
  //    typedef pair < string, FieldDecl * >eventPairType;
  //    typedef map < string, FieldDecl * >eventMapType;

  // Constructors.
  EventDecl();
  EventDecl(const string &, FieldDecl *);

  // Copy constructor.
  EventDecl(const EventDecl &);

  // Destructor.
  ~EventDecl();

  /// Get parameters
  string getName();
  FieldDecl *getASTNode();
  // Print
  void dump(raw_ostream &, int tabn = 0);

private:
  void ptrCheck();

private:
  string _name;
  FieldDecl *_astNode;
};
} // namespace systemc_clang
#endif
