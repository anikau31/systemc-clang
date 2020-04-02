#include "FindTemplateTypes.h"
#include "Utility.h"

using namespace scpar;

//////////////////////////////////////////////////////////////////////
// TemplateType
//////////////////////////////////////////////////////////////////////
TemplateType::TemplateType() : type_name_{""}, type_ptr_{nullptr} {}

TemplateType::TemplateType(std::string name, const Type *t)
    : type_name_{name}, type_ptr_{t} {}

TemplateType::TemplateType(const TemplateType &from) {
  type_name_ = from.type_name_;
  type_ptr_ = from.type_ptr_;
}

TemplateType::~TemplateType() { type_ptr_ = nullptr; }

std::string TemplateType::getTypeName() const { return type_name_; }

std::string TemplateType::toString() const { return getTypeName(); }

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

void FindTemplateTypes::Enumerate(const Type *type) {
  if (!type) {
    return;
  }

  TraverseType(QualType(type->getUnqualifiedDesugaredType(), 1));
}

bool FindTemplateTypes::VisitTemplateSpecializationType(
    TemplateSpecializationType *special_type) {
  // llvm::outs() << "=VisitTemplateSpecializationType=\n";
  // special_type->dump();
  auto template_name{special_type->getTemplateName()};
  // template_name.dump();

  clang::LangOptions LangOpts;
  LangOpts.CPlusPlus = true;
  clang::PrintingPolicy Policy(LangOpts);

  std::string name_string;
  llvm::raw_string_ostream sstream(name_string);
  template_name.print(sstream, Policy, 0);
  // llvm::outs() << "== template_name: " << sstream.str() << "\n";

  auto new_node{
      template_args_.addNode(TemplateType{sstream.str(), special_type})};
  template_args_.addEdge(current_type_node_, new_node);

  template_types_.push_back(TemplateType(sstream.str(), special_type));

  return true;
}

bool FindTemplateTypes::VisitCXXRecordDecl(CXXRecordDecl *cxx_record) {
  // llvm::outs() << "=VisitCXXRecordDecl=\n";
  if (cxx_record != nullptr) {
    IdentifierInfo *info{cxx_record->getIdentifier()};
    if (info != nullptr) {
      // llvm::outs() << " ==> CXXRecord type: " << info->getNameStart() <<
      // "\n";
      template_types_.push_back(
          TemplateType(info->getNameStart(), cxx_record->getTypeForDecl()));
    }
  }

  return true;
}

bool FindTemplateTypes::VisitBuiltinType(BuiltinType *bi_type) {
  llvm::outs() << "=VisitBuiltinType=\n";
  // bi_type->dump();

  clang::LangOptions LangOpts;
  LangOpts.CPlusPlus = true;
  clang::PrintingPolicy Policy(LangOpts);

  auto type_name{bi_type->getNameAsCString(Policy)};
  llvm::outs() << "type is : " << type_name << "\n";

  current_type_node_ = template_args_.addNode(TemplateType(type_name, bi_type));
  template_types_.push_back(TemplateType(type_name, bi_type));

  if (template_args_.size() == 1) {
    template_args_.setRoot(current_type_node_);
  }

  return false;
}

bool FindTemplateTypes::VisitTypedefType(TypedefType *typedef_type) {
  // llvm::outs() << "=VisitTypedefType=\n";
  // typedef_type->dump();
  // child nodes of TemplateSpecializationType are not being invoked.
  if (auto special_type = typedef_type->getAs<TemplateSpecializationType>()) {
    TraverseType(QualType(special_type, 0));
  }
  return true;
}

