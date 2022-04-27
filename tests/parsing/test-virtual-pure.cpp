#include <doctest.h>

#include "SystemCClang.h"
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

TEST_CASE("Virtual function with inheritance") {
  std::string code{systemc_clang::read_systemc_file(
      systemc_clang::test_data_dir, "test-virtual-pure-input.cpp")};

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, systemc_clang::catch_test_args)
          .release();

  /// Turn debug on
  //
  llvm::DebugFlag = false;
  // llvm::setCurrentDebugType("SensitivityMatcher");

  SystemCConsumer systemc_clang_consumer{from_ast};
  systemc_clang_consumer.HandleTranslationUnit(from_ast->getASTContext());

  auto model{systemc_clang_consumer.getSystemCModel()};

  // This provides the module declarations.
  auto instances{model->getInstances()};

  ModuleInstance *b_mod{model->getInstance("b_mod")};
  ModuleInstance *c_mod{model->getInstance("c_mod")};
  ModuleInstance *a_mod{model->getInstance("d_mod")};

  SUBCASE("Found sc_module instances") {
    INFO("Checking number of sc_module instances found: " << instances.size());

    CHECK(instances.size() == 2);

    for (const auto inst : instances) {
      llvm::dbgs() << "\n>>>> Instance called " << inst->getName()
                   << " declaration of " << inst->getInstanceDecl() << "\n";
      // inst->getModuleClassDecl()->dump();
      const CXXRecordDecl *cdecl{inst->getModuleClassDecl()};
      for (const auto &method : cdecl->methods()) {
        if (method->isVirtual()) {
          llvm::dbgs() << "Method name is " << method->getParent()->getNameAsString() << "::" << method->getNameAsString()
                       << " has body " << method->hasBody() << "\n";
          QualType qtype{method->getThisType()};

          for (const auto &ometh : method->overridden_methods()) {
            llvm::dbgs() << " -> overridden method " << ometh->getParent()->getNameAsString() << "::" << ometh->getNameAsString() << "\n";
            if (ometh->hasBody()) {
              llvm::dbgs() << " Body of overridden method " << ometh->hasBody() << "\n";
              ometh->getBody()->dump();
//
//
              // const FunctionDecl *fd {ometh->getDefinition()};
              // if (fd) { fd->dump(); }
//
            }
          }

          llvm::dbgs() << "\n" << "Body of derived method\n";
          if (method->hasBody() && method->isDefined() ) {
            //FunctionDecl * fd{method->getDefined()};
            Stmt *body{method->getBody()};
            body->dump();


          }
         qtype.getTypePtr()->dump();
        }
      }
    }


  }
}
