//===-- src/WaitCalls.h - systec-clang class definition -------*- C++ -*-===//
//
//                     systemc-clang: SystemC Parser
//
// This file is distributed under the University of Illinois License.
// See LICENSE.mkd for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Stores the data members found in a class.
///
//===----------------------------------------------------------------------===//
#ifndef _WAIT_CALLS_H_
#define _WAIT_CALLS_H_

#include "FindWait.h"
#include <string>

namespace systemc_clang {

class WaitCalls {
 public:
  // Constructors.
  WaitCalls();
  WaitCalls(const std::string &, FindWait::waitListType);

  // Copy constructor.
  WaitCalls(const WaitCalls &);

  // Destructor.
  virtual ~WaitCalls();

  /// Get parameters
  std::string getName();
  FindWait::waitListType getWaitList();

  unsigned int getTotalWaits();
  // Print
  void dump(llvm::raw_ostream &, int);

 private:
  std::string _name;
  FindWait::waitListType _waitList;
};
}  // namespace systemc_clang
#endif
