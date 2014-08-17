#include "FindPorts.h"
#include "FindTemplateTypes.h"
using namespace scpar;

FindPorts::FindPorts (CXXRecordDecl * d, llvm::raw_ostream & os):
_os (os)
//, _reflectionContainer(container) 
{

	TraverseDecl (d);

}

FindPorts::portType FindPorts::getInputPorts ()
{
	return _inPorts;
}

FindPorts::portType FindPorts::getOutputPorts ()
{
	return _outPorts;
}

FindPorts::portType FindPorts::getInputOutputPorts ()
{
	return _inoutPorts;
}

bool FindPorts::VisitFieldDecl (FieldDecl * fd)
{
//  _os << "####################### VisitFieldDecl\n ";
	QualType
		q = fd->getType ();
	string
		fname;

	if (IdentifierInfo * info = fd->getIdentifier ())
		{
			fname = info->getNameStart ();
			//     _os << "\n+ Name: " << info->getNameStart();
			//   _os << "\n+ Type: " << q.getAsString();
		}
// 
	/// FIXME: We have to delete these somewhere.  Who is responsible of doing this?

	/// We are going to store these.  So use pointers.
	const Type *
		tp = q.getTypePtr ();
	FindTemplateTypes *
		te = new FindTemplateTypes ();

	te->Enumerate (tp);
	//te->printTemplateArguments();

	/// Check if we have sc_in/sc_out/sc_inout ports.
	/// The vector is organized such that the first element is the port type.
	FindTemplateTypes::argVectorType args = te->getTemplateArgumentsType ();
	FindTemplateTypes::argVectorType::iterator ait = args.begin ();
	if (args.size () == 0)
		{
			return true;
		}
	if (ait->first == "sc_in")
		{
			//        _os << "\n+ sc_in";
			_inPorts.insert (kvType (fname, te));

		}
	else if (ait->first == "sc_out")
		{
			//        _os << "\n+ sc_out";
			_outPorts.insert (kvType (fname, te));

		}
	else if (ait->first == "sc_inout")
		{
			//        _os << "\n+ sc_inout";
			_inoutPorts.insert (kvType (fname, te));

		}
	else
		{
			/// This is sometype we don't know about.
		}

	return true;
}

void
  FindPorts::dump (
)
{
	_os << "\n================= Find Ports ================\n";
	_os << "\n:> Number of input Ports: " << _inPorts.size ();
	for (portType::iterator mit = _inPorts.begin (), mitend = _inPorts.end ();
			 mit != mitend; mit++)
		{
			_os << "\n:>> " << mit->first;
			(mit->second)->printTemplateArguments (_os);
		}

	_os << "\n\n:> Number of output Ports: " << _outPorts.size ();
	for (portType::iterator mit = _outPorts.begin (), mitend = _outPorts.end ();
			 mit != mitend; mit++)
		{
			_os << "\n:>> " << mit->first;
			(mit->second)->printTemplateArguments (_os);
		}

	_os << "\n\n:> Number of inout Ports: " << _inoutPorts.size ();
	for (portType::iterator mit = _inoutPorts.begin (), mitend =
			 _inoutPorts.end (); mit != mitend; mit++)
		{
			_os << "\n:>> " << mit->first;
			(mit->second)->printTemplateArguments (_os);
		}
	_os << "\n================= END Find Ports ================\n\n";
}

FindPorts::~FindPorts ()
{
	// _os << "[[ Destructor FindPorts ]]\n";
	for (portType::iterator mit = _inPorts.begin (), mitend = _inPorts.end ();
			 mit != mitend; mit++)
		{
			delete mit->second;
		}
	_inPorts.clear ();

	for (portType::iterator mit = _outPorts.begin (), mitend = _outPorts.end ();
			 mit != mitend; mit++)
		{
//      _os << "\n:>> " << mit->first;
			delete mit->second;
		}
	_outPorts.clear ();

//    _os << "\n\n:> Number of inout Ports: " << _inoutPorts.size();
	for (portType::iterator mit = _inoutPorts.begin (), mitend =
			 _inoutPorts.end (); mit != mitend; mit++)
		{
//      _os << "\n:>> " << mit->first;
			delete mit->second;
		}
	_inoutPorts.clear ();
}
