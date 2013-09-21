//===-- src/Utility.h - systec-clang class definition -------*- C++ -*-===//
//
//                     systemc-clang: SystemC Parser
//
// This file is distributed under the University of Illinois License.
// See LICENSE.mkd for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief A set of utility functions in the Utility class.
///
//===----------------------------------------------------------------------===//
#ifndef _UTILITY_H_
#define _UTILITY_H_

#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include <string>
namespace scpar {
  using namespace clang;
  using namespace std;
  class Utility {
  public:
    Utility();
    
    void tabit(llvm::raw_ostream&, int tabn);
    string strip(string, string);
  };
}

#endif
