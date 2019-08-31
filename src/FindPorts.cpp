#include "FindPorts.h"
#include "FindTemplateTypes.h"
using namespace scpar;

FindPorts::FindPorts(CXXRecordDecl *d, llvm::raw_ostream &os) : os_{os} {
  TraverseDecl(d);
}

FindPorts::PortType FindPorts::getInputPorts() const { return _inPorts; }

FindPorts::PortType FindPorts::getOutputPorts() const { return _outPorts; }

FindPorts::PortType FindPorts::getInputOutputPorts() const {
  return _inoutPorts;
}

FindPorts::PortType FindPorts::getOtherVars() const {
  return _otherVars;
}

bool FindPorts::VisitFieldDecl(FieldDecl *fd) {
  //  os_ << "####################### VisitFieldDecl\n ";
  QualType q = fd->getType();
  string fname;

  if (IdentifierInfo *info = fd->getIdentifier()) {
    fname = info->getNameStart();
    //     os_ << "\n+ Name: " << info->getNameStart();
    //   os_ << "\n+ Type: " << q.getAsString();
  }
  //
  /// FIXME: We have to delete these somewhere.  Who is responsible of doing
  /// this?

  /// We are going to store these.  So use pointers.
  const Type *tp = q.getTypePtr();
  FindTemplateTypes *te = new FindTemplateTypes();

  te->Enumerate(tp);
  // te->printTemplateArguments();

  /// Check if we have sc_in/sc_out/sc_inout ports.
  /// The vector is organized such that the first element is the port type.
  //	FindTemplateTypes::argVectorType
  auto args{te->getTemplateArgumentsType()};
  FindTemplateTypes::argVectorType::iterator ait{args.begin()};
  if (args.size() == 0) {
    return true;
  }
  string port_type{ ait->getTypeName()};
  if (port_type == "sc_in") {
    //        os_ << "\n+ sc_in";
    _inPorts.insert(kvType(fname, te));
  } else if (port_type == "sc_out") {
    //        os_ << "\n+ sc_out";
    _outPorts.insert(kvType(fname, te));
  } else if (port_type == "sc_inout") {
    //        os_ << "\n+ sc_inout";
    _inoutPorts.insert(kvType(fname, te));
  } else if (port_type != "sc_signal") {
    /// This is sometype we don't know about.
    _otherVars.insert(kvType(fname, te));
  } else {
  //_sc_signal handled in another pass
  }

  return true;
}

void FindPorts::dump() {
  os_ << "\n================= Find Ports ================\n";
  os_ << "\n:> Number of input Ports: " << _inPorts.size();
  for (PortType::iterator mit = _inPorts.begin(), mitend = _inPorts.end();
       mit != mitend; mit++) {
    os_ << "\n:>> " << mit->first;
    (mit->second)->printTemplateArguments(os_);
  }

  os_ << "\n\n:> Number of output Ports: " << _outPorts.size();
  for (PortType::iterator mit = _outPorts.begin(), mitend = _outPorts.end();
       mit != mitend; mit++) {
    os_ << "\n:>> " << mit->first;
    (mit->second)->printTemplateArguments(os_);
  }

  os_ << "\n\n:> Number of inout Ports: " << _inoutPorts.size();
  for (PortType::iterator mit = _inoutPorts.begin(), mitend = _inoutPorts.end();
       mit != mitend; mit++) {
    os_ << "\n:>> " << mit->first;
    (mit->second)->printTemplateArguments(os_);
  }


  os_ << "\n\n:> Number of other Vars: " << _otherVars.size();
  for (PortType::iterator mit = _otherVars.begin(), mitend = _otherVars.end();
       mit != mitend; mit++) {
    os_ << "\n:>> " << mit->first;
    (mit->second)->printTemplateArguments(os_);
  }


  os_ << "\n================= END Find Ports ================\n\n";
}

FindPorts::~FindPorts() {
  // os_ << "[[ Destructor FindPorts ]]\n";
  for (PortType::iterator mit = _inPorts.begin(), mitend = _inPorts.end();
       mit != mitend; mit++) {
    //  for ( auto mit : _inPorts ) {
    delete mit->second;
  }
  _inPorts.clear();

  for (PortType::iterator mit = _outPorts.begin(), mitend = _outPorts.end();
       mit != mitend; mit++) {
    //  for ( auto mit : _outPorts ) {
    //      os_ << "\n:>> " << mit->first;
    delete mit->second;
  }
  _outPorts.clear();

  //    os_ << "\n\n:> Number of inout Ports: " << _inoutPorts.size();
  for (PortType::iterator mit = _inoutPorts.begin(), mitend = _inoutPorts.end();
       mit != mitend; mit++) {
    //  for ( auto mit : _inoutPorts ) {
    //      os_ << "\n:>> " << mit->first;
    delete mit->second;
  }
  _inoutPorts.clear();

  for (PortType::iterator mit = _otherVars.begin(), mitend = _otherVars.end();
       mit != mitend; mit++) {
    //  for ( auto mit : _otherVars ) {
    //      os_ << "\n:>> " << mit->first;
    delete mit->second;
  }
  _otherVars.clear();
}
