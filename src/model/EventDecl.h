#ifndef _EVENT_DECL_H_
#define _EVENT_DECL_H_

#include "clang/AST/DeclCXX.h"
#include <string>

namespace systemc_clang {

class EventDecl {
 public:
  /// Constructors.
  EventDecl();
  EventDecl(const std::string &, clang::FieldDecl *);

  /// Copy constructor.
  EventDecl(const EventDecl &);

  /// Destructor.
  virtual ~EventDecl();

  /// Return the name of the sc_event.
  std::string getName() const;

  /// Return the FieldDecl node for the sc_event declaration.
  const clang::FieldDecl *getASTNode() const;

  /// Dump output.
  ///
  /// \param os Output to the stream.
  void dump(llvm::raw_ostream &);

 private:
  std::string name_;
  clang::FieldDecl *ast_node_;
};
}  // namespace systemc_clang
#endif
