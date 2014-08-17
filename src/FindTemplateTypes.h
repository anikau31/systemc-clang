#ifndef _FIND_TYPE_H_
#define _FIND_TYPE_H_

#include<set>
#include<vector>
#include <string>
#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/Type.h"

namespace scpar {
	using namespace clang;
	using namespace std;

	class FindTemplateTypes:public RecursiveASTVisitor < FindTemplateTypes > {
	public:
		/// Typedefs
		typedef vector < pair < string, const Type *> >vector_t;
		typedef pair < string, const Type *>kvVector_t;
		typedef vector < pair < string, const Type *> >argVectorType;


		  FindTemplateTypes (
		) {
		}
		/// Copy constructor 
  FindTemplateTypes (const FindTemplateTypes & rhs ) {
			copy (rhs._templateTypes.begin (), rhs._templateTypes.end (),
						back_inserter (_templateTypes)
		);
	} string getTemplateType (
	) {
		string s;

		for (vector_t::iterator mit = _templateTypes.begin ();
				 mit != _templateTypes.end (); mit++)
			{
				if (mit != _templateTypes.begin ())
					{
						s += "<";
					}
				s += mit->first;
				if (mit != _templateTypes.begin ())
					{
						s += ">";
					}
			}
		return s;
	}

	vector_t Enumerate (const Type * type
	) {
		_templateTypes.clear ();
		if (!type)
			{
				return _templateTypes;
			}

		TraverseType (QualType (type, 0));
		return _templateTypes;
	}

	bool VisitType (Type * type
	) {
		QualType q = type->getCanonicalTypeInternal ();

//            _os << "\n###### Type: " << q.getAsString() << " \n" ;

		if (type->isBuiltinType ())
			{

				_templateTypes.push_back (pair < string,
																	const Type * >(q.getAsString (), type)
				);

				return true;
			}

		CXXRecordDecl *crType = type->getAsCXXRecordDecl ();

		if (crType != NULL)
			{
				IdentifierInfo *info = crType->getIdentifier ();

				if (info != NULL)
					{
						_templateTypes.push_back (pair < string,
																			const Type * >(info->getNameStart (),
																										 type)
						);
					}
			}

		return true;
	}

	bool VisitIntegerLiteral (IntegerLiteral * l
	) {
		//_os << "\n####### IntegerLiteral: " << l->getValue().toString(10,true) << "\n";
		//_os << "== type ptr: " << l->getType().getTypePtr() << "\n";
		//_os << "== type name: " << l->getType().getAsString() << "\n";
		_templateTypes.push_back (pair < string,
															const Type *
															>(l->getValue ().toString (10, true),
																l->getType ().getTypePtr ()));

		return true;
	}

	vector_t getTemplateArgumentsType (
	) {
		return _templateTypes;
	}

	void printTemplateArguments (llvm::raw_ostream & os, int tabn = 0)
	{

		for (vector_t::iterator mit = _templateTypes.begin ();
				 mit != _templateTypes.end (); mit++)
			{
				for (int i = 0; i < tabn; i++)
					{
						os << " ";
					}
				os << "- " << mit->first << ", type ptr: " << mit->second;
				os << "\n";
			}
	}

	vector < string > getTemplateArguments ()
	{
		vector < string > args;
		for (vector_t::iterator mit = _templateTypes.begin ();
				 mit != _templateTypes.end (); mit++)
			{
				if (mit->first == "sc_in" || mit->first == "sc_out"
						|| mit->first == "sc_inout")
					{
						break;
					}

				args.push_back (mit->first);
				//  _os << "\n:>> " << mit->first << ", type ptr: " << mit->second;
			}
		return args;
	}

	unsigned int size (
	) {
		return _templateTypes.size ();
	}
private:
	vector_t _templateTypes;

};
}
#endif
