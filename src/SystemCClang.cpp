#include "SystemCClang.h"

#include "matchers/FindSimTime.h"

#include "ModuleInstanceType.h"
#include "matchers/NetlistMatcher.h"
#include "matchers/SensitivityMatcher.h"
#include "utils/CXXRecordDeclUtils.h"
//#include "model/Model.h"

#include "clang/AST/ASTImporter.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"

using namespace systemc_clang;

using namespace sc_ast_matchers;
using namespace sc_ast_matchers::utils;

bool SystemCConsumer::preFire() { return true; }

bool SystemCConsumer::postFire() { return true; }

void SystemCConsumer::processNetlist(
    FindSCMain *scmain, ModuleDeclarationMatcher *module_declaration_handler) {
  LLVM_DEBUG(
      llvm::dbgs() << "=============== ##### TEST NetlistMatcher ##### \n";);
  NetlistMatcher netlist_matcher{};
  MatchFinder netlist_registry{};
  netlist_matcher.registerMatchers(netlist_registry, systemc_model_,
                                   module_declaration_handler);

  netlist_registry.match(*(scmain->getSCMainFunctionDecl()), getContext());

  auto instances{systemc_model_->getInstances()};
  for (const auto &inst : instances) {
    ModuleInstance *mdecl{inst};
    auto ctordecl{mdecl->getConstructorDecl()};
    if (ctordecl != nullptr) {
      const FunctionDecl *fd{dyn_cast<FunctionDecl>(ctordecl)};
      ctordecl->getBody(fd);

      LLVM_DEBUG(llvm::dbgs() << "==============> RUN netlist matcher: "
                              << mdecl->getInstanceName() << "\n";);
      // fd->dump();
      netlist_registry.match(*fd, getContext());
      LLVM_DEBUG(llvm::dbgs() << "==============> DONE netlist matcher\n";);
      //}
    }
  }
  LLVM_DEBUG(netlist_matcher.dump();
             llvm::dbgs() << "##### END TEST NetlistMatcher ##### \n";);
}

