/* clang-format off */
#include "catch.hpp"

#include "SystemCClang.h"
#include "Matchers.h"

// This is automatically generated from cmake.
#include "ClangArgs.h"
#include "Testing.h"

/* clang-format on */

using namespace scpar;
using namespace sc_ast_matchers;

TEST_CASE("Only parse a single top-level module", "[parsing]") {
  std::string code{systemc_clang::read_systemc_file(
      systemc_clang::test_data_dir, "templated-module.cpp")};

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, systemc_clang::catch_test_args)
          .release();
  //
  //  Name of top-level module declaration.
  std::string top{"non_template"};
  SystemCConsumer sc{from_ast};
  sc.setTopModule(top);
  sc.HandleTranslationUnit(from_ast->getASTContext());
  auto model{sc.getSystemCModel()};
  auto module_decl{model->getModuleDecl()};
  //
  // auto found_module{std::find_if(module_decl.begin(), module_decl.end(),
  // [&top](const auto &element) {
  // return element.second->getName() == top;
  // })};
  //

  // The model has 3 module declarations.
  REQUIRE(module_decl.size() == 3);
  ModuleDecl *found_module{model->getInstance("non-templated-module-instance")};
  SECTION("Testing top-level module: non_template", "[top-module]") {
    // There should be only one module.
    INFO("Top-level module specified as non_template.");

    // Actually found the module.
    REQUIRE(found_module != nullptr);

    auto found_decl{found_module};
    REQUIRE(found_decl->getIPorts().size() == 3);
    REQUIRE(found_decl->getOPorts().size() == 0);
    REQUIRE(found_decl->getOtherVars().size() == 3);
  }
}

TEST_CASE("Testing top-level module: test", "[top-module]") {
  std::string code{systemc_clang::read_systemc_file(
      systemc_clang::test_data_dir, "templated-module.cpp")};

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, systemc_clang::catch_test_args)
          .release();


  // Name of top-level module declaration.
  std::string top{"test"};

  SystemCConsumer sc{from_ast};
  sc.setTopModule(top);
  sc.HandleTranslationUnit(from_ast->getASTContext());
  auto model{sc.getSystemCModel()};
  auto module_decl{model->getModuleDecl()};

  auto found_module_testing{model->getInstance("testing")};
  auto found_module_testing_float{model->getInstance("testing_float_double")};

  REQUIRE(module_decl.size() == 3);
  SECTION("Testing top-level module: test", "[top module]") {
    // There should be two modules because there are two instances.
    INFO("Top-level module specified as test.");

    // Actually found the module.
    REQUIRE(found_module_testing != nullptr);
    REQUIRE(found_module_testing_float != nullptr);

    auto found_decl{found_module_testing};
    REQUIRE(found_decl->getIPorts().size() == 5);
    REQUIRE(found_decl->getOPorts().size() == 2);
    // This is 4 because sc_buffer is also inheriting from the signal interface.
    REQUIRE(found_decl->getSignals().size() == 4);
    // 1 non-array, and 2 array others
    REQUIRE(found_decl->getOtherVars().size() == 2);

    // TODO: Check the template parameters.
    //

    // test_module_inst
    INFO("KNOWN FAILING: Need to fix the parsing of ports for templated modules"); 
    auto port_bindings{found_decl->getPortBindings()};
    std::vector<std::string> test_ports{"clk", "inS", "outS"};

    // Print all the port binding information
    for (auto const &p: port_bindings) {
      auto pb{ p.second };
      pb->dump();
    }

    for (auto const &pname : test_ports) {
      auto found_it{port_bindings.find(pname)};
      // Actually found the name
      REQUIRE(found_it != port_bindings.end());
      // Check now if the names
      std::string port_name{found_it->first};
      PortBinding *binding{found_it->second};
      std::string as_string{binding->toString()};

      if (port_name == "clk") {
        REQUIRE(as_string == "test test_instance testing clk");
      }
      if (port_name == "inS") {
        REQUIRE(as_string == "test test_instance testing sig1");
      }
      if (port_name == "outS") {
        REQUIRE(as_string == "test test_instance testing sig1");
      }
    }


    auto found_decl2{found_module_testing_float};
    REQUIRE(found_decl2->getIPorts().size() == 5);
    // Array is the second port being recognized.
    REQUIRE(found_decl2->getOPorts().size() == 2);
    // 1 regular signal, 2 array signals, 1 sc_buffer, which is a signal too.
    REQUIRE(found_decl2->getSignals().size() == 4);
    // 1 non-array, and 2 array others
    REQUIRE(found_decl2->getOtherVars().size() == 2);

    // TODO: Check the template parameters.
    //
  }
}
