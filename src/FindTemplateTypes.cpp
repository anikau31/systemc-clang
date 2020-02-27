#include "FindTemplateTypes.h"
#include "Utility.h"

using namespace scpar;

//////////////////////////////////////////////////////////////////////
// TemplateType
//////////////////////////////////////////////////////////////////////
TemplateType::TemplateType() : type_name_{""}, type_ptr_{nullptr} {}

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
  template_args_ = rhs.template_args_;
}

FindTemplateTypes::FindTemplateTypes(const FindTemplateTypes *rhs) {
  copy(rhs->template_types_.begin(), rhs->template_types_.end(),
       back_inserter(template_types_));
  template_args_ = rhs->template_args_;
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

  /*
  llvm::outs() << "####  Sugared #### \n";
  type->dump();
  */
  llvm::outs() << "####  Desugared #### \n";
  type->getUnqualifiedDesugaredType()->dump();

  TraverseType(QualType(type->getUnqualifiedDesugaredType(), 1));
  return template_types_;
}

/*
bool FindTemplateTypes::VisitTemplateArgument(TemplateArgument *ta) {
  llvm::outs() << "=VisitTemplateArgument=\n";

  return true;
}

bool FindTemplateTypes::VisitClassTemplateSpecializationDecl(
    ClassTemplateSpecializationDecl *class_special_type) {
  llvm::outs() << "=VisitClassTemplateSpecializationDecl=\n";
  return true;
}
*/

bool FindTemplateTypes::VisitTemplateSpecializationType(
    TemplateSpecializationType *special_type) {
  llvm::outs() << "=VisitTemplateSpecializationType=\n";
  // special_type->dump();
  auto template_name{special_type->getTemplateName()};
  // template_name.dump();

  clang::LangOptions LangOpts;
  LangOpts.CPlusPlus = true;
  clang::PrintingPolicy Policy(LangOpts);

  std::string name_string;
  llvm::raw_string_ostream sstream(name_string);
  template_name.print(sstream, Policy, 0);
  llvm::outs() << "== template_name: " << sstream.str() << "\n";

  auto new_node{template_args_.addNode(sstream.str())};
  template_args_.addEdge(current_type_node_, new_node);

  template_types_.push_back(TemplateType(sstream.str(), special_type));

  // Template argument
  //

  unsigned int narg{special_type->getNumArgs()};
  for (unsigned int i{0}; i != narg; ++i) {
    llvm::outs() << " ==> template arg \n";
    const TemplateArgument &arg{special_type->getArg(i)};
    arg.dump();
    arg.getAsType().getTypePtr()->dump();
    // traversetype(arg.getastype());
  }

  return true;
}

bool FindTemplateTypes::VisitCXXRecordDecl(CXXRecordDecl *cxx_record) {
  llvm::outs() << "=VisitCXXRecordDecl=\n";
  if (cxx_record != nullptr) {
    IdentifierInfo *info{cxx_record->getIdentifier()};
    if (info != nullptr) {
      llvm::outs() << " ==> CXXRecord type: " << info->getNameStart() << "\n";
      template_types_.push_back(
          TemplateType(info->getNameStart(), cxx_record->getTypeForDecl()));
    }
  }

  return true;
}

bool FindTemplateTypes::VisitTypedefType(TypedefType *typedef_type) {
  llvm::outs() << "=VisitTypedefType=\n";
  // typedef_type->dump();
  // child nodes of TemplateSpecializationType are not being invoked.
  if (auto special_type = typedef_type->getAs<TemplateSpecializationType>()) {
    TraverseType(QualType(special_type, 0));
  }
  return true;
}

