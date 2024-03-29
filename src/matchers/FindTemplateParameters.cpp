#include "FindTemplateParameters.h"
#include "FindTemplateTypes.h"

#include "llvm/Support/Debug.h"
using namespace systemc_clang;
using namespace clang;

FindTemplateParameters::FindTemplateParameters(CXXRecordDecl *declaration)
    : declaration_{declaration},
      template_parameters_{nullptr},
      template_args_{nullptr} {
  if (declaration->hasDefinition() == true) {
    TraverseDecl(declaration);
  }
}

bool FindTemplateParameters::VisitCXXRecordDecl(CXXRecordDecl *declaration) {
  if (IdentifierInfo *info = declaration_->getIdentifier()) {
    auto module_name = info->getNameStart();
    // Check if the class is a templated module class.

    // This will only be called on instances of template specialized classes (so
    // it seems). This should provide to the actual template arguments. For
    // example, int.

    if (const auto tdecl =
            dyn_cast<ClassTemplateSpecializationDecl>(declaration)) {
      template_args_ = &tdecl->getTemplateArgs();
      LLVM_DEBUG(llvm::dbgs()
                     << "@@ template specialization args: " << module_name
                     << ", #args: " << template_args_->size() << "\n";);
      for (size_t i{0}; i < template_args_->size(); ++i) {
        // Check the kind of the argument.
        switch (template_args_->get(i).getKind()) {
          case TemplateArgument::ArgKind::Integral: {
            auto q{template_args_->get(i).getAsIntegral()};
            // auto name{q.getAsString()};
            LLVM_DEBUG(llvm::dbgs() << "@@ Integral: " << q << "\n";);
          }; break;
          case TemplateArgument::ArgKind::Type: {
            auto q{template_args_->get(i).getAsType()};
            auto name{q.getAsString()};
            LLVM_DEBUG(llvm::dbgs() << "@@ arg: " << name << "\n";);
          }; break;
          default: {
          }
        };
      }
    }

    // TODO: I'm not sure if this is required since the above should capture
    // sc_in declarations too.  But, I'll have to add a test for it.
    // This will provide access to the actual template parameters
    auto template_args{declaration->getDescribedClassTemplate()};
    if (template_args != nullptr) {
      LLVM_DEBUG(llvm::dbgs() << "@@ template described class args: "
                              << module_name << "\n";);
      template_parameters_ = template_args->getTemplateParameters();
    }
  }

  return false;
}

const std::vector<std::string> FindTemplateParameters::getTemplateParameters()
    const {
  std::vector<std::string> parm_list;
  if ((template_parameters_ == nullptr) ||
      (template_parameters_->size() <= 0)) {
    return parm_list;
  }

  for (auto parm : template_parameters_->asArray()) {
    parm_list.push_back(parm->getName().str());
    LLVM_DEBUG(llvm::dbgs() << "Parm: " << parm->getName() << "\n";);
  }
  return parm_list;
}

const std::vector<std::string> FindTemplateParameters::getTemplateArgs() const {
  std::vector<std::string> arg_list;
  if ((template_args_ == nullptr) || (template_args_->size() == 0)) {
    return arg_list;
  }

  for (const auto &arg : template_args_->asArray()) {
    std::string name{};
    switch (arg.getKind()) {
      case TemplateArgument::ArgKind::Integral: {
        // Return an LLVM APSInt type.
        auto number{arg.getAsIntegral()};
        SmallString<10> small_str;
        number.toString(small_str);
        arg_list.push_back(std::string(small_str.c_str()));
        LLVM_DEBUG(llvm::dbgs() << "Arg: " << small_str << "\n";);
      }; break;
      case TemplateArgument::ArgKind::Type: {
        auto q{arg.getAsType()};
        name = q.getAsString();
        arg_list.push_back(name);
        LLVM_DEBUG(llvm::dbgs() << "Arg: " << name << "\n";);
      }; break;
      default: {
      }
    };
  }
  return arg_list;
}

FindTemplateParameters::~FindTemplateParameters() { declaration_ = nullptr; }

void FindTemplateParameters::dump() {}
