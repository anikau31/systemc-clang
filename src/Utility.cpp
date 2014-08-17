#include "Utility.h"

using namespace scpar;

void
  Utility::tabit (raw_ostream & os, int tabn
)
{
	for (int i = 0; i < tabn; i++)
		{
			os << " ";
		}
}

string Utility::strip (string s, string sub)
{
	// sub has "struct "
	size_t
		pos = s.find (sub);

	if (pos == string::npos)
		{
			return s;
		}

	return s.erase (pos, sub.length ());
}

template < typename vec > vec Utility::removeDuplicate (vec vecInput)
{
	for (unsigned int i = 0; i < vecInput.size (); i++)
		{
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
	bool Utility::isElementPresent (vec vecInput, element elemInput)
{
	for (unsigned int i = 0; i < vecInput.size (); i++)
		{
			if (elemInput == vecInput.at (i))
				{
					return true;
				}
		}
	return false;
}

template < typename expressionArg >
	string getArgumentName (expressionArg * exp)
{
	if (exp == NULL)
		{
			return string ("NULL");

			clang::LangOptions LangOpts;
			LangOpts.CPlusPlus = true;
			clang::PrintingPolicy Policy (LangOpts);

			string TypeS;

			llvm::raw_string_ostream s (TypeS);

			exp->printPretty (s, 0, Policy);

			return s.str ();
		}
}


Utility::Utility ()
{

}
