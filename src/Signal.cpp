#include <string>
#include "Signal.h"
#include "FindTemplateTypes.h"

using namespace scpar;
using namespace std;

void
  Signal::check (
)
{
	assert (!(_sig == NULL));
}

Signal::Signal ():
_name ("NONE"), _sig (NULL)
{

}

Signal::Signal (const string & name, SignalContainer * s
):
_name (name
),
_sig (s
)
{

}

void
  Signal::setModuleName (const string & name
)
{
	_name = name;
}

string Signal::getName ()
{
	return _name;
}

FindTemplateTypes *
Signal::getTemplateTypes (
)
{
	check ();
	return _sig->getTemplateTypes ();
}

FieldDecl *
Signal::getASTNode (
)
{
	check ();
	return _sig->getASTNode ();
}

void
  Signal::dump (raw_ostream & os, int tabn = 0)
{
	check ();
	for (int i = 0; i < tabn; i++)
		{
			os << " ";
		}
	os << "Signal " << this << " '" << _name << "' FindTemplateTypes " <<
		_sig->getTemplateTypes () << "' FieldDecl' " << _sig->getASTNode ();
	_sig->getTemplateTypes ()->printTemplateArguments (os, tabn);
}
