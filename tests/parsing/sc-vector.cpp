#include "SystemCClang.h"
#include "catch.hpp"
// This is automatically generated from cmake.
#include <iostream>

#include "ClangArgs.h"
#include "Testing.h"

using namespace systemc_clang;

// Source:
// https://www.toptip.ca/2010/03/trim-leading-or-trailing-white-spaces.html
std::string &trim(std::string &s) {
  size_t p = s.find_first_not_of(" \t");
  s.erase(0, p);

  p = s.find_last_not_of(" \t");
  if (string::npos != p) s.erase(p + 1);

  return s;
}

TEST_CASE("Parsing sc_vector", "[sc_vector]") {

  std::string code{systemc_clang::read_systemc_file(
      systemc_clang::test_data_dir, "sc-vector-input.cpp")};
  INFO(systemc_clang::test_data_dir);

  auto catch_test_args = systemc_clang::catch_test_args;
  //catch_test_args.push_back("-I" + systemc_clang::test_data_dir +
  //                          "/llnl-examples/zfpsynth/shared2/");

  llvm::outs() << "Test directory: " << systemc_clang::test_data_dir << "\n";

  ASTUnit *from_ast = tooling::buildASTFromCodeWithArgs(code, catch_test_args).release();

  /// Turn debug on
  //
  llvm::DebugFlag = true;

  SystemCConsumer systemc_clang_consumer{from_ast};
  systemc_clang_consumer.HandleTranslationUnit(from_ast->getASTContext());

  auto model{systemc_clang_consumer.getSystemCModel()};

  // This provides the module declarations.
  auto module_decl{model->getModuleDecl()};
  auto module_instance_map{model->getModuleInstanceMap()};

  // Want to find an instance named "testing".

  ModuleDecl *test_module{model->getInstance("testing")};

  SECTION("Found sc_module instances", "[instances]") {
    // There should be 2 modules identified.
    INFO("Checking number of sc_module declarations found: "
         << module_decl.size());

    REQUIRE(module_decl.size() == 2);

    REQUIRE(test_module != nullptr);

    INFO("Checking member ports for test instance.");
    // These checks should be performed on the declarations.

    // The module instances have all the information.
    // This is necessary until the parsing code is restructured.
    // There is only one module instance
    // auto module_instances{model->getModuleInstanceMap()};
    // auto p_module{module_decl.find("test")};
    //
    //
    auto test_module_inst{test_module};

    // Check if the proper number of ports are found.
    REQUIRE(test_module_inst->getIPorts().size() == 1);
    REQUIRE(test_module_inst->getOPorts().size() == 0);
    REQUIRE(test_module_inst->getIOPorts().size() == 0);
    REQUIRE(test_module_inst->getSignals().size() == 0);
    REQUIRE(test_module_inst->getInputStreamPorts().size() == 0);
    REQUIRE(test_module_inst->getOutputStreamPorts().size() == 0);
    REQUIRE(test_module_inst->getOtherVars().size() == 2);

    //
    // Check port types
    //
    //
    for (auto const &port : test_module_inst->getOtherVars()) {
      std::string name{get<0>(port)};
      PortDecl *pd{get<1>(port)};
      FindTemplateTypes *template_type{pd->getTemplateType()};
      Tree<TemplateType> *template_args{template_type->getTemplateArgTreePtr()};

      // Print out each argument individually using the iterators.
      //

      // Note: template_args must be dereferenced.
      for (auto const &node : *template_args) {
        const TemplateType *type_data{node->getDataPtr()};
        llvm::outs() << "\n- name: " << name
                     << ", type name: " << type_data->getTypeName() << " ";

        // Access the parent of the current node.
        // If the node is a pointer to itself, then the node itself is the
        // parent. Otherwise, it points to the parent node.
        auto parent_node{node->getParent()};
        if (parent_node == node) {
          llvm::outs() << "\n@> parent (itself) type name: "
                       << parent_node->getDataPtr()->getTypeName() << "\n";
        } else {
          // It is a different parent.
          llvm::outs() << "\n@> parent (different) type name: "
                       << parent_node->getDataPtr()->getTypeName() << "\n";
        }

        // Access the children for each parent.
        // We use the template_args to access it.

        auto children{template_args->getChildren(parent_node)};
        for (auto const &kid : children) {
          llvm::outs() << "@> child type name: "
                       << kid->getDataPtr()->getTypeName() << "\n";
        }
      }
      llvm::outs() << "\n";

      std::string dft_str{template_args->dft()};

      if (name == "v4v4") {
        REQUIRE(trim(dft_str) == "sc_vector4 sc_vector4 simple_module");
      }
      if (name == "v4") {
        REQUIRE(trim(dft_str) == "sc_vector4 simple_module");
      }

      if (name == "three_dim") {
        REQUIRE(pd->getArrayType() == true);
        REQUIRE(pd->getArraySizes().size() == 3);
        std::vector<llvm::APInt> sizes{pd->getArraySizes()};
        REQUIRE(sizes[0].getLimitedValue() == 2);
        REQUIRE(sizes[1].getLimitedValue() == 3);
        REQUIRE(sizes[2].getLimitedValue() == 4);
      }
    }

    for (auto const &port : test_module_inst->getOPorts()) {
      auto name{get<0>(port)};
      PortDecl *pd{get<1>(port)};
      auto template_type{pd->getTemplateType()};
      auto template_args{template_type->getTemplateArgTreePtr()};

      std::string dft_str{template_args->dft()};

      if ((name == "out1") || (name == "out2")) {
        REQUIRE(trim(dft_str) == "sc_out int");
      }

      if (name == "out_array_port") {
        REQUIRE(pd->getArrayType() == true);
        REQUIRE(pd->getArraySizes().front() == 5);
      }

      if (name == "two_dim") {
        REQUIRE(pd->getArrayType() == true);
        REQUIRE(pd->getArraySizes().size() == 2);
        std::vector<llvm::APInt> sizes{pd->getArraySizes()};
        REQUIRE(sizes[0].getLimitedValue() == 2);
        REQUIRE(sizes[1].getLimitedValue() == 3);
      }
    }

    for (auto const &port : test_module_inst->getIOPorts()) {
      auto name{get<0>(port)};
      PortDecl *pd{get<1>(port)};
      auto template_type{pd->getTemplateType()};
      auto template_args{template_type->getTemplateArgTreePtr()};

      std::string dft_str{template_args->dft()};

      if ((name == "in_out")) REQUIRE(trim(dft_str) == "sc_inout double");
    }

    for (auto const &sig : test_module_inst->getSignals()) {
      auto name{get<0>(sig)};
      SignalDecl *sg{get<1>(sig)};
      auto template_type{sg->getTemplateTypes()};
      auto template_args{template_type->getTemplateArgTreePtr()};

      // Get the tree as a string and check if it is correct.
      std::string dft_str{template_args->dft()};
      if (name == "internal_signal") {
        REQUIRE(trim(dft_str) == "sc_signal int");
      }

      /// Check array parameters
      if (name == "data") {
        REQUIRE(sg->getArrayType() == true);
        REQUIRE(sg->getArraySizes().front() == 4);
      }

      if (name == "two_dim_sig") {
        REQUIRE(sg->getArrayType() == true);
        REQUIRE(sg->getArraySizes().size() == 2);
        std::vector<llvm::APInt> sizes{sg->getArraySizes()};
        REQUIRE(sizes[0].getLimitedValue() == 2);
        REQUIRE(sizes[1].getLimitedValue() == 3);
      }

      if (name == "three_dim_sig") {
        REQUIRE(sg->getArrayType() == true);
        REQUIRE(sg->getArraySizes().size() == 3);
        std::vector<llvm::APInt> sizes{sg->getArraySizes()};
        REQUIRE(sizes[0].getLimitedValue() == 2);
        REQUIRE(sizes[1].getLimitedValue() == 3);
        REQUIRE(sizes[2].getLimitedValue() == 4);
      }
    }
    //
    // INFO("Checking member ports for simple module instance.");
    // auto simple_module_inst{simple_module};
    //
    // REQUIRE(simple_module_inst->getIPorts().size() == 3);
    // REQUIRE(simple_module_inst->getOPorts().size() == 1);
    // REQUIRE(simple_module_inst->getIOPorts().size() == 0);
    // REQUIRE(simple_module_inst->getSignals().size() == 0);
    // REQUIRE(simple_module_inst->getOtherVars().size() == 1);
    // REQUIRE(simple_module_inst->getInputStreamPorts().size() == 0);
    // REQUIRE(simple_module_inst->getOutputStreamPorts().size() == 0);
    //

    // for (auto const &port : simple_module_inst->getIPorts()) {
    // auto name{get<0>(port)};
    // PortDecl *pd{get<1>(port)};
    // auto template_type{pd->getTemplateType()};
    // auto template_args{template_type->getTemplateArgTreePtr()};
    //
    // std::string dft_str{template_args->dft()};
    //
    // if (name == "clk") {
    // REQUIRE(trim(dft_str) == "sc_in _Bool");
    // }
    // if ((name == "one") || (name == "two")) {
    // REQUIRE(trim(dft_str) == "sc_in int");
    // }
    // }
    //
    // for (auto const &port : simple_module_inst->getOPorts()) {
    // auto name{get<0>(port)};
    // PortDecl *pd{get<1>(port)};
    // auto template_type{pd->getTemplateType()};
    // auto template_args{template_type->getTemplateArgTreePtr()};
    //
    // std::string dft_str{template_args->dft()};
    //
    // if ((name == "out_one")) {
    // REQUIRE(trim(dft_str) == "sc_out int");
    // }
    // }
    //
    // for (auto const &ovar : simple_module_inst->getOtherVars()) {
    // auto name{get<0>(ovar)};
    // PortDecl *pd{get<1>(ovar)};
    // auto template_type{pd->getTemplateType()};
    // auto template_args{template_type->getTemplateArgTreePtr()};
    //
    // std::string dft_str{template_args->dft()};
    //
    // if ((name == "xy")) {
    // REQUIRE(trim(dft_str) == "int");
    // }
    // }

    //
    //
    // Check netlist
    //
    //

    //    test_module_inst
    // auto port_bindings{test_module_inst->getPortBindings()};
    // std::vector<std::string> test_ports{"in1", "in_out", "out1"};
    //
    // for (auto const &pname : test_ports) {
    // auto found_it{port_bindings.find(pname)};
    // REQUIRE(found_it != port_bindings.end());
    // std::string port_name{found_it->first};
    // PortBinding *binding{found_it->second};
    // std::string as_string{binding->toString()};
    //
    // if (port_name == "in1") {
    // REQUIRE(as_string == "test test_instance testing sig1");
    // }
    // if (port_name == "in_out") {
    // REQUIRE(as_string == "test test_instance testing double_sig");
    // }
    // if (port_name == "out1") {
    // REQUIRE(as_string == "test test_instance testing sig1");
    // }
    // }
  }
}
