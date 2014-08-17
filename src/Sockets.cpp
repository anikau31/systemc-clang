#include "Sockets.h"

using namespace scpar;

Sockets::Sockets ():_name ("NONE")
{

}

Sockets::Sockets (const string & name, const string & socketType,
									vector < string > registerCallbackMethods
):
_name (name
),
_socketType (socketType
),
_registerCallbackMethods (registerCallbackMethods
)
{

}

Sockets::Sockets (const Sockets & from
)
{
	_name = from._name;
	_socketType = from._socketType;
	_registerCallbackMethods = from._registerCallbackMethods;
}

string Sockets::getName ()
{
	return _name;
}

string Sockets::getSocketType ()
{
	return _socketType;
}

vector < string > Sockets::getRegisterCallbackMethods ()
{
	return _registerCallbackMethods;
}


void
  Sockets::dump (raw_ostream & os, int tabn
)
{

	os << "\n Sockets : " << _name << " Sockets Type : " << _socketType;
	os << " Register Callback Methods : ";
	for (int i = 0; i < _registerCallbackMethods.size (); i++)
		{
			os << _registerCallbackMethods.at (i) << " ";
		}
}
