#include "catch.hpp"

#include "SystemCClang.h"
// This is automatically generated from cmake.
#include <iostream>
#include "ClangArgs.h"

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

TEST_CASE("Basic parsing checks", "[parsing]") {
  std::string code = R"(
#include "systemc.h"

SC_MODULE( test ){

  // input ports
  sc_in_clk clk;
  sc_in<int> in1;
  sc_in<int> in2;
  // inout ports
  sc_inout<double> in_out;
  // output ports
  sc_out<int> out1;
  sc_out<int> out2;
  //signals
  sc_signal<int> internal_signal;

  // others
  int x;

  /// Test for array of ports and signals.
  //
  static constexpr int MAX_DEPTH = 4;
  const unsigned depth = MAX_DEPTH;

  sc_signal<int> data[MAX_DEPTH];

  sc_out<double> out_array_port[MAX_DEPTH+1];

  // 2d and 3d array
  sc_out<sc_uint<2>> two_dim[2][3];
  sc_in<sc_uint<2>> three_dim[2][3][4];

  sc_signal<sc_uint<2>> two_dim_sig[2][3];
  sc_signal<sc_uint<2>> three_dim_sig[2][3][4];

  void entry_function_1() {
    while(true) {
     x = x+1;
     out1.write(x);
    }
  }
  SC_CTOR( test ) {
    SC_METHOD(entry_function_1);
    sensitive << clk.pos();
  }
};

SC_MODULE( simple_module ){

  sc_in_clk clk;
  sc_in<int> one;
  sc_in<int> two;
  sc_out<int> out_one;
  int yx;

  void entry_function_1() {
    int x_var;
    double y_var;
    sc_int<4> z_var;
    while(true) {
    }
  }

  SC_CTOR( simple_module ) {
    SC_METHOD(entry_function_1);
    sensitive << clk.pos();
  }
};

SC_MODULE(DUT) {

  sc_signal<int> sig1;
  sc_signal<double> double_sig;

  test test_instance;

  simple_module simple;

  int others;
  SC_CTOR(DUT) : test_instance("testing"), simple("simple_module_instance") {
  test_instance.in1(sig1);
  test_instance.in_out(double_sig);
  test_instance.out1(sig1);

  simple.one(sig1);
  }


};

int sc_main(int argc, char *argv[]) {
  DUT d("d");
   return 0;
}
     )";

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, systemc_clang::catch_test_args)
          .release();


  /// Turn debug on
  //
  llvm::DebugFlag = true;

  SystemCConsumer systemc_clang_consumer{from_ast};
  systemc_clang_consumer.HandleTranslationUnit(from_ast->getASTContext());

  auto model{systemc_clang_consumer.getSystemCModel()};

  // This provides the module declarations.
  auto instances{model->getInstances()};
  auto module_instance_map{model->getModuleInstanceMap()};

  // Want to find an instance named "testing".

  ModuleDecl *test_module{model->getInstance("testing")};
  ModuleDecl *simple_module{model->getInstance("simple_module_instance")};

  SECTION("Found sc_module instances", "[instances]") {
    // There should be 2 modules identified.
    INFO("Checking number of sc_module instances found: "
         << instances.size());

    REQUIRE(instances.size() == 3);

    REQUIRE(test_module != nullptr);
    REQUIRE(simple_module != nullptr);

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
    REQUIRE(test_module_inst->getIPorts().size() == 4);
    REQUIRE(test_module_inst->getOPorts().size() == 4);
    REQUIRE(test_module_inst->getIOPorts().size() == 1);
    REQUIRE(test_module_inst->getSignals().size() == 4);
    REQUIRE(test_module_inst->getInputStreamPorts().size() == 0);
    REQUIRE(test_module_inst->getOutputStreamPorts().size() == 0);
    REQUIRE(test_module_inst->getOtherVars().size() == 3);

    //
    // Check port types
    //
    //
    for (auto const &port : test_module_inst->getIPorts()) {
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

      if (name == "clk") {
        REQUIRE(trim(dft_str) == "sc_in _Bool");
      }
      if ((name == "in1") || (name == "in2")) {
        REQUIRE(trim(dft_str) == "sc_in int");
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

    INFO("Checking member ports for simple module instance.");
    auto simple_module_inst{simple_module};

    // Check if the proper number of ports are found.
    REQUIRE(simple_module_inst->getIPorts().size() == 3);
    REQUIRE(simple_module_inst->getOPorts().size() == 1);
    REQUIRE(simple_module_inst->getIOPorts().size() == 0);
    REQUIRE(simple_module_inst->getSignals().size() == 0);
    REQUIRE(simple_module_inst->getOtherVars().size() == 1);
    REQUIRE(simple_module_inst->getInputStreamPorts().size() == 0);
    REQUIRE(simple_module_inst->getOutputStreamPorts().size() == 0);

    //
    // Check port types
    //
    //
    for (auto const &port : simple_module_inst->getIPorts()) {
      auto name{get<0>(port)};
      PortDecl *pd{get<1>(port)};
      auto template_type{pd->getTemplateType()};
      auto template_args{template_type->getTemplateArgTreePtr()};

      std::string dft_str{template_args->dft()};

      if (name == "clk") {
        REQUIRE(trim(dft_str) == "sc_in _Bool");
      }
      if ((name == "one") || (name == "two")) {
        REQUIRE(trim(dft_str) == "sc_in int");
      }
    }

    for (auto const &port : simple_module_inst->getOPorts()) {
      auto name{get<0>(port)};
      PortDecl *pd{get<1>(port)};
      auto template_type{pd->getTemplateType()};
      auto template_args{template_type->getTemplateArgTreePtr()};

      std::string dft_str{template_args->dft()};

      if ((name == "out_one")) {
        REQUIRE(trim(dft_str) == "sc_out int");
      }
    }

    for (auto const &ovar : simple_module_inst->getOtherVars()) {
      auto name{get<0>(ovar)};
      PortDecl *pd{get<1>(ovar)};
      auto template_type{pd->getTemplateType()};
      auto template_args{template_type->getTemplateArgTreePtr()};

      std::string dft_str{template_args->dft()};

      if ((name == "xy")) {
        REQUIRE(trim(dft_str) == "int");
      }
    }

    //
    //
    // Check netlist
    //
    //

    // test_module_inst
    auto port_bindings{test_module_inst->getPortBindings()};
    std::vector<std::string> test_ports{"in1", "in_out", "out1"};

    for (auto const &pname : test_ports) {
      auto found_it{port_bindings.find(pname)};
      // Actually found the name
      REQUIRE(found_it != port_bindings.end());
      // Check now if the names
      std::string port_name{found_it->first};
      PortBinding *binding{found_it->second};
      std::string as_string{binding->toString()};

      if (port_name == "in1") {
        REQUIRE(as_string == "test test_instance testing sig1");
      }
      if (port_name == "in_out") {
        REQUIRE(as_string == "test test_instance testing double_sig");
      }
      if (port_name == "out1") {
        REQUIRE(as_string == "test test_instance testing sig1");
      }
    }
  }
}
