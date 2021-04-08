#include "SystemCClang.h"

#include "matchers/FindConstructor.h"
#include "matchers/FindEntryFunctions.h"
#include "matchers/FindEvents.h"
#include "matchers/FindGlobalEvents.h"
#include "matchers/FindNetlist.h"
#include "matchers/FindNotify.h"
#include "matchers/FindSCMain.h"
#include "matchers/FindSimTime.h"
#include "matchers/FindTLMInterfaces.h"
#include "matchers/FindTemplateParameters.h"
#include "matchers/FindWait.h"

#include "matchers/Matchers.h"
#include "ModuleInstanceType.h"
#include "matchers/NetlistMatcher.h"
#include "matchers/SensitivityMatcher.h"

//#include "model/Model.h"

#include "clang/AST/ASTImporter.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"

using namespace systemc_clang;

using namespace sc_ast_matchers;

bool SystemCConsumer::preFire() { return true; }

bool SystemCConsumer::postFire() { return true; }

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

  // MultiMap CXXRecordDecl => ModuleInstance*
  // Instances with same CXXRecordDecl will have multiple entries
  auto found_module_declarations{
      module_declaration_handler.getFoundModuleDeclarations()};
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

  llvm::outs()
      << "############# ============= NEW FIRE ============ ################\n";
  llvm::outs() << "Size of module instances: "
               << found_module_declarations.size() << "\n";

  for (const auto &inst : found_module_declarations) {
    auto cxx_decl{inst.first};
    ModuleInstance *add_module_decl{inst.second};

    // setInstanceInfo done in pruneMatches
    //
    FindTemplateParameters tparms{cxx_decl};
    add_module_decl->setTemplateParameters(tparms.getTemplateParameters());
    add_module_decl->setTemplateArgs(tparms.getTemplateArgs());

    /// 3. Find constructor
    //
    //
    LLVM_DEBUG(llvm::dbgs() << "4. Set the constructor.\n";);
    vector<EntryFunctionContainer *> _entryFunctionContainerVector;
    FindConstructor constructor{add_module_decl->getModuleClassDecl(), os_};
    add_module_decl->addConstructor(&constructor);

    /// 4. Find ports
    /// This is done for the declaration.
    //
    //
    // 5. Find  entry functions within one sc_module.
    LLVM_DEBUG(llvm::dbgs() << "5. Set the entry functions\n";);
    FindEntryFunctions findEntries{add_module_decl->getModuleClassDecl(), os_};
    FindEntryFunctions::entryFunctionVectorType *entryFunctions{
        findEntries.getEntryFunctions()};
    LLVM_DEBUG(llvm::dbgs() << "6. Set the process\n";);
    add_module_decl->addProcess(entryFunctions);

    SensitivityMatcher sens_matcher{};
    MatchFinder matchRegistry{};
    sens_matcher.registerMatchers(matchRegistry);
    matchRegistry.match(*constructor.getConstructorDecl(), getContext());

    for (size_t i{0}; i < entryFunctions->size(); i++) {
      EntryFunctionContainer *ef{(*entryFunctions)[i]};

      /// Add the sensitivity information to each of the entry functions.
      EntryFunctionContainer::SenseMapType sensitivity_info{
          sens_matcher.getSensitivityMap()};
      ef->addSensitivityInfo(sensitivity_info);

      if (ef->getEntryMethod() == nullptr) {
        LLVM_DEBUG(llvm::dbgs() << "ERROR";);
        continue;
      }

      FindWait findWaits{ef->getEntryMethod(), os_};
      ef->addWaits(findWaits);

      FindNotify findNotify{ef->getEntryMethod(), os_};
      ef->addNotifys(findNotify);

      _entryFunctionContainerVector.push_back(ef);
    }

    systemc_model_->addInstance(add_module_decl);

    //
  }
  llvm::outs()
      << "############# =====  END NEW FIRE ============ ################\n";

  LLVM_DEBUG(llvm::dbgs() << " @@@@@@@@ =============== Populate sub-modules "
                             "============= \n";);
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

  // All instances are within the SystemC model.
  //  This must come after instances of ModuleInstance have been generated.
  //  This is because the netlist matcher inserts the port bindings into the
  //  instance.

  LLVM_DEBUG(
      llvm::dbgs() << "=============== ##### TEST NetlistMatcher ##### \n";);
  NetlistMatcher netlist_matcher{};
  MatchFinder netlist_registry{};
  netlist_matcher.registerMatchers(netlist_registry, systemc_model_,
                                   &module_declaration_handler);

  netlist_registry.match(*scmain.getSCMainFunctionDecl(), getContext());

  auto instances{systemc_model_->getInstances()};
  for (const auto &inst : instances) {
    ModuleInstance *mdecl{inst};
    auto ctordecl{mdecl->getConstructorDecl()};
    if (ctordecl != nullptr) {
      const FunctionDecl *fd{dyn_cast<FunctionDecl>(ctordecl)};
      ctordecl->getBody(fd);

      LLVM_DEBUG(llvm::dbgs() << "==============> RUN netlist matcher: "
                              << mdecl->getInstanceName() << "\n";);
      //fd->dump();
      netlist_registry.match(*fd, getContext());
      LLVM_DEBUG(llvm::dbgs() << "==============> DONE netlist matcher\n";);
      //}
    }
  }
  LLVM_DEBUG(netlist_matcher.dump();
             llvm::dbgs() << "##### END TEST NetlistMatcher ##### \n";);

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
