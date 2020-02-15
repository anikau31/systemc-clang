#include "FindTemplateTypes.h"

using namespace scpar;

//////////////////////////////////////////////////////////////////////
// TemplateType
//////////////////////////////////////////////////////////////////////

TemplateType::TemplateType(string name, const Type *t)
    : type_name_{name}, type_ptr_{t} {}

TemplateType::TemplateType(const TemplateType &from) {
  type_name_ = from.type_name_;
  type_ptr_ = from.type_ptr_;
}

TemplateType::~TemplateType() { type_ptr_ = nullptr; }

string TemplateType::getTypeName() const { return type_name_; }

const Type *TemplateType::getTypePtr() const { return type_ptr_; }

//////////////////////////////////////////////////////////////////////
// FindTemplateTypes
//////////////////////////////////////////////////////////////////////

bool FindTemplateTypes::shouldVisitTemplateInstantiations() const {
  return true;
}

// Constructor
FindTemplateTypes::FindTemplateTypes() {}

/// Copy constructor
FindTemplateTypes::FindTemplateTypes(const FindTemplateTypes &rhs) {
  copy(rhs.template_types_.begin(), rhs.template_types_.end(),
       back_inserter(template_types_));
}

FindTemplateTypes::FindTemplateTypes(const FindTemplateTypes *rhs) {
  copy(rhs->template_types_.begin(), rhs->template_types_.end(),
       back_inserter(template_types_));
}

// Destructor
FindTemplateTypes::~FindTemplateTypes() { template_types_.clear(); }

string FindTemplateTypes::getTemplateType() {
  string s{};

  // type_vector_t::iterator
  for (auto mit = template_types_.begin(); mit != template_types_.end();
       ++mit) {
    // for ( auto const &mit: template_types_ ) {
    if (mit != template_types_.begin()) {
      s += "<";
    }
    s += mit->getTypeName();
    if (mit != template_types_.begin()) {
      s += ">";
    }
  }
  return s;
}

FindTemplateTypes::type_vector_t FindTemplateTypes::Enumerate(
    const Type *type) {
  template_types_.clear();
  if (!type) {
    return template_types_;
  }

  // type->dump();
  TraverseType(QualType(type, 0));
  return template_types_;
}

bool FindTemplateTypes::VisitClassTemplateSpecializationDecl(ClassTemplateSpecializationDecl *class_special_type) {

  llvm::outs() << "=VisitClassTemplateSpecializationDecl=\n";
  return true;
}

bool FindTemplateTypes::VisitTemplateSpecializationType(TemplateSpecializationType *special_type) {
  llvm::outs() << "=VisitTemplateSpecializationType=\n";
  special_type->dump();
  auto template_name {special_type->getTemplateName()};
  template_name.dump();
  

  clang::LangOptions LangOpts;
  LangOpts.CPlusPlus = true;
  clang::PrintingPolicy Policy(LangOpts);

  std::string name_string;
  llvm::raw_string_ostream sstream(name_string);
  template_name.print(sstream, Policy, 0);
  llvm::outs() << "== template_name: " << sstream.str() << "\n";
  template_types_.push_back(TemplateType(sstream.str(), special_type));

  return true;
}

bool FindTemplateTypes::VisitCXXRecordDecl(CXXRecordDecl *cxx_record) {
  llvm::outs() << "=VisitCXXRecordDecl=\n";
    if (cxx_record != nullptr) {
      IdentifierInfo *info{cxx_record->getIdentifier()};
      if (info != nullptr) {
        llvm::outs() << " ==> CXXRecord type: " << info->getNameStart() << "\n";
        template_types_.push_back(TemplateType(info->getNameStart(), cxx_record->getTypeForDecl()));
      }
    }

    return true;
}

bool FindTemplateTypes::VisitTypedefType(TypedefType *typedef_type) {
  llvm::outs() << "=VisitTypedefType=\n";
  typedef_type->dump();
  // child nodes of TemplateSpecializationType are not being invoked.
  if (auto special_type = typedef_type->getAs<TemplateSpecializationType>()) {
    TraverseType(QualType(special_type, 0));
  }
  return true;
}


