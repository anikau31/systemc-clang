#include "FindTemplateParameters.h"
#include "FindTemplateTypes.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/Type.h"
#include "clang/Basic/SourceManager.h"

using namespace scpar;

FindTemplateParameters::FindTemplateParameters(CXXRecordDecl *declaration,
                                               llvm::raw_ostream &os)
    : declaration_{declaration},
      os_{os},
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
      //tdecl->dump();
      template_args_ = &tdecl->getTemplateArgs();
      os_ << "@@ template specialization args: " << module_name
          << ", #args: " << template_args_->size() << "\n";
      for (size_t i{0}; i < template_args_->size(); ++i) {
        // Check the kind of the argument.
        switch (template_args_->get(i).getKind()) {
          case TemplateArgument::ArgKind::Integral: {
            auto q{template_args_->get(i).getAsIntegral()};
            // auto name{q.getAsString()};
            os_ << "@@ Integral: " << q << "\n";
          }; break;
          case TemplateArgument::ArgKind::Type: {
            auto q{template_args_->get(i).getAsType()};
            auto name{q.getAsString()};
            os_ << "@@ arg: " << name << "\n";
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
      os_ << "@@ template described class args: " << module_name << "\n";
      template_parameters_ = template_args->getTemplateParameters();
    }
  }

  return false;
}

vector<string> FindTemplateParameters::getTemplateParameters() const {
  vector<string> parm_list;
  if ((template_parameters_ == nullptr) ||
      (template_parameters_->size() <= 0)) {
    return parm_list;
  }

  for (auto parm : template_parameters_->asArray()) {
    parm_list.push_back(parm->getName());
    os_ << "Parm: " << parm->getName() << "\n";
  }
  return parm_list;
}

vector<string> FindTemplateParameters::getTemplateArgs() const {
  vector<string> arg_list;
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
        arg_list.push_back(string(small_str.c_str()));
        os_ << "Arg: " << small_str << "\n";
      }; break;
      case TemplateArgument::ArgKind::Type: {
        auto q{arg.getAsType()};
        name = q.getAsString();
        arg_list.push_back(name);
        os_ << "Arg: " << name << "\n";
      }; break;
      default: {
      }
    };
  }
  return arg_list;
}

FindTemplateParameters::~FindTemplateParameters() { declaration_ = nullptr; }

void FindTemplateParameters::dump() {}
