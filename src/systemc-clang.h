#ifndef _SYSTEMC_CLANG_H_
#define _SYSTEMC_CLANG_H_

#include <sstream>
#include <string>
namespace systemc_clang {


template <typename T>
std::string to_string(T* pointer) {
  std::ostringstream address;
  address << pointer;
  return address.str();
}
}  // namespace systemc_clang
#endif
