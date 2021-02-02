#ifndef _SIGNAL_DECL_H_
#define _SIGNAL_DECL_H_

#include <string>

#include "json.hpp"

#include "PortDecl.h"

namespace systemc_clang {

using json = nlohmann::json;

/// Forward declarations
class FindTemplateTypes;

///////////////////////////////////////////////////////////////////////////////
///
/// Class SignalDecl
///
/// This class stores signals that were declared in an sc_module.
///
///////////////////////////////////////////////////////////////////////////////

class SignalDecl : public PortDecl {
 public:
  /// Constructors
  SignalDecl();
  SignalDecl(const PortDecl &pd);

  /// Default destrubtor.
  virtual ~SignalDecl();

  /// Construct a signal declaration.
  ///
  /// \param name Name of the signal declaration.
  /// \param fd The FieldDecl pointer from the AST that refers to the signal
  /// declaration. 
  /// \param tt The template types that are found.
  SignalDecl(const std::string &name, clang::FieldDecl *fd,
             FindTemplateTypes *tt);

  /// Get parameters
  std::string getName();

  /// Return the template types that were found.
  FindTemplateTypes *getTemplateTypes();

  /// Return the AST node found for the signal declaration.
  const clang::FieldDecl *getASTNode() const;

  /// Dump json.
  json dump_json();
};
}  // namespace systemc_clang
#endif
