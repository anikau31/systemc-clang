#include "catch.hpp"

#include "SystemCClang.h"

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
#include "systemc.h"

SC_MODULE( test ){

  // input ports
  sc_in_clk clk;
  sc_in<bool> bool_clk;

  void entry_function_1() {
    while(true) {
    }
  }
  SC_CTOR( test ) {
    SC_METHOD(entry_function_1);
    sensitive << clk.pos();
  }
};


int sc_main(int argc, char *argv[]) {
  test test_instance("testing");

  return 0;
}
     )";

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, systemc_clang::catch_test_args)
          .release();

  SystemCConsumer sc{from_ast};
  sc.HandleTranslationUnit(from_ast->getASTContext());

  auto model{sc.getSystemCModel()};

  // This provides the module declarations.
  auto instances{model->getInstances()};
  auto module_instance_map{model->getModuleInstanceMap()};

  // Want to find an instance named "testing".

  ModuleDecl *test_module{model->getInstance("testing")};

  SECTION("Found sc_module instances", "[instances]") {
    // There should be 2 modules identified.
    INFO("Checking number of sc_module declarations found: "
         << instances.size());

    REQUIRE(instances.size() == 1);

    REQUIRE(test_module != nullptr);

    INFO("Checking clock port parsing.");
    // These checks should be performed on the declarations.

    ModuleDecl *test_module_inst{test_module};

    // Check if the proper number of ports are found.
    //
    // There is only one input port seen as sc_in<bool> clk;
    auto input_ports{test_module_inst->getIPorts()};
    REQUIRE(input_ports.size() == 2);

    // Try to access each of the ports    
    // // Iterate over all ports and their arguments.
    for (const auto &port : input_ports) {
      auto name = get<0>(port);
      llvm::outs() << "name: " << name << "\n";
      PortDecl *pd{get<1>(port)};
      FindTemplateTypes *template_type{pd->getTemplateType()};
      Tree<TemplateType> *template_args{template_type->getTemplateArgTreePtr()};

      std::string dft_str{template_args->dft()};

      if ((name == "bool_clk") || (name == "clk")) REQUIRE(trim(dft_str) == "sc_in _Bool");

      /*
      if (name == "bool_clk" ) {
        REQUIRE((template_args[0].getTypeName() == "sc_in"));
        REQUIRE((template_args[1].getTypeName() == "_Bool"));
      }

      if (name == "clk" ) {
        REQUIRE((template_args[0].getTypeName() == "sc_in"));
        REQUIRE((template_args[1].getTypeName() == "_Bool"));
      }
      */

    }
    
    REQUIRE(test_module_inst->getOPorts().size() == 0);
    REQUIRE(test_module_inst->getIOPorts().size() == 0);
    REQUIRE(test_module_inst->getSignals().size() == 0);
    REQUIRE(test_module_inst->getOtherVars().size() == 0);
    REQUIRE(test_module_inst->getInputStreamPorts().size() == 0);
    REQUIRE(test_module_inst->getOutputStreamPorts().size() == 0);


    auto port_bindings{test_module_inst->getPortBindings()};
    REQUIRE(port_bindings.size() == 0 );
  }
}
