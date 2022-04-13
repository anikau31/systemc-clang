#include "SystemCClang.h"
#include <doctest.h>

#include "Matchers.h"

// This is automatically generated from cmake.
#include "ClangArgs.h"
#include "Testing.h"

using namespace systemc_clang;
using namespace sc_ast_matchers;

TEST_CASE("Only parse a single top-level module"){
  std::string code{systemc_clang::read_systemc_file(
      systemc_clang::test_data_dir, "xor-hierarchy-input.cpp")};
  llvm::DebugFlag = true;

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, systemc_clang::catch_test_args)
          .release();

  // Name of top-level module declaration.
  std::string top{"exor2"};

  SystemCConsumer sc{from_ast};
  // sc.setTopModule(top);
  sc.HandleTranslationUnit(from_ast->getASTContext());
  auto model{sc.getSystemCModel()};
  auto found_module_decl{model->getInstance("dut_exor2")};
  SUBCASE("Testing top-level module: exor2") {
    // There should be only one module.
    INFO("Top-level module specified as exor2.");
      // Actually found the module.
    CHECK(found_module_decl != nullptr);  // != module_decl.end());

    auto found_decl{found_module_decl};
    CHECK(found_decl->getIPorts().size() == 2);
    CHECK(found_decl->getOPorts().size() == 1);
    CHECK(found_decl->getSignals().size() == 3);
    CHECK(found_decl->getOtherVars().size() == 0);
    // Other sc_module instances are recognized as others.
    CHECK(found_decl->getNestedModuleInstances().size() == 4);

    // Check how many nested modules it has.
    auto nested_decls{found_module_decl->getNestedModuleInstances()};
    CHECK(nested_decls.size() == 4);

    // Print out the nested declaration's.
    llvm::outs() << "############# Nested Decl test bug ##############\n";
    for (const auto &ndecl : nested_decls) {
      ndecl->dump(llvm::outs());

      //
      // This is how we should be accessing the EntryFunctionContainer.
      //
      llvm::outs() << "############ Process map way \n";
      auto process_map{ndecl->getProcessMap()};
      for (const auto &proc : process_map) {
        // Get the EntryFunctionContainer.
        ProcessDecl *pd{get<1>(proc)};
        auto entry_function{pd->getEntryFunction()};
        // Get the sensitivity map.
        auto sense_map{entry_function->getSenseMap()};

        for (auto const &sense : sense_map) {
          llvm::outs() << "sensitivity name: " << sense.first << "\n";
          // There is more to the sensitivity info ...
        }
      }

      // Test the bug
      // This is how we should not be allowed to access the
      // EntryFunctionContainer. This access should be removed.
      llvm::outs() << "############ Bug way \n";
      std::vector<EntryFunctionContainer *> entry_functions{
          ndecl->getEntryFunctionContainer()};
      // Go through each of the entry function and print them out.
      for (const auto &ef : entry_functions) {
        // ef->dump(llvm::outs(),0);
        auto sense_map{ef->getSenseMap()};

        for (auto const &sense : sense_map) {
          llvm::outs() << "sensitivity name: " << sense.first << "\n";
          // There is more to the sensitivity info ...
        }
      }
    }

    // Check port bindings
    // Instance: exor2
    found_module_decl = model->getInstance("exor2");

    auto port_bindings{found_module_decl->getPortBindings()};
    llvm::outs() << "decl: " << found_decl->getName()
                 << ", Number of port bindings: " << port_bindings.size()
                 << "\n";

    int check_count{3};
    for (auto const &binding : port_bindings) {
      PortBinding *pb{binding.second};
      std::string port_name{pb->getCallerPortName()};
      std::string caller_name{pb->getCallerInstanceName()};
      std::string as_string{pb->toString()};
      llvm::outs() << "check string: " << as_string << "\n";

      if (caller_name == "dut") {
        if (port_name == "A") {
          CHECK(as_string == "exor2 dut dut_exor2 A ASig");
          --check_count;
        }
        if (port_name == "B") {
          CHECK(as_string == "exor2 dut dut_exor2 B BSig");
          --check_count;
        }
        if (port_name == "F") {
          CHECK(as_string == "exor2 dut dut_exor2 F FSig");
          --check_count;
        }
      }
    }
    CHECK(check_count == 0);

    //
    // Instance: dut_exor2
    found_module_decl = model->getInstance("dut_exor2");
    port_bindings = found_module_decl->getPortBindings();
    llvm::outs() << "decl: " << found_decl->getName()
                 << ", Number of port bindings: " << port_bindings.size()
                 << "\n";

    check_count = 12;
    for (auto const &binding : port_bindings) {
      PortBinding *pb{binding.second};
      std::string port_name{pb->getCallerPortName()};
      std::string caller_name{pb->getCallerInstanceName()};
      std::string as_string{pb->toString()};
      llvm::outs() << "check string: " << as_string << "\n";

      // n1
      if (caller_name == "n1") {
        if (port_name == "A") {
          CHECK(as_string == "nand2 n1 N1 A A");
          --check_count;
        }
        if (port_name == "B") {
          CHECK(as_string == "nand2 n1 N1 B B");
          --check_count;
        }
        if (port_name == "F") {
          CHECK(as_string == "nand2 n1 N1 F S1");
          --check_count;
        }
      }

      // n2
      if (caller_name == "n2") {
        if (port_name == "A") {
          CHECK(as_string == "nand2 n2 N2 A A");
          --check_count;
        }
        if (port_name == "B") {
          CHECK(as_string == "nand2 n2 N2 B S1");
          --check_count;
        }
        if (port_name == "F") {
          CHECK(as_string == "nand2 n2 N2 F S2");
          --check_count;
        }
      }

      // n3
      if (caller_name == "n3") {
        if (port_name == "A") {
          CHECK(as_string == "nand2 n3 N3 A S1");
          --check_count;
        }
        if (port_name == "B") {
          CHECK(as_string == "nand2 n3 N3 B B");
          --check_count;
        }
        if (port_name == "F") {
          CHECK(as_string == "nand2 n3 N3 F S3");
          --check_count;
        }
      }

      if (caller_name == "n4") {
        if (port_name == "A") {
          CHECK(as_string == "nand2 n4 N4 A S2");
          --check_count;
        }
        if (port_name == "B") {
          CHECK(as_string == "nand2 n4 N4 B S3");
          --check_count;
        }
        if (port_name == "F") {
          CHECK(as_string == "nand2 n4 N4 F F");
          --check_count;
        }
      }
    }
    CHECK(check_count == 0);
  }
}