bool FindTemplateTypes::VisitRecordType(RecordType *rt) {
  // llvm::outs() << "=VisitRecordType=\n";
  auto type_decl{rt->getDecl()};
  auto type_name{type_decl->getName()};
  // llvm::outs() << " ==> name : " << type_name << "\n";

  current_type_node_ = template_args_.addNode(TemplateType(type_name, rt));

  if (template_args_.size() == 1) {
    template_args_.setRoot(current_type_node_);
  }

  if (stack_current_node_.size() > 0) {
    template_args_.addEdge(stack_current_node_.top(), current_type_node_);
  }

  stack_current_node_.push(current_type_node_);
  // template_args_.dump();

  template_types_.push_back(TemplateType(type_name, rt));

  if (auto ctsd = dyn_cast<ClassTemplateSpecializationDecl>(type_decl)) {
    // llvm::outs() << " ==> CTSD : " << ctsd->getTemplateArgs().size() << "\n";

    const TemplateArgumentList &arg_list{ctsd->getTemplateArgs()};
    for (unsigned int i{0}; i < arg_list.size(); ++i) {
      const TemplateArgument &targ{arg_list[i]};
      // llvm::outs() << " ====> template argument: ";
      // targ.dump();
      // llvm::outs() << "\n";
      // TODO Write this into the vector.
      // llvm::outs() << " ====> template type : " << targ.getKind() << "\n";
      //
      if (targ.getKind() == TemplateArgument::ArgKind::Type) {
        QualType template_name{targ.getAsType()};
        //  llvm::outs() << " ====> template_type_name " <<
        //  template_name.getAsString() << "\n";
        const Type *arg_type{targ.getAsType().getTypePtr()};

        if (!arg_type->isBuiltinType()) {
          stack_current_node_.push(current_type_node_);
          TraverseType(arg_list[i].getAsType());
          current_type_node_ = stack_current_node_.top();
          stack_current_node_.pop();
        } else {
          auto new_node{template_args_.addNode(
              TemplateType{template_name.getAsString(), arg_type})};
          template_args_.addEdge(current_type_node_, new_node);

          template_types_.push_back(
              TemplateType(template_name.getAsString(), rt));
        }
      } else if (targ.getKind() == TemplateArgument::ArgKind::Integral) {
        QualType template_name{targ.getNonTypeTemplateArgumentType()};
        // template_name.dump();

        // Special case:
        // Ignore when there are implicit arguments.
        //
        // sc_core::sc_writer_policy
        //
        // llvm::outs() << " ====> template_type_name integral "
        //             << template_name.getAsString() << "\n";
        // llvm::outs() << " ====> Integral : ";
        //

        if (template_name.getAsString() != "enum sc_core::sc_writer_policy") {
          auto integral{targ.getAsIntegral()};
          SmallString<16> integral_string{};
          integral.toString(integral_string);
          // llvm::outs() << integral_string << "\n";

          auto new_node{template_args_.addNode(TemplateType{
              integral_string.c_str(), template_name.getTypePtr()})};
          template_args_.addEdge(current_type_node_, new_node);

          template_types_.push_back(TemplateType(integral_string.c_str(), rt));
        }
      }
    }
  }
  // llvm::outs() << ">>> stack size: " << stack_current_node_.size() << "\n";
  current_type_node_ = stack_current_node_.top();
  stack_current_node_.pop();

  // llvm::outs() << "=END VisitRecordType=\n";
  return true;
}

bool FindTemplateTypes::VisitIntegerLiteral(IntegerLiteral *l) {
  // llvm::outs() << "\n=VisitIntegerLiteral: \n";
  //<< l->getValue().toString(10,true) <<
  //"\n"; _os << "== type ptr: " << l->getType().getTypePtr() << "\n"; _os <<
  //"== type name: " << l->getType().getAsString() << "\n";
  template_types_.push_back(TemplateType(l->getValue().toString(10, true),
                                         l->getType().getTypePtr()));
  // template_types_.push_back(pair<string, const Type
  // *>(l->getValue().toString(10, true), l->getType().getTypePtr()));

  return false;
}

Tree<TemplateType> *FindTemplateTypes::getTemplateArgTreePtr() {
  return &template_args_;
}

FindTemplateTypes::type_vector_t FindTemplateTypes::getTemplateArgumentsType() {
  return template_types_;
}

json FindTemplateTypes::dump_json() {
  json tree_j;

  auto args{getTemplateArgTreePtr()};

  for (auto const &node : *args) {
    // Returns a TreeNodePtr
    auto type_data{node->getDataPtr()};
    auto parent_node{node->getParent()};
    auto parent_data{parent_node->getDataPtr()};
    if (parent_node->getDataPtr() == node->getDataPtr()) {
      // llvm::outs() << "\nInsert parent node: " << type_data->getTypeName()
      //             << "\n";
      tree_j[type_data->getTypeName()] = nullptr;
    } else {
      // FIXME: This does not print the tree properly.
      // There does not seem to be a simple way to access the appropriate
      // location for the insertion of the new values in this JSON.
      // TODO: Perhaps the way to do this is to construct a string that JSON can
      // use.
      tree_j[parent_data->getTypeName()].push_back(type_data->getTypeName());
    }
  }
  llvm::outs() << tree_j.dump(4);
  return tree_j;
}

void FindTemplateTypes::printTemplateArguments(llvm::raw_ostream &os) {
  auto root_node{template_args_.getRoot()};
  auto s{template_args_.dft(root_node)};
  os << "> Template args (DFT): " << s << "\n";
}

vector<std::string> FindTemplateTypes::getTemplateArguments() {
  vector<std::string> template_arguments;
  for (auto const &mit : template_types_) {
    std::string name{mit.getTypeName()};
    if (name == "sc_in" || name == "sc_out" || name == "sc_inout") {
      break;
    }
    template_arguments.push_back(name);
  }
  return template_arguments;
}

size_t FindTemplateTypes::size() { return template_types_.size(); }
