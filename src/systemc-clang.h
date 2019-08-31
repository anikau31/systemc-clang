#ifndef _SYSTEMC_CLANG_H_
#define _SYSTEMC_CLANG_H_
#include <sstream>
#include <string>
#include <iostream>
#include "json.hpp"

namespace scpar {


using json = nlohmann::json;

template < typename T>
std::string to_string( T * pointer ) {
  std::ostringstream address;
  address << pointer;
  return address.str();
}
}
#endif
