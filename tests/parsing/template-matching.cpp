/* clang-format off */
#include <doctest.h>

#include "SystemCClang.h"
#include "Matchers.h"

// This is automatically generated from cmake.
#include "ClangArgs.h"
#include "Testing.h"

/* clang-format on */

using namespace systemc_clang;
using namespace sc_ast_matchers;

TEST_CASE("Only parse a single top-level module") {
  std::string code{systemc_clang::read_systemc_file(
      systemc_clang::test_data_dir, "templated-module-input.cpp")};

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
  auto instances{model->getInstances()};
  //
  // auto found_module{std::find_if(module_decl.begin(), module_decl.end(),
  // [&top](const auto &element) {
  // return element.second->getName() == top;
  // })};
  //

  // The model has 3 module declarations.
  CHECK(instances.size() == 4);
  ModuleInstance *found_module{model->getInstance("non-templated-module-instance")};
  SUBCASE("Testing top-level module: non_template") {
    // There should be only one module.
    INFO("Top-level module specified as non_template.");

    // Actually found the module.
    CHECK(found_module != nullptr);

    auto found_decl{found_module};
    CHECK(found_decl->getIPorts().size() == 3);
    CHECK(found_decl->getOPorts().size() == 0);
    CHECK(found_decl->getOtherVars().size() == 3);
  }
}

TEST_CASE("Testing top-level module: test") {
  std::string code{systemc_clang::read_systemc_file(
      systemc_clang::test_data_dir, "templated-module-input.cpp")};

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, systemc_clang::catch_test_args)
          .release();


  // Name of top-level module declaration.
  std::string top{"test"};

  SystemCConsumer sc{from_ast};
  sc.setTopModule(top);
  sc.HandleTranslationUnit(from_ast->getASTContext());
  auto model{sc.getSystemCModel()};
  auto instances{model->getInstances()};

  auto found_module_testing{model->getInstance("testing")};
  auto found_module_testing_float{model->getInstance("testing_float_double")};
  auto dut{model->getInstance("d")};

  CHECK(instances.size() == 4);
  SUBCASE("Testing top-level module: test") {
    // There should be two modules because there are two instances.
    INFO("Top-level module specified as test.");

    // Actually found the module.
    CHECK(found_module_testing != nullptr);
    CHECK(found_module_testing_float != nullptr);

    auto found_decl{found_module_testing};
    CHECK(found_decl->getIPorts().size() == 5);
    CHECK(found_decl->getOPorts().size() == 2);
    // This is 4 because sc_buffer is also inheriting from the signal interface.
    CHECK(found_decl->getSignals().size() == 4);
    // 1 non-array, and 2 array others
    CHECK(found_decl->getOtherVars().size() == 2);

    // TODO: Check the template parameters.
    //

    //
    //
    //
    
    // Instance: testing
    CHECK(found_module_testing->getPortBindings().size() == 0);
    // Instance: d
    auto port_bindings{dut->getPortBindings()};

    int check_count{3};
    for (auto const &binding : port_bindings) {
      PortBinding *pb{binding.second};
      std::string port_name{pb->getCallerPortName()};
      std::string caller_name{pb->getCallerInstanceName()};
      std::string as_string{pb->toString()};
      llvm::outs() << "check string: " << as_string << "\n";
      if (caller_name == "test_instance") {
        if (port_name == "clk") {
          CHECK(as_string == "test test_instance testing clk clk");
          --check_count;
        }
        if (port_name == "inS") {
          CHECK(as_string == "test test_instance testing inS sig1");
          --check_count;
        }
        if (port_name == "outS") {
          CHECK(as_string == "test test_instance testing outS sig1");
          --check_count;
        }
      }
    }
    CHECK(check_count == 0);

  /*
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
      CHECK(found_it != port_bindings.end());
      // Check now if the names
      std::string port_name{found_it->first};
      PortBinding *binding{found_it->second};
      std::string as_string{binding->toString()};

      if (port_name == "clk") {
        CHECK(as_string == "test test_instance testing clk");
      }
      if (port_name == "inS") {
        CHECK(as_string == "test test_instance testing sig1");
      }
      if (port_name == "outS") {
        CHECK(as_string == "test test_instance testing sig1");
      }
    }
    */


    auto found_decl2{found_module_testing_float};
    CHECK(found_decl2->getIPorts().size() == 5);
    // Array is the second port being recognized.
    CHECK(found_decl2->getOPorts().size() == 2);
    // 1 regular signal, 2 array signals, 1 sc_buffer, which is a signal too.
    CHECK(found_decl2->getSignals().size() == 4);
    // 1 non-array, and 2 array others
    CHECK(found_decl2->getOtherVars().size() == 2);

  }
    // TODO: Check the template parameters.
    //
}
