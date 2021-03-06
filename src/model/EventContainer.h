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

#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include <map>
#include <string>

namespace systemc_clang {
using namespace clang;
using namespace std;

class EventContainer {
public:
  EventContainer();
  EventContainer(string, VarDecl *);

  ~EventContainer();

  // Copy constructor.
  EventContainer(const EventContainer &);

  string getEventName() const;
  VarDecl *getASTNode() const;

  void dump(llvm::raw_ostream &os, int tabn = 0);

private:
  // Name of port on which the binding happens.
  string event_name_;
  VarDecl *ast_node_;
};
} // namespace systemc_clang
#endif
