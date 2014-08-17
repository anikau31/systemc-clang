#include "FindEvents.h"
#include "FindTemplateTypes.h"
using namespace scpar;
using namespace std;

FindEvents::FindEvents (CXXRecordDecl * d, llvm::raw_ostream & os):
_os (os)
{

	TraverseDecl (d);
//  printInClassEvents();
}

FindEvents::FindEvents (const FindEvents & from
):
_os (from._os
)
{
	_inClassEvents = from._inClassEvents;
}

FindEvents::~FindEvents ()
{
	// These pointer are NOT to be deleted
	// : FieldDecl in _inClassEvents.
	// This is because they are pointers to the clang AST, which clang should free.

}

bool FindEvents::VisitFieldDecl (FieldDecl * fd)
{
	//FindTemplateTypes te(_os);
	QualType
		q = fd->getType ();

	if (q.getAsString () == "class sc_core::sc_event")
		{
			if (IdentifierInfo * info = fd->getIdentifier ())
				{
					//_os << "\n+ Name: " << info->getNameStart();
					//_os << "\n+ Type: " << q.getAsString();
					_inClassEvents.insert (kvType (info->getNameStart (), fd));
				}
		}
	return true;
}

FindEvents::classEventMapType FindEvents::getInClassEvents ()
{																// FIXME: change to return data structure
	return _inClassEvents;
}

vector < string > FindEvents::getEventNames ()
{
	vector < string > keys;
	for (classEventMapType::iterator vit = _inClassEvents.begin ();
			 vit != _inClassEvents.end (); vit++)
		{
			keys.push_back (vit->first);
		}
	return keys;
}

void
  FindEvents::dump (
)
{
	_os << "\n ============== FindEvents  ===============";
	_os << "\n:> Print in-class sc_event data members";
	for (classEventMapType::iterator vit = _inClassEvents.begin ();
			 vit != _inClassEvents.end (); vit++)
		{
			_os << "\n:> name: " << vit->first << ", FieldDecl*: " << vit->second;
		}
	_os << "\n ============== END FindEvents ===============";
}