bool SystemCConsumer::fire() {
  clang::TranslationUnitDecl *tu{getContext().getTranslationUnitDecl()};
  // Reflection database.
  systemc_model_ = new Model{};

  FindGlobalEvents globals{tu, os_};
  FindGlobalEvents::globalEventMapType eventMap{globals.getEventMap()};
  systemc_model_->addGlobalEvents(eventMap);

  ModuleDeclarationMatcher module_declaration_handler{};
  MatchFinder matchRegistry{};
  module_declaration_handler.registerMatchers(matchRegistry);
  matchRegistry.matchAST(getContext());
  module_declaration_handler.processInstanceCXXDecls(getContext());

  LLVM_DEBUG(llvm::dbgs() << "== Print module declarations pruned\n";
             module_declaration_handler.dump();
             llvm::dbgs() << "================ END =============== \n";);
  ////////////////////////////////////////////////////////////////
  // Find the sc_main
  ////////////////////////////////////////////////////////////////
  FindSCMain scmain{tu, os_};

  if (scmain.isSCMainFound()) {
    clang::FunctionDecl *fnDecl{scmain.getSCMainFunctionDecl()};

    // TODO: find any instances in sc_main.

    FindSimTime scstart{fnDecl, os_};
    systemc_model_->addSimulationTime(scstart.returnSimTime());
  } else {
    LLVM_DEBUG(llvm::dbgs() << "\n Could not find SCMain";);
  }
  //
  // Create a ModuleInstance for each instance with the appropriately parsed
  // ModuleInstance.
  //

  // MultiMap CXXRecordDecl => ModuleInstance*
  // Instances with same CXXRecordDecl will have multiple entries
  auto found_module_declarations{
      module_declaration_handler.getFoundModuleDeclarations()};

  LLVM_DEBUG(llvm::dbgs() << "############# ============= NEW FIRE "
                             "============ ################\n";
             llvm::dbgs() << "Size of module instances: "
                          << found_module_declarations.size() << "\n";);

  for (const auto &inst : found_module_declarations) {
    clang::CXXRecordDecl *cxx_decl{inst.first};
    ModuleInstance *add_module_decl{inst.second};

    /// Process the module declaration first.
    // processModuleDeclaration(cxx_decl, add_module_decl);

    /// Process the base classes for the module declaration.
    // auto base_decls{getAllBaseClasses(cxx_decl)};
    // for (const auto &base: base_decls) {
    //  auto name{ base->getNameAsString() };
    //  ModuleInstance *base_module_instance{ new ModuleInstance{name, base} };
    // processModuleDeclaration(const_cast<clang::CXXRecordDecl*>(base),
    // base_module_instance); llvm::dbgs() << " ############### Base module
    // instance ################# \n"; add_module_decl->addBaseInstance(
    // base_module_instance);

    /*
for (const auto &base_decl : base_decls) {
      llvm::dbgs() << "=============================== BASES " <<
decl->getNameAsString() << " =======================\n"; llvm::dbgs() << "Run
base instance matcher: "
                   << base_decl->getNameAsString() << " \n";
      InstanceMatcher base_instance_matcher;
      MatchFinder base_instance_reg{};
      base_instance_matcher.registerMatchers(base_instance_reg);
      base_instance_matcher.setParentFieldDecl(vd);
      base_instance_reg.match(*base_decl, context);
      llvm::dbgs() << "+ Dump base instance matcher\n";
      base_instance_matcher.dump();
      llvm::dbgs() << "+ End dump base instance matcher\n";

      /// Copy contents over.
      instance_matcher_= base_instance_matcher;
    }

*/

    //}

    systemc_model_->addInstance(add_module_decl);
  }

  LLVM_DEBUG(
      llvm::dbgs() << "############# =====  END NEW FIRE ============ "
                      "################\n";

      llvm::dbgs() << "=============== Populate sub-modules ============= \n";);
  // This must have the instance matcher already run.
  // You need systemc_model_ and instance_matcher to build the hierarchy of
  // sub-modules.
  auto instance_matcher{module_declaration_handler.getInstanceMatcher()};
  auto instance_map{instance_matcher.getInstanceMap()};

  LLVM_DEBUG(
      llvm::dbgs() << "- Print out all the instances in the instance map\n";);

  systemc_model_->populateNestedModules();
  LLVM_DEBUG(
      llvm::dbgs() << "===========END  Populate sub-modules ============= \n";);

  // Netlist matcher code
  //
  // All instances are within the SystemC model.
  //  This must come after instances of ModuleInstance have been generated.
  //  This is because the netlist matcher inserts the port bindings into the
  //  instance.

  processNetlist(&scmain, &module_declaration_handler);

  LLVM_DEBUG(
      llvm::dbgs() << "\nParsed SystemC model from systemc-clang\n";
      llvm::dbgs() << "============= MODEL ============================\n";
      systemc_model_->dump(llvm::dbgs());
      llvm::dbgs() << "==============END========================\n";);
  return true;
}

void SystemCConsumer::HandleTranslationUnit(clang::ASTContext &context) {
  // ///////////////////////////////////////////////////////////////
  // / Pass 1: Find the necessary information.
  // ///////////////////////////////////////////////////////////////

  bool pre = false;
  pre = preFire();

  if (!pre) {
    return;
  }

  bool f = false;
  f = fire();

  if (!f) {
    return;
  }
  postFire();
}

SystemCConsumer::SystemCConsumer(clang::ASTUnit *from_ast, std::string top)
    : os_{llvm::errs()},
      sm_{from_ast->getSourceManager()},
      context_{from_ast->getASTContext()},
      top_{top},
      systemc_model_{nullptr} {}

SystemCConsumer::SystemCConsumer(clang::CompilerInstance &ci, std::string top)
    : os_{llvm::errs()},
      sm_{ci.getSourceManager()},
      context_{ci.getASTContext()},
      top_{top},
      systemc_model_{nullptr} {}

SystemCConsumer::~SystemCConsumer() {
  if (systemc_model_ != nullptr) {
    delete systemc_model_;
    systemc_model_ = nullptr;
  }
}

void SystemCConsumer::setTopModule(const std::string &top_module_decl) {
  top_ = top_module_decl;
}

Model *SystemCConsumer::getSystemCModel() { return systemc_model_; }

const std::string &SystemCConsumer::getTopModule() const { return top_; }

clang::ASTContext &SystemCConsumer::getContext() const { return context_; }

clang::SourceManager &SystemCConsumer::getSourceManager() const { return sm_; }
