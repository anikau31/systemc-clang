#include "FindModule.h"
#include "FindTemplateTypes.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/Type.h"
#include "clang/Basic/SourceManager.h"

using namespace systemc_clang;

string FindModule::getModuleName() const { return module_name_; }

FindModule::FindModule(CXXRecordDecl *declaration, llvm::raw_ostream &os)
    : declaration_{declaration}, os_{os}, is_systemc_module_{false} {
  if (declaration->hasDefinition() == true) {
    TraverseDecl(declaration);
  }
}

bool FindModule::VisitCXXRecordDecl(CXXRecordDecl *declaration) {
  if (declaration_->getNumBases() <= 0) {
    return true;
  }

  // CXXRecordDecl::base_class_iterator
  for (auto bi = begin(declaration_->bases()), be = end(declaration_->bases());
       bi != be; ++bi) {
    string base_name = bi->getType().getAsString();

      if (IdentifierInfo *info = declaration_->getIdentifier()) {
        module_name_ = info->getNameStart();
      }

    if (( module_name_ != "sc_event_queue") && 
       (base_name == "::sc_core::sc_module" ||
        base_name == "sc_core::sc_module" ||
        base_name == "class sc_core::sc_module"))  {
      is_systemc_module_ = true;

    }
  }

  if (is_systemc_module_ == false) {
    return true;
  }

  return false;
}

// vector<string> FindModule::getTemplateParameters() const {
//     vector<string> parm_list;
//     if ( (template_parameters_ == nullptr)
//         || (template_parameters_->size() <= 0) ) {
//         return parm_list;
//     }

//     for (auto parm : template_parameters_->asArray() ) {
//         parm_list.push_back( parm->getName() );
//         os_ << "Parm: " << parm->getName() << "\n";
//     }
//     return parm_list;
// }

FindModule::~FindModule() { declaration_ = nullptr; }

bool FindModule::isSystemCModule() const { return is_systemc_module_; }

void FindModule::dump() {
  os_ << "\n ============== FindModule ===============";
  os_ << "\n:> module name: " << module_name_
      << ", CXXRecordDecl*: " << declaration_
      << ", isSCModule: " << is_systemc_module_;
  os_ << "\n ============== END FindModule ===============";
}
