#ifndef _SIGNAL_CONTAINER_H_
#define _SIGNAL_CONTAINER_H_

#include "PortDecl.h"
#include "FindTemplateTypes.h"

#include "llvm/Support/raw_ostream.h"

namespace scpar {

// Forward declarations

class SignalContainer : public PortDecl {
  public:
  SignalContainer(const std::string &name, FindTemplateTypes *tt, clang::FieldDecl *fd);

  // Copy constructor
  SignalContainer(const SignalContainer &from);

  ~SignalContainer();

  void dump(llvm::raw_ostream &os);

};
};
#endif
