#include "FindTemplateTypes.h"

using namespace scpar;

//////////////////////////////////////////////////////////////////////
// TemplateType
//////////////////////////////////////////////////////////////////////

TemplateType::TemplateType( string name, const Type* t ):
  type_name_{ name }, type_ptr_{t} {}

TemplateType::TemplateType( const TemplateType & from ) {
  type_name_ = from.type_name_;
  type_ptr_  = from.type_ptr_;
}

TemplateType::~TemplateType() {
  type_ptr_ = nullptr;
}

string TemplateType::getTypeName() const { return type_name_; }

const Type* TemplateType::getTypePtr() { return type_ptr_; }

//////////////////////////////////////////////////////////////////////
// FindTemplateTypes
//////////////////////////////////////////////////////////////////////

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
FindTemplateTypes::~FindTemplateTypes() {
  // TODO: We should convert these into objects.
  template_types_.clear();
}

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

FindTemplateTypes::type_vector_t FindTemplateTypes::Enumerate(const Type *type) {
  template_types_.clear();
  if (!type) {
    return template_types_;
  }

  TraverseType(QualType(type, 0));
  return template_types_;
}

bool FindTemplateTypes::VisitType( Type *type ) {
    QualType q{type->getCanonicalTypeInternal()};
    //      cout << "\n###### Type: " << q.getAsString() << " \n" ;
    if (type->isBuiltinType()) {
      template_types_.push_back( TemplateType( q.getAsString(), type) );
      //template_types_.push_back( pair<string, const Type *>(q.getAsString(), type) );
      return true;
    }

    CXXRecordDecl *p_cxx_record{type->getAsCXXRecordDecl()};
    if (p_cxx_record != nullptr) {
      IdentifierInfo *info{p_cxx_record->getIdentifier()};
      if (info != nullptr) {
        template_types_.push_back( TemplateType( info->getNameStart(), type ) );
        //template_types_.push_back( pair<string, const Type *>(info->getNameStart(), type) );
      }
    }
    return true;
  }

bool FindTemplateTypes::VisitIntegerLiteral(IntegerLiteral *l) {
    //_os << "\n####### IntegerLiteral: " << l->getValue().toString(10,true) <<
    //"\n"; _os << "== type ptr: " << l->getType().getTypePtr() << "\n"; _os <<
    //"== type name: " << l->getType().getAsString() << "\n";
  template_types_.push_back( TemplateType( l->getValue().toString(10, true),  l->getType().getTypePtr() ) );
  //template_types_.push_back(pair<string, const Type *>(l->getValue().toString(10, true), l->getType().getTypePtr()));

    return true;
  }

FindTemplateTypes::type_vector_t FindTemplateTypes::getTemplateArgumentsType() { return template_types_; }

void FindTemplateTypes::printTemplateArguments(llvm::raw_ostream &os) {
    vector<string> template_arguments;
    for (auto const &mit : template_types_) {
      template_arguments.push_back( mit.getTypeName() );
    }

    // Print the template arguments to the output stream
    os << ", " << template_arguments.size() << " arguments, ";
    for (auto const &targ : template_arguments) {
      os << targ << " ";
    }
  }

vector<string> FindTemplateTypes::getTemplateArguments() {
    vector<string> template_arguments;
    for (auto const &mit : template_types_) {
      string name{ mit.getTypeName() };
      if (name == "sc_in" || name == "sc_out" || name == "sc_inout") {
        break;
      }
      template_arguments.push_back( name );
    }
    return template_arguments;
  }

size_t FindTemplateTypes::size() { return template_types_.size(); }
