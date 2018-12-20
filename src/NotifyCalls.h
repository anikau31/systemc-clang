//===-- src/NotifyCalls.h - systec-clang class definition -------*- C++ -*-===//
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
#ifndef _NOTIFY_CALLS_H_
#define _NOTIFY_CALLS_H_

#include "clang/AST/DeclCXX.h"
#include <string>
#include <map>
#include "Utility.h"
#include "FindNotify.h"

namespace scpar {
  using namespace clang;
  using namespace std;

  class NotifyCalls {
  public:
    // Constructors.
    NotifyCalls();
    NotifyCalls( const string &, FindNotify::NotifyCallListType );

    // Copy constructor.
    NotifyCalls( const NotifyCalls & );

    // Destructor.
    ~NotifyCalls();

    /// Get parameters
    string getName() const;
    FindNotify::NotifyCallListType getNotifyCallList();

    unsigned int getNumNotifyCalls();
    // Print
    void dump ( llvm::raw_ostream &, int );

  private:
    string name_;
    FindNotify::NotifyCallListType notify_call_list_;
  };
}
#endif
