#include "FindPorts.h"
#include "FindTemplateTypes.h"
using namespace scpar;

FindPorts::FindPorts ( CXXRecordDecl *d, llvm::raw_ostream &os ) :
  os_{os} {
	TraverseDecl(d);
}

FindPorts::PortType FindPorts::getInputPorts() const {
	return _inPorts;
}

FindPorts::PortType FindPorts::getOutputPorts() const {
	return _outPorts;
}

FindPorts::PortType FindPorts::getInputOutputPorts() const {
	return _inoutPorts;
}

bool FindPorts::VisitFieldDecl( FieldDecl *fd ) {
  //  os_ << "####################### VisitFieldDecl\n ";
	QualType q = fd->getType();
	string fname;

	if (IdentifierInfo *info = fd->getIdentifier())	{
    fname = info->getNameStart();
    //     os_ << "\n+ Name: " << info->getNameStart();
    //   os_ << "\n+ Type: " << q.getAsString();
  }
  // 
	/// FIXME: We have to delete these somewhere.  Who is responsible of doing this?

	/// We are going to store these.  So use pointers.
	const Type *tp = q.getTypePtr();
	FindTemplateTypes *te = new FindTemplateTypes();

	te->Enumerate(tp);
	//te->printTemplateArguments();

	/// Check if we have sc_in/sc_out/sc_inout ports.
	/// The vector is organized such that the first element is the port type.
	FindTemplateTypes::argVectorType args{ te->getTemplateArgumentsType() };
	FindTemplateTypes::argVectorType::iterator ait{ args.begin() };
	if ( args.size() == 0 )	{
    return true;
  }
	if ( ait->first == "sc_in" ) {
    //        os_ << "\n+ sc_in";
    _inPorts.insert ( kvType(fname, te) );
  } else if (ait->first == "sc_out") {
    //        os_ << "\n+ sc_out";
    _outPorts.insert( kvType( fname, te ) );
  }
	else if ( ait->first == "sc_inout" ) {
    //        os_ << "\n+ sc_inout";
    _inoutPorts.insert ( kvType( fname, te ) );
  }
	else {
    /// This is sometype we don't know about.
  }

	return true;
}

void FindPorts::dump() {
	os_ << "\n================= Find Ports ================\n";
	os_ << "\n:> Number of input Ports: " << _inPorts.size ();
  for (PortType::iterator mit = _inPorts.begin (), mitend = _inPorts.end ();			 mit != mitend; mit++) {
    os_ << "\n:>> " << mit->first;
    (mit->second)->printTemplateArguments(os_);
  }

	os_ << "\n\n:> Number of output Ports: " << _outPorts.size ();
  for (PortType::iterator mit = _outPorts.begin (), mitend = _outPorts.end ();			 mit != mitend; mit++) {
    os_ << "\n:>> " << mit->first;
    (mit->second)->printTemplateArguments(os_);
  }

	os_ << "\n\n:> Number of inout Ports: " << _inoutPorts.size ();
  for (PortType::iterator mit = _inoutPorts.begin (), mitend = _inoutPorts.end (); mit != mitend; mit++) {
    os_ << "\n:>> " << mit->first;
    (mit->second)->printTemplateArguments(os_);
  }
	os_ << "\n================= END Find Ports ================\n\n";
}

FindPorts::~FindPorts() {
	// os_ << "[[ Destructor FindPorts ]]\n";
  for (PortType::iterator mit = _inPorts.begin (), mitend = _inPorts.end ();  			 mit != mitend; mit++) {
    //  for ( auto mit : _inPorts ) {
    delete mit->second;
  }
	_inPorts.clear ();

  for (PortType::iterator mit = _outPorts.begin (), mitend = _outPorts.end ();			 mit != mitend; mit++) {
    //  for ( auto mit : _outPorts ) {
    //      os_ << "\n:>> " << mit->first;
    delete mit->second;
  }
	_outPorts.clear();

  //    os_ << "\n\n:> Number of inout Ports: " << _inoutPorts.size();
	for (PortType::iterator mit = _inoutPorts.begin (), mitend =			 _inoutPorts.end (); mit != mitend; mit++) {
    //  for ( auto mit : _inoutPorts ) {
    //      os_ << "\n:>> " << mit->first;
    delete mit->second;
  }
	_inoutPorts.clear();
}
