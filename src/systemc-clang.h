#ifndef _SYSTEMC_CLANG_H_
#define _SYSTEMC_CLANG_H_
//#include <iostream>
#include <sstream>
#include <string>
//#include "json.hpp"
namespace systemc_clang {

//using json = nlohmann::json;

template <typename T>
std::string to_string(T* pointer) {
  std::ostringstream address;
  address << pointer;
  return address.str();
}
}  // namespace systemc_clang
#endif
