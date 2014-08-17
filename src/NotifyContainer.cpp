#include "NotifyContainer.h"
#include "llvm/Support/raw_ostream.h"

using namespace scpar;
using namespace llvm;

NotifyContainer::~NotifyContainer()
{

	// DO NOT free anything since nothing is dynamically allocated.

	// Clear _args.
	_args.clear();
}

NotifyContainer::NotifyContainer(CXXMethodDecl * m, CallExpr * c):_entryMethodDecl(m), _astNode(c),
_numArgs(c->getNumArgs())
{

	// Use CXXMemberCallExpr to populate the ARgMap.
	populateArgMap();
}

NotifyContainer::NotifyContainer(const NotifyContainer & from)
{
	_entryMethodDecl = from._entryMethodDecl;
	_astNode = from._astNode;
	_numArgs = from._numArgs;

	// TODO: Copy all strings (std::vector handles this)
	_args = from._args;
}

unsigned int NotifyContainer::getNumArgs()
{
	return _numArgs;
}

CXXMethodDecl *NotifyContainer::getEntryMethod()
{
	return _entryMethodDecl;
}

CallExpr *NotifyContainer::getASTNode()
{
	return _astNode;
}

NotifyContainer::waitArgVectorType NotifyContainer::getArgList()
{
	return _args;
}

string NotifyContainer::getArgString(Expr * e)
{
	if (!e)
	{
		return string("NULL");
	}

	LangOptions LO;

	LO.CPlusPlus = true;
	PrintingPolicy Policy(LO);
	string TypeS;
	raw_string_ostream s(TypeS);

	e->printPretty(s, 0, Policy);
	return s.str();
}

void NotifyContainer::populateArgMap()
{
	if (!_astNode)
	{
		return;
	}

	for (unsigned int i = 0; i < _astNode->getNumArgs(); i++)
	{
		_args.push_back(getArgString(_astNode->getArg(i)));
	}
}


void NotifyContainer::dump(raw_ostream & os, int tabn)
{

	for (int i = 0; i < tabn; i++)
	{
		os << " ";
	}

	if (getNumArgs() > 2)
	{
		os << "NotifyContainer numargs: " << getNumArgs() - 1 << " ";
		os << " arglist: ";
		for (unsigned int i = 0; i < getNumArgs() - 1; i++)
		{
			os << "'" << _args[i] << "' ";
		}
	}
	else
	{
		os << "NotifyContainer numargs: " << getNumArgs() << " ";
		if (getNumArgs() > 0)
		{
			os << " arglist: ";
		}
		for (unsigned int i = 0; i < getNumArgs(); i++)
		{
			os << "'" << _args[i] << "' ";
		}
	}

	os << "\n";
}
