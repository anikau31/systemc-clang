#include "catch.hpp"

// This is automatically generated from cmake.
#include <iostream>
#include "ClangArgs.h"
#include <string>

#include "Tree.h"
using namespace scpar;

// Source:
// https://www.toptip.ca/2010/03/trim-leading-or-trailing-white-spaces.html
std::string &trim(std::string &s) {
  size_t p = s.find_first_not_of(" \t");
  s.erase(0, p);

  p = s.find_last_not_of(" \t");
  if (std::string::npos != p) s.erase(p + 1);

  return s;
}


TEST_CASE("Tree representation of template types", "[Tree]") {
  struct Node {
    std::string data_;

    // Constructor
    Node(std::string d) : data_(d) {}
    // These methods are necessary (I know it should be an interface but ...)
    //
    std::string getData() const { return data_; }
    std::string toString() const { return data_; }
  };

  Tree<Node> t{};

  auto zero = t.addNode(Node{"0"});
  auto one = t.addNode(Node{"1"});
  auto two = t.addNode(Node{"2"});
  auto three = t.addNode(Node{"3"});
  auto four = t.addNode(Node{"4"});
  auto five = t.addNode(Node{"5"});
  auto six = t.addNode(Node{"6"});

  // Layout of the tree
  //       0
  //      / \
  //    1    2
  //   /    / \ 
  //  6 -- 3  5
  //
  t.addEdge(zero, one);
  t.addEdge(zero, two);
  t.addEdge(two, three);
  t.addEdge(two, five);
  t.addEdge(three, six);
  t.addEdge(one, six);

  t.dump();

  std::cout << "DFS\n";
  auto dft_str {t.dft(zero)};
  REQUIRE(trim(dft_str) == "0 1 6 2 3 6 5");

  std::cout << "\nBFS\n";
  auto bft_str{t.bft(zero)};
  REQUIRE(trim(bft_str) == "0 2 1 5 3 6");
}