bool FindTemplateTypes::VisitType(Type *type) {
  llvm::outs() << "=VisitType=\n";
  //type->dump();
  QualType q{type->getCanonicalTypeInternal()};
  // cout << "\n###### Type: " << q.getAsString() << " \n";
  if (type->isBuiltinType()) {
    cout << " ==> builtin type: " << q.getAsString() << "\n";
    template_types_.push_back(TemplateType(q.getAsString(), type));
    return false;
  } 

  /*

  else

      // Template for sc_stream_in and sc_stream_out
      // These are types that are dependent on the parameter for the template.
      if (type->isDependentType()) {
    if (auto dp = type->getAs<TemplateSpecializationType>()) {
      auto tn{dp->getTemplateName()};
      auto tunder{tn.getUnderlying()};
      auto name{tunder.getAsTemplateDecl()->getNameAsString()};
      template_types_.push_back(TemplateType(name, type));
      // cout << " ==> dependent type: " << name << "\n";
    }
  }
  // Identify the _Bool in the sc_in_clk
  // TODO: Hack.  We need to figure out a clean way to identify types.
  else if (auto tt = type->getAs<TemplateSpecializationType>()) {
    auto arg{tt->getArgs()};
    auto arg_kind{arg->getKind()};
    // llvm::outs() << "==> template specialization type: " << arg_kind << "\n";
    // We have to make sure that it is fully evaluated before moving forward.
    // If it is not then just keep parsing.
    // if (arg_kind == TemplateArgument::ArgKind::Expression) {
    //  return true;
    //}
    //
    // Break out if the template argument is integral or a type.
    if ((arg_kind == TemplateArgument::ArgKind::Integral) ||
        (arg_kind == TemplateArgument::ArgKind::Type)) {
      template_types_.push_back(
          TemplateType(arg->getAsType().getAsString(), type));
      return false;
    }
  } else {
    CXXRecordDecl *p_cxx_record{type->getAsCXXRecordDecl()};
    if (p_cxx_record != nullptr) {
      IdentifierInfo *info{p_cxx_record->getIdentifier()};
      // cout << "##### info; " << info->getNameStart() << "\n";
      if (info != nullptr) {
        template_types_.push_back(TemplateType(info->getNameStart(), type));
        // cout << " ==> CXXRecord type: " << info->getNameStart() << "\n";
      }
    }
  }
  */
  return true;
}

bool FindTemplateTypes::VisitIntegerLiteral(IntegerLiteral *l) {
  llvm::outs() << "\n=VisitIntegerLiteral: \n";
  //<< l->getValue().toString(10,true) <<
  //"\n"; _os << "== type ptr: " << l->getType().getTypePtr() << "\n"; _os <<
  //"== type name: " << l->getType().getAsString() << "\n";
  template_types_.push_back(TemplateType(l->getValue().toString(10, true),
                                         l->getType().getTypePtr()));
  // template_types_.push_back(pair<string, const Type
  // *>(l->getValue().toString(10, true), l->getType().getTypePtr()));

  return true;
}

FindTemplateTypes::type_vector_t FindTemplateTypes::getTemplateArgumentsType() {
  return template_types_;
}

void FindTemplateTypes::printTemplateArguments(llvm::raw_ostream &os) {
  vector<string> template_arguments;
  for (auto const &mit : template_types_) {
    template_arguments.push_back(mit.getTypeName());
  }

  // Print the template arguments to the output stream
  os << ", " << template_arguments.size() << " arguments, ";
  for (auto const &targ : template_arguments) {
    os << targ << " ";
  }
  os << "\n";
}

vector<string> FindTemplateTypes::getTemplateArguments() {
  vector<string> template_arguments;
  for (auto const &mit : template_types_) {
    string name{mit.getTypeName()};
    if (name == "sc_in" || name == "sc_out" || name == "sc_inout") {
      break;
    }
    template_arguments.push_back(name);
  }
  return template_arguments;
}

size_t FindTemplateTypes::size() { return template_types_.size(); }
