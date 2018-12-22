#include "FindSimTime.h"

using namespace scpar;

FindSimTime::FindSimTime ( FunctionDecl * tuDecl, llvm::raw_ostream & os ) :
  os_ (os) {
  //  _sigInst{nullptr},
  //  call_expr_{nullptr} {

    assert (!(tuDecl == nullptr ));
    TraverseDecl (tuDecl);
}

FindSimTime::~FindSimTime() {
	simulation_time_.clear ();
}

bool FindSimTime::VisitCallExpr( CallExpr * c ) {
	if (c->getDirectCallee ()->getNameInfo ().getAsString () != "sc_start") {
			return true;
		}

  //  os_ << "Found sc_start";
	if (c->getNumArgs () > 0)	{
      //    os_ <<"\n Simulation time is :" << getArgumentName(c->getArg(0));
			if (c->getNumArgs () > 1)	{
					//    os_ << " " << getArgumentName(c->getArg(1));
					simulation_time_.insert (simulationTimePairType
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
	//  os_ << ", argument: " << s.str() << "\n";
	return s.str ();
  }
*/

FindSimTime::simulationTimeMapType FindSimTime::returnSimTime() {
	return simulation_time_;
}
