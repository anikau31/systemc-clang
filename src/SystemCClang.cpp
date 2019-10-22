#include "SystemCClang.h"
#include "clang/AST/ASTImporter.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace scpar;
using namespace clang;
using namespace std;

using namespace sc_ast_matchers;
bool SystemCConsumer::fire() {

  os_ << "Top module: " << getTopModule() << "\n";
  TranslationUnitDecl* translation_unit{getContext().getTranslationUnitDecl()};
  // Reflection database.
  systemc_model_ = new Model{} ;

  // ANI : Do we need FindGlobalEvents?
  FindGlobalEvents globals{translation_unit, os_};
  FindGlobalEvents::globalEventMapType event_map{globals.getEventMap()};
  globals.dump_json();
  systemc_model_->addGlobalEvents( event_map );

  // 
  // TODO: 
  // A first pass should be made to collect all sc_module declarations.
  // This is important so that the top-level module can be found. 
  //
  os_ <<"================ TESTMATCHER =============== \n";
  ModuleDeclarationMatcher module_declaration_handler{}; 
  MatchFinder matchRegistry{};
  module_declaration_handler.registerMatchers( matchRegistry );
  // Run all the matchers
  matchRegistry.matchAST(getContext());
  os_ <<"================ END =============== \n";

  // Check if the top-level module one of the sc_module declarations?
  //
  auto found_modules{ module_declaration_handler.getFoundModuleDeclarations() };
  auto found_top_module{ 
    std::find_if(found_modules.begin(), found_modules.end(), 
        [this]( std::tuple<std::string, CXXRecordDecl*>& element ) { 
        return std::get<0>(element) == getTopModule();  }  ) 
  }; 
  if (found_top_module != found_modules.end()) {
    os_ << "Found the top module: " << get<0>(*found_top_module) << ", " << get<1>(*found_top_module) << "\n";
  } 

  // =========================================================== 
  // 1. Add every found module to the model. 
  // =========================================================== 
  // Every module that is found should be added to the model.
  // We could do this in the AST matcher actually. 
  for ( auto const& element : found_modules ) {
    auto module_declaration{ new ModuleDecl{get<0>(element), get<1>(element)} };
    os_ << "name: " << get<0>(element) << "\n";;

    // =========================================================== 
    // 2. Traverse every module, and get its member declarations.
    // =========================================================== 
    //
    // This can be replaced with matchers, where all fields are 
    // recognized in the matchers.

    //
    //  Find the entry functions.
    //
    FindConstructor constructor{ module_declaration->getModuleClassDecl(), os_ };
    module_declaration->addConstructor( constructor.returnConstructorStmt() );

    //  
    //  Find the ports
    //
    FindPorts found_ports{ module_declaration->getModuleClassDecl(), os_ };
    module_declaration->addInputPorts( found_ports.getInputPorts() );
    module_declaration->addOutputPorts( found_ports.getOutputPorts() );
    module_declaration->addInputOutputPorts( found_ports.getInputOutputPorts() );
    module_declaration->addOtherVars( found_ports.getOtherVars() ); 

    os_ << "Number of found ports" ;
    found_ports.dump();
  //  module_declaration->dumpPorts(os_, 4);
    // 
    // Find the sc_signals within the module. 
    //
    FindSignals signals{ module_declaration->getModuleClassDecl(), os_ };
    module_declaration->addSignals( signals.getSignals() );

    // 
    // Find the entry functions (SC_METHOD should be the only one really)
    //
    FindEntryFunctions find_entries{ module_declaration->getModuleClassDecl(), os_ };
    auto entry_functions{ find_entries.getEntryFunctions() };
    module_declaration->addProcess( entry_functions );

    // For every entry function, figure out its sensitivity list. 
    for ( size_t i{0}; i < entry_functions->size(); ++i ) {
      EntryFunctionContainer *ef{ (*entry_functions)[i] };

      if (ef->getEntryMethod() == nullptr) { os_ << "ERROR"; }

      FindSensitivity find_sensitivity{ constructor.returnConstructorStmt(), os_ };
      ef->addSensitivityInfo( find_sensitivity );
    }

    // Add into the model.
    systemc_model_->addModuleDecl( module_declaration );
  }

  //
  // Print the contents of the model so far.  It should only have declarations.
  //
  os_ << "[Pass 1]: Discover sc_module declarations.\n";
  auto module_decls_sofar { systemc_model_->getModuleDecl() };
  for ( auto md : module_decls_sofar ) {
    os_ << "[ module name ] " << md.first << "\n";
    auto md_ptr { md.second };
    md_ptr->dump( os_ );

  }


  // TODO:
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
  //
  // FindSCModules scmod{tu, os_};
  //
  // FindSCModules::moduleMapType scmodules{scmod.getSystemCModulesMap()};
  //
  // for (FindSCModules::moduleMapType::iterator mit = scmodules.begin(),
  // mitend = scmodules.end();  mit != mitend; ++mit) {
  // ModuleDecl *md = new ModuleDecl{mit->first, mit->second};
  //  md->setTemplateParameters( scmod.getTemplateParameters() );
  //       os_ << "SIZE: " << scmod.getTemplateParameters().size() << "\n";
  // systemc_model_->addModuleDecl(md);
  //

  //TODO: find any instances in the module declarations
  // os_ << "=> Processing module: " << mit->first << "\n";
  //   md->getModuleClassDecl()->dump();
  //  FindModuleInstance module_instance{md->getModuleClassDecl(), os_};
  // }
  //
  // You only need to look for the main() if a topModule is not found.  
  // So, the CXXRecordDecl to be passed to the next stage should be topModule if found. 

  //
  // =========================================================== 
  // 3. We have gone over all the sc_module declarations now.  
  // =========================================================== 
  // We can start to look for instances.  If a top module is specified 
  // then we can start looking for instances there; otherwise, we look for 
  // them in the main.
  //

  ////////////////////////////////////////////////////////////////
  // Find the sc_main
  ////////////////////////////////////////////////////////////////
  //

  // There was no top-level module found or specified.

  if (found_top_module == found_modules.end()) {
    FindSCMain scmain{translation_unit, os_};

    if (scmain.isSCMainFound()) {
      FunctionDecl *fnDecl{scmain.getSCMainFunctionDecl()};

      // TODO: find any instances in sc_main.

      FindSimTime scstart{fnDecl, os_};
      systemc_model_->addSimulationTime(scstart.returnSimTime());
      ////////////////////////////////////////////////////////////////
      // Find the netlist.
      ////////////////////////////////////////////////////////////////
      FindNetlist find_netlist{scmain.getSCMainFunctionDecl()};
      find_netlist.dump();
      systemc_model_->addNetlist( find_netlist );


    } else {
      os_ << "\n Could not find SCMain";
    }
  } else {
    // There is a top-module specified.

    // Get the functionDecl for the top-level module's function where
    // the modules are bound.  
    // TODO: Need an example of this.
    //
    // FindNetlist find_netlist{scmain.getSCMainFunctionDecl()};
    // find_netlist.dump();
    // systemc_model_->addNetlist( find_netlist );
  }

  ////////////////////////////////////////////////////////////////
  // Figure out the module map.
  ////////////////////////////////////////////////////////////////
  Model::moduleMapType moduleMap{systemc_model_->getModuleDecl()};

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
        FindSensitivity findSensitivity{constructor.returnConstructorStmt(), os_};
        ef->addSensitivityInfo(findSensitivity);

        if (ef->getEntryMethod() == nullptr) {
          os_ << "ERROR";
          continue;
        }

        FindWait findWaits{ef->getEntryMethod(), os_};
        ef->addWaits(findWaits);

        FindNotify findNotify{ef->_entryMethodDecl, os_};
        ef->addNotifys(findNotify);
        _entryFunctionContainerVector.push_back(ef);
      }
      moduleDeclVec.push_back(md);
    }
    systemc_model_->addModuleDeclInstances(mainmd, moduleDeclVec);
  }
  os_ << "\n";
  os_ << "\n## SystemC model\n";
  systemc_model_->dump(os_);
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
  systemc_model_{nullptr} {}

  SystemCConsumer::~SystemCConsumer() {
    if (systemc_model_ != nullptr) {
      delete systemc_model_;
      systemc_model_ = nullptr;
    }
  }

Model *SystemCConsumer::getSystemCModel() { return systemc_model_; }

std::string SystemCConsumer::getTopModule() const { return top_; }

ASTContext& SystemCConsumer::getContext() const { return context_; }

SourceManager& SystemCConsumer::getSourceManager() const { return sm_; }
