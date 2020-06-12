#ifndef _TESTING_H_
#define _TESTING_H_

// Includes
#include <fstream>
//#include <iostream>
#include <sstream>
#include <string>

namespace systemc_clang {

std::ifstream file{};
// Open the file for reading
std::string read_systemc_file(std::string const data_dir,
                              std::string const name) {
  // If the file is already open, close it
  //  - this is just in case the file changed since the last opening
  if (file.is_open()) {
    file.close();
  }

  // Open the file and set the character position to zero
  file.open(data_dir + name);

  // If the file did not open, indicate an error occurred.
  if (!file.is_open()) {
    llvm::errs() << "[ERROR]: " << name << " not found" << "\n";
    return "";
  }

  std::string content;
  // Assign to the string the 'content' the contents of the file
  content = std::string(std::istreambuf_iterator<char>(file),
                        std::istreambuf_iterator<char>());

  return content;
}

};  // namespace systemc_clang

#endif
