//===-- src/EventContainer.h - systec-clang class definition -------*- C++
//-*-===//
//
//                     systemc-clang: SystemC Parser
//
// This file is distributed under the University of Illinois License.
// See LICENSE.mkd for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Container class to hold sc_event information.
///
//===----------------------------------------------------------------------===//
#ifndef _EVENT_CONTAINER_H_
#define _EVENT_CONTAINER_H_

//#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include <string>

/// Forward declaration of clang class.
namespace clang {
class VarDecl;
};

namespace systemc_clang {
class EventContainer {
 public:
  EventContainer();
  EventContainer(std::string, clang::VarDecl *);

  virtual ~EventContainer();

  // Copy constructor.
  EventContainer(const EventContainer &);

  std::string getEventName() const;
  clang::VarDecl *getASTNode() const;

  void dump(llvm::raw_ostream &os, int tabn = 0);

 private:
  // Name of port on which the binding happens.
  std::string event_name_;
  clang::VarDecl *ast_node_;
};
}  // namespace systemc_clang
#endif
