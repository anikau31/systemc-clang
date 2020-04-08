#include "SystemCClang.h"
#include "catch.hpp"

#include "Matchers.h"

// This is automatically generated from cmake.
#include "ClangArgs.h"
#include "Testing.h"

using namespace scpar;
using namespace sc_ast_matchers;

TEST_CASE("Only parse a single top-level module", "[parsing]") {
  std::string code{systemc_clang::read_systemc_file(
      systemc_clang::test_data_dir, "xor-hierarchy.cpp")};

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, systemc_clang::catch_test_args)
          .release();

  // Name of top-level module declaration.
  std::string top{"exor2"};

  SystemCConsumer sc{from_ast};
  //sc.setTopModule(top);
  sc.HandleTranslationUnit(from_ast->getASTContext());
  auto model{sc.getSystemCModel()};
  auto module_decl{model->getModuleDecl()};
  //auto module_decl{model->getModuleInstanceMap()};

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
  
  auto found_module{std::find_if(module_decl.begin(), module_decl.end(),
                                 [&module_instance_name](const auto &element) {
                                   // Get the declaration name.
                                   return element.second->getName() == module_instance_name;
                                 })};

  SECTION("Testing top-level module: exor2", "[exor2]") {
    // There should be only one module.
    INFO("Top-level module specified as exor2.");
    // exor2 has several sub-modules in it.
    // Note that module_decl will just hold the three unique module declarations.
    // The instances will not be there.
    // Therefore, the total number of module decls are 3: nand2, exor.
    REQUIRE(module_decl.size() == 2);

    // Actually found the module.
    //REQUIRE(found_module_decl != nullptr); // != module_decl.end());

    //auto found_decl{found_module_decl}; //->second};
    auto found_decl{found_module->second};
    REQUIRE(found_decl->getIPorts().size() == 2);
    REQUIRE(found_decl->getOPorts().size() == 1);
    REQUIRE(found_decl->getSignals().size() == 3);
    // Other sc_module instances are recognized as others.
    REQUIRE(found_decl->getOtherVars().size() == 4);
    llvm::outs() << "All checks passed.\n";
  }
}
