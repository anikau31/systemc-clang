#include "SystemCClang.h"
#include "catch.hpp"

// This is automatically generated from cmake.
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
#include <systemc.h>
SC_MODULE(counter) {
  // clock
  sc_in_clk clk;

  // output port
  sc_out<sc_uint<32>> count_out;

  // member variable
  sc_uint<32> keep_count;

  SC_CTOR(counter) {
    SC_METHOD(count_up);
    sensitive << clk.pos();
  }

  void count_up() {
    keep_count = keep_count + 1;
    count_out.write( keep_count );
  }

};

// Top level module.
SC_MODULE(DUT) {
  counter count;
  sc_clock clock;
  sc_signal< sc_uint<32> > counter_out;

  SC_CTOR(DUT): count{"counter_instance"} {
    // port bindings
    count.clk(clock);
    count.count_out(counter_out);
  };
};

int sc_main() {
  DUT dut{"design_under_test"};
  return 0;
};
     )";

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, systemc_clang::catch_test_args)
          .release();

  SystemCConsumer consumer{from_ast};
  consumer.HandleTranslationUnit(from_ast->getASTContext());

  Model *model{consumer.getSystemCModel()};

  // This provides the module declarations.
  auto instances{model->getInstances()};

  // Want to find an instance named "counter_instance".
  ModuleInstance *test_module{model->getInstance("counter_instance")};

  SECTION("Found sc_module instances", "[instances]") {
    // There should be 2 modules identified.
    INFO("Checking number of sc_module instances found: "
         << instances.size());

    // DUT and 
    REQUIRE(instances.size() == 2);

    REQUIRE(test_module != nullptr);

    INFO("Checking clock port parsing.");
    // These checks should be performed on the declarations.

    ModuleInstance *test_module_inst{test_module};

    // Check if the proper number of ports are found.
    //
    // There is only one input port seen as sc_in<bool> clk;
    auto input_ports{test_module_inst->getIPorts()};
    REQUIRE(input_ports.size() == 1);

    auto output_ports{test_module_inst->getOPorts()};
    REQUIRE(output_ports.size() == 1);
    /*
    // Try to access each of the ports
    // // Iterate over all ports and their arguments.
    for (const auto &port : input_ports) {
      auto name = get<0>(port);
      llvm::outs() << "name: " << name << "\n";
      PortDecl *pd{get<1>(port)};
      FindTemplateTypes *template_type{pd->getTemplateType()};
      Tree<TemplateType> *template_args{template_type->getTemplateArgTreePtr()};

      std::string dft_str{template_args->dft()};

      if ((name == "bool_clk") || (name == "clk"))
        REQUIRE(trim(dft_str) == "sc_in _Bool");

         }

    REQUIRE(test_module_inst->getOPorts().size() == 0);
    REQUIRE(test_module_inst->getIOPorts().size() == 0);
    REQUIRE(test_module_inst->getSignals().size() == 0);
    REQUIRE(test_module_inst->getOtherVars().size() == 0);
    REQUIRE(test_module_inst->getInputStreamPorts().size() == 0);
    REQUIRE(test_module_inst->getOutputStreamPorts().size() == 0);

    auto port_bindings{test_module_inst->getPortBindings()};
    REQUIRE(port_bindings.size() == 0);
    */
  }
}
