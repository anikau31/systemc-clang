//===-- src/enums.h - systec-clang class definition -------*- C++ -*-===//
//
//                     systemc-clang: SystemC Parser
//
// This file is distributed under the University of Illinois License.
// See LICENSE.mkd for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Defining enumerated types used throughout systemc-clang.
///
//===----------------------------------------------------------------------===//
#ifndef _ENUM_H_
#define _ENUM_H_

namespace systemc_clang {
enum class ASTSTATE {
  EMPTY,
  DECLSTMT,
  MEMBEREXPR,
  CXXOPERATORCALLEXPR,
  CXXCONSTRUCTEXPR
};  // End enum ASTSTATE

enum class ReadWrite { RWINIT, READ, WRITE };  // End enum ReadWrite

enum class PROCESS_TYPE {
  NONE,
  THREAD,
  CTHREAD,
  METHOD
};  // End enum PROCESS_TYPE
}  // End namespace systemc_clang
#endif
