#include "SystemCClang.h"
#include "catch.hpp"
#include "clang/Tooling/Tooling.h"

#include "FindMemberFieldMatcher.h"
#include "Matchers.h"

// This is automatically generated from cmake.
#include "ClangArgs.h"
#include "Testing.h"

using namespace clang::ast_matchers;
using namespace systemc_clang;
using namespace sc_ast_matchers;

#undef DEBUG_TYPE
#define DEBUG_TYPE "Tests"

template <typename T>
bool find_name(std::vector<T> &names, const T &find_name) {
  auto found_it = std::find(names.begin(), names.end(), find_name);
  if (found_it != names.end()) {
    names.erase(found_it);
    return true;
  }

  return false;
}

// This test works
TEST_CASE("Read SystemC model from file for testing", "[parsing]") {
  std::string code{systemc_clang::read_systemc_file(
      systemc_clang::test_data_dir, "xor-hierarchy.cpp")};

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, systemc_clang::catch_test_args)
          .release();

  LLVM_DEBUG(llvm::dbgs() << "================ TESTMATCHER =============== \n";);
  InstanceMatcher inst_matcher{};
  MatchFinder matchRegistry{};
  inst_matcher.registerMatchers(matchRegistry);
  // Run all the matchers
  matchRegistry.matchAST(from_ast->getASTContext());
  inst_matcher.dump();
  llvm::outs() << "================ END =============== \n";

  SECTION("Test instance matcher", "[instance-matcher]") {
    // There should be five instances here.
    // DUT2, n1, n2, n3, n4
    auto instances{inst_matcher.getInstanceMap()};

    REQUIRE(instances.size() == 6);

    std::vector<std::string> var_names{"dut", "d", "n1",
                                       "n2",  "n3",      "n4"};
    std::vector<std::string> var_type_names{
        "struct exor2", "struct DUT",
        "struct nand2", "struct nand2",
        "struct nand2", "struct nand2"};
    std::vector<std::string> instance_names{"exor2", "N1",
                                            "N2",    "N3",        "N4"};

    for (auto const &entry : instances) {
      auto inst{entry.second};
      inst.dump();

      find_name(var_names, inst.var_name);
      find_name(var_type_names, inst.var_type_name);
      find_name(instance_names, inst.instance_name);

      // Test findInstanceByVariableType()
      if (inst.is_field_decl) {
        auto fdecl{(cast<FieldDecl>(entry.first))};
        auto cxx_decl{fdecl->getType().getTypePtr()->getAsCXXRecordDecl()};

        std::vector<InstanceMatcher::InstanceDeclType> found_instances;
        inst_matcher.findInstanceByVariableType(cxx_decl, found_instances);

        if (inst.var_name == "dut") {
          // Find all the instances of exor2
          LLVM_DEBUG(llvm::dbgs() << "<<<< DUT\n";);
          REQUIRE(found_instances.size() == 1);
        } else {
          LLVM_DEBUG(llvm::dbgs() << "<<<< NOT DUT\n";);
          // Find all the instances of nand2
          REQUIRE(found_instances.size() == 4);
        }

        // Run the FieldMemberMatcher on it to get all the FieldDecls in it.
        sc_ast_matchers::FindMemberFieldMatcher field_matcher{};
        MatchFinder registry{};
        field_matcher.registerMatchers(registry);

        // Notice that the match() call must have the context.  We can get it
        // from the SystemCConsumer object.
        registry.match(*cxx_decl, from_ast->getASTContext());

        // Check to see if we got the FieldDecl we expect.
        std::vector<FieldDecl *> fields{field_matcher.getFieldDecls()};

        LLVM_DEBUG(llvm::dbgs() << "- number of FieldDecl in type " << fields.size()
                     << "\n";);

        for (auto const &fld : fields) {
          LLVM_DEBUG(fld->dump(););

          const Type *field_type{fld->getType().getTypePtr()};
          FindTemplateTypes find_tt{};
          find_tt.Enumerate(field_type);

          // Ge the tree.
          auto template_args{find_tt.getTemplateArgTreePtr()};
          std::string dft_str{template_args->dft()};
          LLVM_DEBUG(llvm::outs() << dft_str << "\n";);
        }
      }
    }
    // All the variable name should be found
    REQUIRE(var_names.size() == 0);
    REQUIRE(var_type_names.size() == 0);
    REQUIRE(instance_names.size() == 0);
  }
}
