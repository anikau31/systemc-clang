//===-- src/Utility.h - systec-clang class definition -------*- C++ -*-===//
//
//                     systemc-clang: SystemC Parser
//
// This file is distributed under the University of Illinois License.
// See LICENSE.mkd for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief A set of utility functions in the Utility class.
///
//===----------------------------------------------------------------------===//
#ifndef _UTILITY_H_
#define _UTILITY_H_

#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/Basic/SourceManager.h"
#include "clang/AST/PrettyPrinter.h"
#include <string>
namespace scpar {
	using namespace clang;
	using namespace std;
	class Utility {
	public:
		Utility (){
		
		}

		/*
		void tabit (llvm::raw_ostream &, int tabn);
		string strip (string, string);
	
		template < typename vec > vec removeDuplicate (vec);
		template < typename vec, typename element > bool isElementPresent (vec, element);
		template < typename element > string getArgumentName (element *);
		*/	
		void tabit (raw_ostream & os, int tabn)
		{
			for (int i = 0; i < tabn; i++) {
				os << " ";
			}
		}

		string strip (string s, string sub) {
			// sub has "struct "
			size_t pos = s.find (sub);

			if (pos == string::npos) {
				return s;
			}

			return s.erase (pos, sub.length ());
		}

		template < typename vec > vec removeDuplicate (vec vecInput) {
			for (unsigned int i = 0; i < vecInput.size (); i++) {
				for (unsigned int j = 0; j < vecInput.size (); j++)
				{
					if (i != j && vecInput.at (i) == vecInput.at (j))
						{
							vecInput.erase (vecInput.begin () + j);
						}
				}
			}
			return vecInput;
		}

		template < typename vec, typename element >
			bool isElementPresent (vec vecInput, element elemInput) {
				for (unsigned int i = 0; i < vecInput.size (); i++) {
					if (elemInput == vecInput.at (i)) {
					return true;
				}
			}
			return false;
		}

		template < typename expressionArg > string getArgumentName (expressionArg * exp)
{
			if (exp == NULL) {
				return string ("NULL");
			}

			clang::LangOptions LangOpts;
			LangOpts.CPlusPlus = true;
			clang::PrintingPolicy Policy (LangOpts);

			string TypeS;

			llvm::raw_string_ostream s (TypeS);

			exp->printPretty (s, 0, Policy);

			return s.str ();
			}
	};
}
#endif
