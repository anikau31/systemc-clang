#include "SystemCClang.h"
#include "clang/AST/ASTImporter.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace scpar;
using namespace clang;
using namespace std;

bool SystemCConsumer::fire() {

  os_ << "Top module: " << getTopModule() << "\n";
  TranslationUnitDecl *tu{getContext().getTranslationUnitDecl()};
  // Reflection database.
  systemcModel_ = new Model{} ;

  // ANI : Do we need FindGlobalEvents?
  FindGlobalEvents globals{tu, os_};
  FindGlobalEvents::globalEventMapType eventMap{globals.getEventMap()};
  globals.dump_json();
  systemcModel_->addGlobalEvents(eventMap);

  // 
  // TODO: 
  // A first pass should be made to collect all sc_module declarations.
  // This is important so that the top-level module can be found. 
  //
  ModuleDeclarationMatcher moduleDeclarationHandler{}; 
  MatchFinder matchRegistry{};

  matchRegistry.addMatcher( matchModuleDeclarations, &moduleDeclarationHandler );
  // Run all the matchers
  matchRegistry.matchAST(getContext());

  // Check if the top-level module one of the sc_module declarations?
  //
  auto foundModules{ moduleDeclarationHandler.getFoundModuleDeclarations() };
  auto foundTopModule{ 
    std::find_if(foundModules.begin(), foundModules.end(), 
      [this]( std::tuple<std::string, CXXRecordDecl*>& element ) { 
      return std::get<0>(element) == getTopModule();  }  ) 
  }; 
  if (foundTopModule != foundModules.end()) {
    os_ << "Found the top module: " << get<0>(*foundTopModule) << ", " << get<1>(*foundTopModule) << "\n";
  } 

  // for ( auto const& element : foundModules ) {
    // systemcModel_->addModuleDecl( new ModuleDecl{element} );
    // os_ << "name: " << get<0>(element) << "\n";;
  // }
  TODO:
  // IMPORTANT: DO NOT ERASE
  // This code allows us to traverse using AST Matchers a node, and not the whole AST.
  // The approach is to import a part of the AST into another ASTUnit, and then invoke matchers on it. 
  // std::unique_ptr<ASTUnit> ToUnit = buildASTFromCode("", "to.cc");
  // ASTImporter importer(ToUnit->getASTContext(), ToUnit->getFileManager(),
  // tu->getASTContext(),ci_.getFileManager(), true );
  // llvm::Expected<Decl *> ImportedOrErr = importer.Import(node);
  //
  // if (!ImportedOrErr) {
  // llvm::Error Err = ImportedOrErr.takeError();
  // llvm::errs() << "ERROR: " << Err << "\n";
  // consumeError(std::move(Err));
  // return 1;
  // }
  //
  //
  // Decl *Imported = *ImportedOrErr;
  // Imported->getTranslationUnitDecl()->dump();
  //
  // matchRegistry.matchAST(ToUnit->getASTContext());
  
  // Find the sc_modules
  //
  // This code is no longer required. 
  // This is because we are now using AST matchers to find all the nodes, and create the
  // module declarations.
  FindSCModules scmod{tu, os_};

  FindSCModules::moduleMapType scmodules{scmod.getSystemCModulesMap()};

  for (FindSCModules::moduleMapType::iterator mit = scmodules.begin(),
         mitend = scmodules.end();  mit != mitend; ++mit) {
    ModuleDecl *md = new ModuleDecl{mit->first, mit->second};
    // md->setTemplateParameters( scmod.getTemplateParameters() );
    //       os_ << "SIZE: " << scmod.getTemplateParameters().size() << "\n";
    systemcModel_->addModuleDecl(md);

    //
    // TODO: find any instances in the module declarations
    os_ << "=> Processing module: " << mit->first << "\n";
    //md->getModuleClassDecl()->dump();
  //  FindModuleInstance module_instance{md->getModuleClassDecl(), os_};
  }


  ////////////////////////////////////////////////////////////////
  // Find the sc_main
  ////////////////////////////////////////////////////////////////
  FindSCMain scmain{tu, os_};

  if (scmain.isSCMainFound()) {
    FunctionDecl *fnDecl{scmain.getSCMainFunctionDecl()};

  // TODO: find any instances in sc_main.

    //fnDecl->dump();

    FindSimTime scstart{fnDecl, os_};
    systemcModel_->addSimulationTime(scstart.returnSimTime());
  } else {
    os_ << "\n Could not find SCMain";
  }

  ////////////////////////////////////////////////////////////////
  // Find the netlist.
  ////////////////////////////////////////////////////////////////
  FindNetlist findNetlist{scmain.getSCMainFunctionDecl()};
  findNetlist.dump();
  systemcModel_->addNetlist(findNetlist);

  ////////////////////////////////////////////////////////////////
  // Figure out the module map.
  ////////////////////////////////////////////////////////////////
  Model::moduleMapType moduleMap{systemcModel_->getModuleDecl()};

  for (Model::moduleMapType::iterator mit = moduleMap.begin(),
         mitend = moduleMap.end();   mit != mitend; mit++) {
    ModuleDecl *mainmd{mit->second};
    int numInstances{mainmd->getNumInstances()};
    vector<ModuleDecl *> moduleDeclVec;

    os_ << "\n";
    os_ << "For module: " << mit->first << " num instance : " << numInstances;

    for (unsigned int num{0}; num < numInstances; ++num) {
      ModuleDecl *md = new ModuleDecl{};

      // Find the template arguments for the class.
      FindTemplateParameters tparms{mainmd->getModuleClassDecl(), os_};

      md->setTemplateParameters( tparms.getTemplateParameters() );
      os_ << "@@# " << mainmd->getTemplateParameters().size() << "\n";
      md->dump_json();


      vector<EntryFunctionContainer *> _entryFunctionContainerVector;
      FindConstructor constructor{mainmd->getModuleClassDecl(), os_};
      md->addConstructor(constructor.returnConstructorStmt());

      FindPorts ports{mainmd->getModuleClassDecl(), os_};
      md->addInputPorts(ports.getInputPorts());
      md->addOutputPorts(ports.getOutputPorts());
      md->addInputOutputPorts(ports.getInputOutputPorts());
      md->addOtherVars(ports.getOtherVars()); 

      FindTLMInterfaces findTLMInterfaces{mainmd->getModuleClassDecl(), os_};
      md->addInputInterfaces(findTLMInterfaces.getInputInterfaces());
      md->addOutputInterfaces(findTLMInterfaces.getOutputInterfaces());
      md->addInputOutputInterfaces(
          findTLMInterfaces.getInputOutputInterfaces());

      FindSignals signals{mainmd->getModuleClassDecl(), os_};
      md->addSignals(signals.getSignals());

      FindEntryFunctions findEntries{mainmd->getModuleClassDecl(), os_};
      FindEntryFunctions::entryFunctionVectorType *entryFunctions{
          findEntries.getEntryFunctions()};
      md->addProcess(entryFunctions);

      for (size_t i = 0; i < entryFunctions->size(); i++) {
        EntryFunctionContainer *ef{(*entryFunctions)[i]};
        FindSensitivity findSensitivity{constructor.returnConstructorStmt(),
                                        os_};
        ef->addSensitivityInfo(findSensitivity);

        if (ef->getEntryMethod() == nullptr) {
          os_ << "ERROR";
          continue;
        }

        FindWait findWaits{ef->getEntryMethod(), os_};
        ef->addWaits(findWaits);

        FindNotify findNotify{ef->_entryMethodDecl, os_};
        ef->addNotifys(findNotify);

  #ifdef USE_SAUTO 
        /// Does not compile
        SuspensionAutomata suspensionAutomata(findWaits.getWaitCalls(),
                                              ef->getEntryMethod(), &getContext(),
                                              llvm::errs());
        if (suspensionAutomata.initialize()) {
          suspensionAutomata.genSusCFG();
          suspensionAutomata.dumpSusCFG();
          suspensionAutomata.genSauto();
          suspensionAutomata.dumpSauto();
          ef->addSusCFGAuto(suspensionAutomata);
        }
#endif 

        _entryFunctionContainerVector.push_back(ef);
      }
      moduleDeclVec.push_back(md);
    }
    systemcModel_->addModuleDeclInstances(mainmd, moduleDeclVec);
  }

  /*
FindSCMain scmain(tu, os_);

if (scmain.isSCMainFound())
{
          FunctionDecl *fnDecl = scmain.getSCMainFunctionDecl();

          FindSimTime scstart(fnDecl, os_);
          systemcModel_->addSimulationTime(scstart.returnSimTime());

}
else {
          os_ <<"\n Could not find SCMain";
}

FindNetlist findNetlist(scmain.getSCMainFunctionDecl());
findNetlist.dump();
systemcModel_->addNetlist(findNetlist);
*/

// Only do this if SAUTO flag is set.
#ifdef USE_SAUTO
  // Generate SAUTO
  // Placing it here so that unique SAUTO for each instance
  // Model::moduleMapType moduleMap = systemcModel_->getModuleDecl();
  Model::moduleInstanceMapType moduleInstanceMap =
      systemcModel_->getModuleInstanceMap();

  for (Model::moduleInstanceMapType::iterator it = moduleInstanceMap.begin(),
                                              eit = moduleInstanceMap.end();
       it != eit; it++) {
    vector<ModuleDecl *> moduleDeclVec = it->second;
    for (size_t i = 0; i < moduleDeclVec.size(); i++) {
      ModuleDecl *moduleDecl = moduleDeclVec.at(i);
      vector<EntryFunctionContainer *> entryFunctionContainer =
          moduleDecl->getEntryFunctionContainer();
      for (size_t j = 0; j < entryFunctionContainer.size(); j++) {

        SuspensionAutomata suspensionAutomata(
            entryFunctionContainer.at(j)->getWaitCalls(),
            entryFunctionContainer.at(j)->getEntryMethod(), &getContext(),
            llvm::errs());
        if (suspensionAutomata.initialize()) {
          suspensionAutomata.genSusCFG();
          //suspensionAutomata.dumpSusCFG();
          suspensionAutomata.genSauto();
          //suspensionAutomata.dumpSauto();
          entryFunctionContainer.at(j)->addSusCFGAuto(suspensionAutomata);
        }
      }
    }
  } 
#endif 

  os_ << "\n";
  os_ << "\n## SystemC model\n";
  systemcModel_->dump(os_);
  return true;
}

void SystemCConsumer::HandleTranslationUnit(ASTContext &context) {
  // ///////////////////////////////////////////////////////////////
  // / Pass 1: Find the necessary information.
  // ///////////////////////////////////////////////////////////////

  fire(); 
}

SystemCConsumer::SystemCConsumer( CompilerInstance &ci, std::string top )
  : os_{llvm::errs()}, sm_{ci.getSourceManager()},
  context_{ci.getASTContext()},
  ci_{ci},
  top_{top},
  systemcModel_{nullptr} {}

  SystemCConsumer::~SystemCConsumer() {
  if (systemcModel_ != nullptr) {
    delete systemcModel_;
    systemcModel_ = nullptr;
  }
}

Model *SystemCConsumer::getSystemCModel() { return systemcModel_; }

std::string SystemCConsumer::getTopModule() const { return top_; }

ASTContext& SystemCConsumer::getContext() const { return context_; }
  
SourceManager& SystemCConsumer::getSourceManager() const { return sm_; }
