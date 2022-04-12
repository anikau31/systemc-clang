#include <doctest.h>
#include "SystemCClang.h"

// This is automatically generated from cmake.
#include "ClangArgs.h"
#include "Testing.h"

using namespace systemc_clang;

TEST_CASE("Read SystemC model from file for testing") {
  std::string code{systemc_clang::read_systemc_file(
      systemc_clang::test_data_dir, "basic-module-method-input.cpp")};

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, systemc_clang::catch_test_args)
          .release();

  SystemCConsumer sc{from_ast};
  sc.HandleTranslationUnit(from_ast->getASTContext());
  auto model{sc.getSystemCModel()};

  SUBCASE("No ports bound for test declaration") {
    // The module instances have all the information.

    auto test_module{model->getInstance("d")};

    // There is only one module instance

    // Check if the proper number of ports are found.
    INFO(
        "FAIL_TEST: A module must have a port bound for it to be "
        "recognized.");
    CHECK(test_module != nullptr);
    auto module_ptr{test_module};

    CHECK(module_ptr->getInstanceName() == "d");

    /*
    CHECK(module_ptr->getIPorts().size() == 2);
    CHECK(module_ptr->getOPorts().size() == 1);
    CHECK(module_ptr->getIOPorts().size() == 0);
    CHECK(module_ptr->getSignals().size() == 1);
    CHECK(module_ptr->getOtherVars().size() == 0);
    CHECK(module_ptr->getInputStreamPorts().size() == 0);
    CHECK(module_ptr->getOutputStreamPorts().size() == 0);
    */

    // instance: d
    auto test_module_inst{module_ptr};
    auto port_bindings{test_module_inst->getPortBindings()};

    int check_count{3};
    for (auto const &binding : port_bindings) {
      PortBinding *pb{binding.second};
      std::string port_name{pb->getCallerPortName()};
      std::string caller_name{pb->getCallerInstanceName()};
      std::string as_string{pb->toString()};
      llvm::outs() << "check string: " << as_string << "\n";
       if (caller_name == "test_instance") {
        if (port_name == "clk") {
          CHECK(as_string == "test test_instance testing_pb clk clock");
          --check_count;
        }
        if (port_name == "in") {
          CHECK(as_string == "test test_instance testing_pb in sig1");
          --check_count;
        }
        if (port_name == "out") {
          CHECK(as_string == "test test_instance testing_pb out sig1");
          --check_count;
        }
      }

    }
    CHECK(check_count == 0);


    // Instance: testing_pb
    test_module = model->getInstance("testing_pb");

    CHECK(test_module->getInstanceName() == "testing_pb");
    module_ptr = test_module;

    CHECK(module_ptr->getIPorts().size() == 2);
    CHECK(module_ptr->getOPorts().size() == 1);
    CHECK(module_ptr->getIOPorts().size() == 0);
    CHECK(module_ptr->getSignals().size() == 1);
    CHECK(module_ptr->getOtherVars().size() == 0);
    CHECK(module_ptr->getInputStreamPorts().size() == 0);
    CHECK(module_ptr->getOutputStreamPorts().size() == 0);
    CHECK(module_ptr->getPortBindings().size() == 0);
    CHECK(module_ptr->getNestedModuleInstances().size() == 0); 

  }
}