bool FindTemplateTypes::VisitType(Type *type) {
  llvm::outs() << "=VisitType=\n";
  /*
  QualType q{type->getCanonicalTypeInternal()};
  llvm::outs() << "\n###### Type: " << q.getAsString() << " \n";

  if (type->isBuiltinType()) {
    llvm::outs() << " ==> builtin type: " << q.getAsString() << "\n";
    template_types_.push_back(TemplateType(q.getAsString(), type));
    return false;
  }

  // This detects a user-defined type (class/struct).
  // This is done by checking that this is not a TemplateSpecializationType, and
  // if it is not a DependentType then there is no further parsing to be done.
  //
  // This is done because it is not clear how to detect user-defined types
  // that are the inner-most template arguments.
  //
  if ((!type->getAs<TemplateSpecializationType>()) &&
      (!type->isDependentType())) {
    llvm::outs() << " ==> user defined type: " << q.getAsString() << "\n";
    type->dump();

    // FIXME: This is where struct fp_t<11,52> fails.
    //
    //



    Utility util;
    std::string type_name{q.getAsString()};
    type_name = util.strip(type_name, "class ");
    type_name = util.strip(type_name, "struct ");

    template_types_.push_back(TemplateType(type_name, type));
    return false;
  }

  */
  return true;
}

bool FindTemplateTypes::VisitRecordType(RecordType *rt) {
  llvm::outs() << "=VisitRecordType=\n";
  auto type_decl{rt->getDecl()};
  auto type_name{type_decl->getName()};
  llvm::outs() << " ==> name : " << type_name << "\n";

  /*
  if ( template_args_.begin() == template_args_.end()) {
    template_args_.insert(template_args_.begin(), TemplateType(type_name, rt));
    current_template_ = template_args_.begin();
  } else {

    current_template_ = template_args_.insert(template_args_.begin(),
  TemplateType(type_name, rt));

  }
  */

  current_type_node_ = template_args_.addNode( type_name );
  if (template_args_.size() == 1) {
    template_args_.setRoot(current_type_node_);
  }

  stack_current_node_.push( current_type_node_);
  template_args_.dump();

  template_types_.push_back(TemplateType(type_name, rt));

  if (auto ctsd = dyn_cast<ClassTemplateSpecializationDecl>(type_decl)) {
    llvm::outs() << " ==> CTSD : " << ctsd->getTemplateArgs().size() << "\n";
    // ctsd->dump();

    const TemplateArgumentList &arg_list{ctsd->getTemplateArgs()};
    for (unsigned int i{0}; i < arg_list.size(); ++i) {
      const TemplateArgument &targ{arg_list[i]};
      llvm::outs() << " ====> template argument: ";
      targ.dump();
      llvm::outs() << "\n";
      // TODO Write this into the vector.
      llvm::outs() << " ====> template type : " << targ.getKind() << "\n";

      if (targ.getKind() == TemplateArgument::ArgKind::Type) {
        QualType template_name{targ.getAsType()};
        llvm::outs() << " ====> template_type_name "
                     << template_name.getAsString() << "\n";
        const Type *arg_type{targ.getAsType().getTypePtr()};
        //.getTypePtr()->dump();
        if (!arg_type->isBuiltinType()) {
          TraverseType(arg_list[i].getAsType());
        } else {
          auto new_node {template_args_.addNode(template_name.getAsString())};
          template_args_.addEdge(current_type_node_, new_node );

          template_types_.push_back(TemplateType(template_name.getAsString(), rt));
        }
      }

      if (targ.getKind() == TemplateArgument::ArgKind::Integral) {
        QualType template_name{targ.getNonTypeTemplateArgumentType()};
        template_name.dump();
        llvm::outs() << " ====> template_type_name integral "
                     << template_name.getAsString() << "\n";

        llvm::outs() << " ====> Integral : ";
        auto integral{targ.getAsIntegral()};
        SmallString<16> integral_string{};
        integral.toString(integral_string);
        llvm::outs() << integral_string << "\n";

        auto new_node {template_args_.addNode(integral_string.c_str())};
        template_args_.addEdge(current_type_node_, new_node );

        template_types_.push_back(TemplateType(integral_string.c_str(), rt));
      }
    }
  }
  current_type_node_ = stack_current_node_.top();
  stack_current_node_.pop();

  llvm::outs() << "=END VisitRecordType=\n";
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

  return false;
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

  os << ">>>> Print current node \n";
  auto root_node{template_args_.getRoot()};
  os << ">>>> Print arguments using DFT\n";
  template_args_.dump();
  template_args_.dft(root_node);
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
