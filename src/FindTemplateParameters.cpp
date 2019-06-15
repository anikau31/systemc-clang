#include "FindTemplateParameters.h"
#include "FindTemplateTypes.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/Type.h"
#include "clang/Basic/SourceManager.h"

using namespace scpar;

FindTemplateParameters::FindTemplateParameters(CXXRecordDecl *declaration, llvm::raw_ostream &os)
    : declaration_{declaration}, os_{os}, template_parameters_{nullptr} {
  if (declaration->hasDefinition() == true) {
    TraverseDecl(declaration);
  }
}

bool FindTemplateParameters::VisitCXXRecordDecl(CXXRecordDecl *declaration) {
    if (IdentifierInfo *info = declaration_->getIdentifier()) {
        auto module_name = info->getNameStart();

        // Check if the class is a templated module class.
        auto template_args{ declaration->getDescribedClassTemplate() };
        if (template_args != nullptr) {
            template_parameters_ = template_args->getTemplateParameters();

        }
    }

    return false;
}

vector<string> FindTemplateParameters::getTemplateParameters() const {
    vector<string> parm_list;
    if ( (template_parameters_ == nullptr)
        || (template_parameters_->size() <= 0) ) {
        return parm_list;
    }

    for (auto parm : template_parameters_->asArray() ) {
        parm_list.push_back( parm->getName() );
//        os_ << "Parm: " << parm->getName() << "\n";
    }
    return parm_list;
}

FindTemplateParameters::~FindTemplateParameters() { declaration_ = nullptr; }

void FindTemplateParameters::dump() {

}
