#ifndef _FIND_TEMPLATE_TYPES_H_
#define _FIND_TEMPLATE_TYPES_H_

#include <set>
#include <vector>
#include <string>
#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/Type.h"

#include <iostream>

namespace scpar {
  using namespace clang;
  using namespace std;

  // This class is going to find the arguments from templates
  class FindTemplateTypes : public RecursiveASTVisitor < FindTemplateTypes > {
  public:
    /// Typedefs
    typedef vector < pair < string, const Type *> > type_vector_t;
    typedef vector < pair < string, const Type *> > argVectorType;

    // Constructor
    FindTemplateTypes() { }

    /// Copy constructor
    FindTemplateTypes( const FindTemplateTypes &rhs ) {
      copy (rhs._templateTypes.begin(), rhs._templateTypes.end(),
            back_inserter (_templateTypes) );
    }

    FindTemplateTypes( const FindTemplateTypes *rhs ) {
      copy (rhs->_templateTypes.begin(), rhs->_templateTypes.end(),
            back_inserter (_templateTypes) );
    }

    string getTemplateType() {
      string s{};

      // type_vector_t::iterator
      for (auto mit = _templateTypes.begin (); mit != _templateTypes.end (); ++mit)  {
        //for ( auto const &mit: _templateTypes ) {
        if ( mit != _templateTypes.begin() )  {
          s += "<";
        }
        s += mit->first;
        if ( mit != _templateTypes.begin() )  {
          s += ">";
        }
      }
      return s;
    }

    type_vector_t Enumerate( const Type *type ) {
      _templateTypes.clear();
      if ( !type ) {
        return _templateTypes;
      }

      TraverseType( QualType (type, 0) );
      return _templateTypes;
    }

    bool VisitType( Type *type ) {
      QualType q{type->getCanonicalTypeInternal()};
      //      cout << "\n###### Type: " << q.getAsString() << " \n" ;
      if ( type->isBuiltinType() )  {
        _templateTypes.push_back( pair < string, const Type * >(q.getAsString(), type) );
        return true;
      }

      CXXRecordDecl *p_cxx_record{type->getAsCXXRecordDecl()};
      if ( p_cxx_record != nullptr )  {
        IdentifierInfo *info{p_cxx_record->getIdentifier()};
        if ( info != nullptr )  {
          _templateTypes.push_back(pair < string, const Type * >(info->getNameStart(), type) );
        }
      }
      return true;
    }

    bool VisitIntegerLiteral( IntegerLiteral *l ) {
      //_os << "\n####### IntegerLiteral: " << l->getValue().toString(10,true) << "\n";
      //_os << "== type ptr: " << l->getType().getTypePtr() << "\n";
      //_os << "== type name: " << l->getType().getAsString() << "\n";
      _templateTypes.push_back (pair < string, const Type * >(l->getValue ().toString (10, true),
                                                              l->getType().getTypePtr()));

      return true;
    }

    type_vector_t getTemplateArgumentsType() {
      return _templateTypes;
    }

    void printTemplateArguments( llvm::raw_ostream &os, int tabn = 0 )  {
      vector < string > template_arguments; //{ getTemplateArguments() };
      // type_vector_t::iterator
      //      for (auto mit = _templateTypes.begin(); mit != _templateTypes.end(); mit++)   {
      for ( auto const &mit: _templateTypes ) {
        for ( auto i{0}; i < tabn; ++i)  {
          os << " ";
        }
        os << "- " << mit.first << ", type ptr: " << mit.second;
        os << "\n";
        template_arguments.push_back( mit.first );
      }

      // Print the template arguments to the output stream
      os << "= ";
      for ( auto const &targ: template_arguments ) {
        os << targ << "  ";
      }
    }

    vector < string > getTemplateArguments() {
      vector < string > template_arguments;
      // type_vector_t::iterator
      //      for ( auto mit = _templateTypes.begin(); mit != _templateTypes.end(); ++mit )  {
      for ( auto const &mit: _templateTypes ) {
        if ( mit.first == "sc_in" || mit.first == "sc_out"  || mit.first == "sc_inout" )  {
          break;
        }
        template_arguments.push_back( mit.first );
      }
      return template_arguments;
    }

    size_t size() {
      return _templateTypes.size();
    }
  
  private:
    type_vector_t _templateTypes;

  };
}
#endif
