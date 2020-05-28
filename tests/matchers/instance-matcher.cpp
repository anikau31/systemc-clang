#include "SystemCClang.h"
#include "catch.hpp"
#include "clang/Tooling/Tooling.h"

#include "Matchers.h"

// This is automatically generated from cmake.
#include "ClangArgs.h"
#include "Testing.h"

using namespace clang::ast_matchers;
using namespace scpar;
using namespace sc_ast_matchers;

template <typename T>
bool find_name(std::vector<T> &names, const T &find_name) {
  auto found_it = std::find(names.begin(), names.end(), find_name);
  if (found_it != names.end()) {
    names.erase(found_it);
    return true;
  }

  return false;
}

/*
TEST_CASE("Test for templated instance matching", "[instance matching]") {
  std::string code{systemc_clang::read_systemc_file(
      systemc_clang::test_data_dir, "netlist-matcher-templated-model.cpp")};

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, systemc_clang::catch_test_args)
          .release();

  llvm::outs() << "================ TESTMATCHER =============== \n";
  InstanceMatcher inst_matcher{};
  MatchFinder matchRegistry{};
  inst_matcher.registerMatchers(matchRegistry);
  // Run all the matchers
  matchRegistry.matchAST(from_ast->getASTContext());
  inst_matcher.dump();
  llvm::outs() << "================ END =============== \n";

  SECTION("Test instance matcher", "[instance-matcher]") {
    auto instances{inst_matcher.getInstanceMap()};

    INFO (" There is a bug with templates that it is identifying additional
instances"); REQUIRE(instances.size() == 2);

  }
}
*/

// This test works
TEST_CASE("Read SystemC model from file for testing", "[parsing]") {
  std::string code{systemc_clang::read_systemc_file(
      systemc_clang::test_data_dir, "xor-hierarchy.cpp")};

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, systemc_clang::catch_test_args)
          .release();

  llvm::outs() << "================ TESTMATCHER =============== \n";
  InstanceMatcher inst_matcher{};
  MatchFinder matchRegistry{};
  inst_matcher.registerMatchers(matchRegistry);
  // Run all the matchers
  matchRegistry.matchAST(from_ast->getASTContext());
  inst_matcher.dump();
  llvm::outs() << "================ END =============== \n";

  SECTION("Test instance matcher", "[instance-matcher]") {
    // There should be five instances here.
    // DUT2, n1, n2, n3, n4, d
    auto instances{inst_matcher.getInstanceMap()};

    REQUIRE(instances.size() == 6);

    std::vector<std::string> var_names{"dut", "d", "n1",
                                       "n2",  "n3",      "n4"};
    std::vector<std::string> var_type_names{
        "struct exor2", "struct DUT",
        "struct nand2", "struct nand2",
        "struct nand2", "struct nand2"};
    std::vector<std::string> instance_names{"exor2",  "N1",
                                            "N2",    "N3",        "N4"};

    for (auto const &entry : instances) {
      auto inst{entry.second};
      inst.dump();

      find_name(var_names, inst.var_name);
      find_name(var_type_names, inst.var_type_name);
      find_name(instance_names, inst.instance_name);

      // Test findInstanceByVariableType()
      if (inst.is_field_decl) {
        auto fdecl{(cast<FieldDecl>(entry.first))};
        auto cxx_decl{fdecl->getType().getTypePtr()->getAsCXXRecordDecl()};

        std::vector<InstanceMatcher::InstanceDeclType> found_instances;
        inst_matcher.findInstanceByVariableType(cxx_decl, found_instances);

        if (inst.var_name == "dut") {
          // Find all the instances of exor2
          REQUIRE(found_instances.size() == 1);
        } else {
          // Find all the instances of nand2
          REQUIRE(found_instances.size() == 4);
        }

        // Check the parent of the FieldDecl to see whom it is instantiated in.
        if ( (inst.var_name == "n1") || (inst.var_name == "n2") || (inst.var_name == "n3") || (inst.var_name == "n4") )   {
          REQUIRE(inst.parent_name == "dut");
        }
      }
    }
    // All the variable name should be found
    REQUIRE(var_names.size() == 0);
    REQUIRE(var_type_names.size() == 0);
    REQUIRE(instance_names.size() == 0);
  }
}
