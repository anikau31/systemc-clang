#include "FindTLMInterfaces.h"
#include "FindTemplateTypes.h"
using namespace scpar;

FindTLMInterfaces::FindTLMInterfaces (CXXRecordDecl * d, llvm::raw_ostream & os)
:_os (os)
{
	TraverseDecl (d);
}

FindTLMInterfaces::interfaceType FindTLMInterfaces::getInputInterfaces ()
{
	return _inInterfaces;
}

FindTLMInterfaces::interfaceType FindTLMInterfaces::getOutputInterfaces ()
{
	return _outInterfaces;
}

FindTLMInterfaces::interfaceType FindTLMInterfaces::getInputOutputInterfaces ()
{
	return _inoutInterfaces;
}

bool FindTLMInterfaces::VisitFieldDecl (FieldDecl * fd)
{
   
	QualType q = fd->getType ();
	string fname;

	if (IdentifierInfo * info = fd->getIdentifier ()) {
			fname = info->getNameStart ();   
		}
  
	const Type *tp = q.getTypePtr ();
	FindTemplateTypes *te = new FindTemplateTypes ();

	te->Enumerate (tp);
 
	FindTemplateTypes::argVectorType args = te->getTemplateArgumentsType ();
	FindTemplateTypes::argVectorType::iterator ait = args.begin ();
	if (args.size () == 0) {
			return true;
	}
	
 if (ait->first == "sc_fifo_in") {  
			_inInterfaces.insert (kvType (fname, te));	     
 }
	
 else if (ait->first == "sc_fifo_out") {
	 _outInterfaces.insert (kvType (fname, te));
	}

	else if (ait->first == "sc_fifo_inout") {
	 _inoutInterfaces.insert (kvType (fname, te));
	}
	
 else {
			/// This is sometype we don't know about.
	}

	return true;
}

void FindTLMInterfaces::dump ()
{
	_os << "\n================= Find Interfaces ================\n";
	_os << "\n:> Number of input Interfaces: " << _inInterfaces.size ();
	for (interfaceType::iterator mit = _inInterfaces.begin (), mitend = _inInterfaces.end ();
			 mit != mitend; mit++)
		{
			_os << "\n:>> " << mit->first;
			(mit->second)->printTemplateArguments (_os);
		}

	_os << "\n\n:> Number of output Interfaces: " << _outInterfaces.size ();
	for (interfaceType::iterator mit = _outInterfaces.begin (), mitend = _outInterfaces.end ();
			 mit != mitend; mit++)
		{
			_os << "\n:>> " << mit->first;
			(mit->second)->printTemplateArguments (_os);
		}

	_os << "\n\n:> Number of inout Interfaces: " << _inoutInterfaces.size ();
	for (interfaceType::iterator mit = _inoutInterfaces.begin (), mitend =
			 _inoutInterfaces.end (); mit != mitend; mit++)
		{
			_os << "\n:>> " << mit->first;
			(mit->second)->printTemplateArguments (_os);
		}
	_os << "\n================= END Find Interfaces ================\n\n";
}

FindTLMInterfaces::~FindTLMInterfaces ()
{
	// _os << "[[ Destructor FindTLMInterfaces ]]\n";
	for (interfaceType::iterator mit = _inInterfaces.begin (), mitend = _inInterfaces.end ();
			 mit != mitend; mit++)
		{
			delete mit->second;
		}
	_inInterfaces.clear ();

	for (interfaceType::iterator mit = _outInterfaces.begin (), mitend = _outInterfaces.end ();
			 mit != mitend; mit++)
		{
    
			delete mit->second;
		}
	_outInterfaces.clear ();

	for (interfaceType::iterator mit = _inoutInterfaces.begin (), mitend =
			 _inoutInterfaces.end (); mit != mitend; mit++)
		{
    
			delete mit->second;
		}
	_inoutInterfaces.clear ();
}
