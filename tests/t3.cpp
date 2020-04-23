#include "catch.hpp"
#include "SystemCClang.h"

// This is automatically generated from cmake.
#include "ClangArgs.h"
#include "Testing.h"

using namespace scpar;

TEST_CASE("Read SystemC model from file for testing", "[parsing]") {
  std::string code{systemc_clang::read_systemc_file(
      systemc_clang::test_data_dir, "basic-module-method.cpp")};

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, systemc_clang::catch_test_args)
          .release();

  SystemCConsumer sc{from_ast};
  sc.HandleTranslationUnit(from_ast->getASTContext());
  auto model{sc.getSystemCModel()};
  auto module_decl{model->getModuleDecl()};

  SECTION("No ports bound for test declaration", "[ports]") {
    // The module instances have all the information.
    // auto test_module{module_decl.find("test")};

    auto test_module{model->getInstance("testing_pb")};

    // There is only one module instance

    // Check if the proper number of ports are found.
    INFO(
        "FAIL_TEST: A module must have a port bound for it to be "
        "recognized.");
    REQUIRE(test_module != nullptr);
    auto module_ptr{test_module};

    REQUIRE(module_ptr->getInstanceName() == "testing_pb");

    REQUIRE(module_ptr->getIPorts().size() == 2);
    REQUIRE(module_ptr->getOPorts().size() == 1);
    REQUIRE(module_ptr->getIOPorts().size() == 0);
    REQUIRE(module_ptr->getSignals().size() == 1);
    REQUIRE(module_ptr->getOtherVars().size() == 0);
    REQUIRE(module_ptr->getInputStreamPorts().size() == 0);
    REQUIRE(module_ptr->getOutputStreamPorts().size() == 0);

    // test_module_inst
    auto test_module_inst{module_ptr};
    auto port_bindings{test_module_inst->getPortBindings()};
    std::vector<std::string> test_ports{"clk", "in", "out"};

    for (auto const &pname : test_ports) {
      auto found_it{port_bindings.find(pname)};
      // Actually found the name
      REQUIRE(found_it != port_bindings.end());
      // Check now if the names
      std::string port_name{found_it->first};
      PortBinding *binding{found_it->second};
      std::string as_string{binding->toString()};

      if (port_name == "in") {
        REQUIRE(as_string == "test test_instance testing_pb sig1");
      }
      if (port_name == "out") {
        REQUIRE(as_string == "test test_instance testing_pb sig1");
      }
      if (port_name == "clk") {
        REQUIRE(as_string == "test test_instance testing_pb clock");
      }
    }


  }
}
