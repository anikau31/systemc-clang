#include "SystemCClang.h"
#include "catch.hpp"

#include "Matchers.h"

// This is automatically generated from cmake.
#include "ClangArgs.h"
#include "Testing.h"

using namespace systemc_clang;
using namespace sc_ast_matchers;

TEST_CASE("Only parse a single top-level module", "[parsing]") {
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
  /*
  auto module_decl{model->getModuleDecl()};

  std::string module_instance_name{"exor2"};

  // Search for the module we want.
  ModuleDecl *found_module_decl{nullptr};
  for (auto const &entry : module_decl) {
    // This is the sc_module class name.
    llvm::outs() << "decl name: " << entry.first << "\n";
    auto instance_list{ entry.second };
    // for (auto const &inst : instance_list) {
      // if (inst->getInstanceName() == module_instance_name ) {
        // found_module_decl = inst;
      // }
    // }
  }

                                  */
  auto found_module_decl{model->getInstance("dut_exor2")};
  SECTION("Testing top-level module: exor2", "[exor2]") {
    // There should be only one module.
    INFO("Top-level module specified as exor2.");
    // exor2 has several sub-modules in it.
    // Note that module_decl will just hold the three unique module
    // declarations. The instances will not be there. Therefore, the total
    // number of module decls are 3: nand2, exor.
    // REQUIRE(module_instances.size() == 2);

    // Actually found the module.
    REQUIRE(found_module_decl != nullptr);  // != module_decl.end());

    // auto found_decl{found_module_decl}; //->second};
    auto found_decl{found_module_decl};
    REQUIRE(found_decl->getIPorts().size() == 2);
    REQUIRE(found_decl->getOPorts().size() == 1);
    REQUIRE(found_decl->getSignals().size() == 3);
    REQUIRE(found_decl->getOtherVars().size() == 0);
    // Other sc_module instances are recognized as others.
    REQUIRE(found_decl->getNestedModules().size() == 4);

    // Check how many nested modules it has.
    // It should have 4: N1 - N4
    auto nested_decls{ found_module_decl->getNestedModules()};
    REQUIRE(nested_decls.size() == 4);

    // Print out the nested declaration's. 
    llvm::outs() << "############# Nested Decl test bug ##############\n";
    for (const auto &ndecl: nested_decls ) {
      ndecl->dump(llvm::outs());

      // 
      // This is how we should be accessing the EntryFunctionContainer.
      // 
      llvm::outs() << "############ Process map way \n";
      auto process_map{ndecl->getProcessMap()};
      for (const auto &proc: process_map) {
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
      // This is how we should not be allowed to access the EntryFunctionContainer.
      // This access should be removed.
      llvm::outs() << "############ Bug way \n";
      std::vector<EntryFunctionContainer*> entry_functions{ndecl->getEntryFunctionContainer()};
      // Go through each of the entry function and print them out.
      for (const auto &ef: entry_functions) {
       // ef->dump(llvm::outs(),0);
        auto sense_map{ef->getSenseMap()};

        for (auto const &sense : sense_map) {
          llvm::outs() << "sensitivity name: " << sense.first << "\n";
          // There is more to the sensitivity info ...
        }

      }
    }


    // Check port bindings
    //
    // test_module_inst
    auto port_bindings{found_decl->getPortBindings()};
    llvm::outs() << "decl: " << found_decl->getName()
                 << ", Number of port bindings: " << port_bindings.size()
                 << "\n";
    std::vector<std::string> test_ports{"A", "B", "F"};

    for (auto const &pname : test_ports) {
      llvm::outs() << "=> pname: " << pname << "\n";
      auto found_it{port_bindings.find(pname)};
      // Actually found the name
      REQUIRE(found_it != port_bindings.end());
      // Check now if the names
      std::string port_name{found_it->first};
      PortBinding *binding{found_it->second};
      std::string as_string{binding->toString()};

      if (port_name == "A") {
        REQUIRE(as_string == "exor2 dut dut_exor2 ASig");
      }
      if (port_name == "B") {
        REQUIRE(as_string == "exor2 dut dut_exor2 BSig");
      }
      if (port_name == "F") {
        REQUIRE(as_string == "exor2 dut dut_exor2 FSig");
      }
    }

    llvm::outs() << "All checks passed.\n";
  }

  // Submodule port bindings
  // std::vector<std::string> instance_names{"N1", "N2", "N3", "N4"};
  // std::vector<ModuleDecl *> found_modules{};
//
  // Populate a vector of ModuleDecl* that match the above instance names.
  // for (auto const &name : instance_names) {
    // found_modules.push_back(model->getInstance(name));
  // }
//
  // REQUIRE(found_modules.size() == instance_names.size());
  auto found_module_n1{model->getInstance("N1")};
  auto found_module_n2{model->getInstance("N2")};
  auto found_module_n3{model->getInstance("N3")};
  auto found_module_n4{model->getInstance("N4")};

  REQUIRE(found_module_n1 != nullptr);
  REQUIRE(found_module_n2 != nullptr);
  REQUIRE(found_module_n3 != nullptr);
  REQUIRE(found_module_n4 != nullptr);

  SECTION("Test port information and binding for sub-modules",
          "[nested submodules]") {
    llvm::outs() << "Found a sub-module: " << found_module_n1 << "\n";

    // N1
    auto port_bindings{found_module_n1->getPortBindings()};
    std::vector<std::string> test_ports{"A", "B", "F"};

    for (auto const &pname : test_ports) {
      auto found_it{port_bindings.find(pname)};
      std::string port_name{found_it->first};
      PortBinding *binding{found_it->second};
      std::string as_string{binding->toString()};

      if (port_name == "A") {
        REQUIRE(as_string == "nand2 n1 N1 A");
      }
      if (port_name == "B") {
        REQUIRE(as_string == "nand2 n1 N1 B");
      }
      if (port_name == "F") {
        REQUIRE(as_string == "nand2 n1 N1 S1");
      }
    }

    // N2
    port_bindings = found_module_n2->getPortBindings();

    for (auto const &pname : test_ports) {
      auto found_it{port_bindings.find(pname)};
      std::string port_name{found_it->first};
      PortBinding *binding{found_it->second};
      std::string as_string{binding->toString()};

      if (port_name == "A") {
        REQUIRE(as_string == "nand2 n2 N2 A");
      }
      if (port_name == "B") {
        REQUIRE(as_string == "nand2 n2 N2 S1");
      }
      if (port_name == "F") {
        REQUIRE(as_string == "nand2 n2 N2 S2");
      }
    }


    // N3
    port_bindings = found_module_n3->getPortBindings();

    for (auto const &pname : test_ports) {
      auto found_it{port_bindings.find(pname)};
      std::string port_name{found_it->first};
      PortBinding *binding{found_it->second};
      std::string as_string{binding->toString()};

      if (port_name == "A") {
        REQUIRE(as_string == "nand2 n3 N3 S1");
      }
      if (port_name == "B") {
        REQUIRE(as_string == "nand2 n3 N3 B");
      }
      if (port_name == "F") {
        REQUIRE(as_string == "nand2 n3 N3 S3");
      }
    }

    // N4
    port_bindings = found_module_n4->getPortBindings();

    for (auto const &pname : test_ports) {
      auto found_it{port_bindings.find(pname)};
      std::string port_name{found_it->first};
      PortBinding *binding{found_it->second};
      std::string as_string{binding->toString()};

      if (port_name == "A") {
        REQUIRE(as_string == "nand2 n4 N4 S2");
      }
      if (port_name == "B") {
        REQUIRE(as_string == "nand2 n4 N4 S3");
      }
      if (port_name == "F") {
        REQUIRE(as_string == "nand2 n4 N4 F");
      }
    }
  }


}
