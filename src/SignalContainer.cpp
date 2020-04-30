#include "SignalContainer.h"

using namespace scpar;

SignalContainer::SignalContainer(const std::string &name, FindTemplateTypes *tt,
                                 clang::FieldDecl *fd)
    : PortDecl(name, fd, tt) {}

// Copy constructor
SignalContainer::SignalContainer(const SignalContainer &from)
    : PortDecl(from) {}

SignalContainer::~SignalContainer() {
  // Base class should be cleaning everything up.
}

void SignalContainer::dump(llvm::raw_ostream &os) {
  llvm::outs() << "SignalContainer ";
  PortDecl::dump(llvm::outs());
}
