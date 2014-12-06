#include "FindSimTime.h"
using namespace scpar;

FindSimTime::FindSimTime (FunctionDecl * tuDecl, llvm::raw_ostream & os):
_os (os), _sigInst (NULL), _callexpr (NULL)
{
	assert (!(tuDecl == NULL));

	TraverseDecl (tuDecl);
}

FindSimTime::~FindSimTime ()
{
	_simTime.clear ();
}

bool FindSimTime::VisitCallExpr (CallExpr * c)
{

	if (c->getDirectCallee ()->getNameInfo ().getAsString () != "sc_start")
		{
			return true;
		}

//  _os << "Found sc_start";
	if (c->getNumArgs () > 0)
		{
//    _os <<"\n Simulation time is :" << getArgumentName(c->getArg(0));
			if (c->getNumArgs () > 1)
				{
					//    _os << " " << getArgumentName(c->getArg(1));
					_simTime.insert (simulationTimePairType
													 (getArgumentName (c->getArg (0)),
														getArgumentName (c->getArg (1))));
				}
		}
	return true;
}

/*
string FindSimTime::getArgumentName (Expr * arg)
{
	if (arg == NULL)
		return string ("NULL");

	clang::LangOptions LangOpts;
	LangOpts.CPlusPlus = true;
	clang::PrintingPolicy Policy (LangOpts);

	string
		TypeS;

	llvm::raw_string_ostream s (TypeS);

	arg->printPretty (s, 0, Policy);
	//  _os << ", argument: " << s.str() << "\n";
	return s.str ();
}
*/

FindSimTime::simulationTimeMapType FindSimTime::returnSimTime ()
{
	return _simTime;
}
