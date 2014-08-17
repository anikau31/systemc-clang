#include "FindSignals.h"
#include "FindTemplateTypes.h"
using namespace scpar;

FindSignals::~FindSignals ()
{
//  _os << "[[ Destructor FindSignals ]]\n";
	for (FindSignals::signalMapType::iterator sit = _signals->begin ();
			 sit != _signals->end (); sit++)
		{
			delete sit->second;
		}
	_signals->clear ();
	delete _signals;
}

FindSignals::FindSignals (CXXRecordDecl * d, llvm::raw_ostream & os):
_os (os)
{
	_signals = new FindSignals::signalMapType ();
	state = 0;
	TraverseDecl (d);
}


bool FindSignals::VisitFieldDecl (FieldDecl * fd)
{
//  _os << "####################### FindSignals::VisitFieldDecl\n ";
	QualType
		q = fd->getType ();

	if (IdentifierInfo * info = fd->getIdentifier ())
		{
			//    fname = info->getNameStart();
			//    _os << "\n+ Name: " << info->getNameStart();
//    _os << "\n+ Type: " << q.getAsString();
			//   _os << "\n+ also name: " << fd->getNameAsString();

			/// We are going to store these.  So use pointers.
			const Type *
				tp = q.getTypePtr ();
			FindTemplateTypes *
				te = new FindTemplateTypes ();

			te->Enumerate (tp);
			// te->printTemplateArguments(_os);

			string
				tt = te->getTemplateType ();

//    _os << "OUTPUT ============ " << tt << "\n";
			if ((signed) tt.find ("sc_signal") == -1)
				{
					delete
						te;

					return true;
				}
			SignalContainer *
				sc = new SignalContainer (fd->getNameAsString (), te, fd);

			_signals->insert (FindSignals::
												signalPairType (fd->getNameAsString (), sc));

		}

	return true;
}

FindSignals::signalMapType * FindSignals::getSignals ()
{
	return _signals;
}

void
  FindSignals::dump (
)
{
	_os << "\n================= Find Signals  ================\n";

	for (FindSignals::signalMapType::iterator sit = _signals->begin ();
			 sit != _signals->end (); sit++)
		{

			_os << sit->second;
			sit->second->dump (_os);
		}
	_os << "\n================= END Find Ports ================\n\n";
}
