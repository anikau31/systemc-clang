#ifndef _APINT_UTILS_H_
#define _APINT_UTILS_H_

#include <string>

#include "llvm/ADT/APInt.h"
#include "llvm/ADT/SmallString.h"

namespace systemc_clang {
namespace utils {
namespace apint {

template <typename T>
std::string toString(const T &i) {
  llvm::SmallString<10> small_str;
  i.toString(small_str, 10, true);

  return small_str.str().str();
}

};  // namespace apint
};  // namespace utils
};  // namespace systemc_clang
#endif
