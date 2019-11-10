#include "FindPorts.h"
#include "FindTemplateTypes.h"
using namespace scpar;

FindPorts::FindPorts(CXXRecordDecl *d, llvm::raw_ostream &os) : os_{os} { TraverseDecl(d); }

FindPorts::PortType FindPorts::getInputPorts() const { return inPorts_; }

FindPorts::PortType FindPorts::getOutputPorts() const { return outPorts_; }

FindPorts::PortType FindPorts::getInputOutputPorts() const { return inoutPorts_; }

FindPorts::PortType FindPorts::getInStreamPorts() const { return inStreamPorts_; }

FindPorts::PortType FindPorts::getOutStreamPorts() const { return outStreamPorts_; }

FindPorts::PortType FindPorts::getOtherVars() const { return otherVars_; }

bool FindPorts::VisitFieldDecl(FieldDecl *fd) {
  QualType q { fd->getType() };
  string fname{};

  if ( IdentifierInfo *info = fd->getIdentifier() ) {
    fname = info->getNameStart();
    //os_ << "\n+ Name: " << info->getNameStart();
  //  os_ << "\n+ Type: " << q.getAsString();
  }

  // These should be deleted in the appropriate container's constructor.
  //
  const Type *tp{ q.getTypePtr() };

  //
  // Continue to parse other types
  FindTemplateTypes *te{ new FindTemplateTypes() };
  te->Enumerate(tp);
  //te->printTemplateArguments(os_);

  /// Check if we have sc_in/sc_out/sc_inout ports.
  /// The vector is organized such that the first element is the port type.
  //	FindTemplateTypes::argVectorType
  auto args{ te->getTemplateArgumentsType() };
  FindTemplateTypes::argVectorType::iterator ait{args.begin()};

  if ( args.size()==0 ) {
    return true;
  }

  string port_type{ ait->getTypeName()};
  //os_ << "@@@@@ Port type: " << port_type << "\n";
  if (port_type == "sc_in") {
   // os_ << "\n+ sc_in";
    inPorts_.insert(kvType(fname, te));
  } else if (port_type == "sc_out") {
    //        os_ << "\n+ sc_out";
    outPorts_.insert(kvType(fname, te));
  } else if (port_type == "sc_inout") {
    //        os_ << "\n+ sc_inout";
    inoutPorts_.insert(kvType(fname, te));
  } else if (port_type == "sc_stream_in") {
    inStreamPorts_.insert(kvType(fname, te));
  } else if (port_type == "sc_stream_out") {
    outStreamPorts_.insert(kvType(fname, te));
  } else if (port_type != "sc_signal") {
    // Signals are handled in FindSignals
    // Treat everything that is not a sc_signal as others.
    /// This is sometype we don't know about.
    otherVars_.insert(kvType(fname, te));
  } else {
    //_sc_signal handled in another pass
    // So any others are others.
  }

  return true;
}

void FindPorts::dump() {
  os_ << "\n================= Find Ports ================\n";
  os_ << "\n:> Number of input Ports: " << inPorts_.size();
  for (PortType::iterator mit = inPorts_.begin(), mitend = inPorts_.end();
      mit != mitend; mit++) {
    os_ << "\n:>> " << mit->first;
    (mit->second)->printTemplateArguments(os_);
  }

  os_ << "\n\n:> Number of output Ports: " << outPorts_.size();
  for (PortType::iterator mit = outPorts_.begin(), mitend = outPorts_.end();
      mit != mitend; mit++) {
    os_ << "\n:>> " << mit->first;
    (mit->second)->printTemplateArguments(os_);
  }

  os_ << "\n\n:> Number of inout Ports: " << inoutPorts_.size();
  for (PortType::iterator mit = inoutPorts_.begin(), mitend = inoutPorts_.end();
      mit != mitend; mit++) {
    os_ << "\n:>> " << mit->first;
    (mit->second)->printTemplateArguments(os_);
  }

  os_ << "\n\n:> Number of inStream Ports: " << inStreamPorts_.size();
  for (PortType::iterator mit = inStreamPorts_.begin(), mitend = inStreamPorts_.end();
      mit != mitend; mit++) {
    os_ << "\n:>> " << mit->first;
    (mit->second)->printTemplateArguments(os_);
  }



  os_ << "\n\n:> Number of other Vars: " << otherVars_.size();
  for (PortType::iterator mit = otherVars_.begin(), mitend = otherVars_.end();
      mit != mitend; mit++) {
    os_ << "\n:>> " << mit->first;
    (mit->second)->printTemplateArguments(os_);
  }


  os_ << "\n================= END Find Ports ================\n\n";
}

FindPorts::~FindPorts() {
  // os_ << "[[ Destructor FindPorts ]]\n";
  for (PortType::iterator mit = inPorts_.begin(), mitend = inPorts_.end();
      mit != mitend; mit++) {
    //  for ( auto mit : inPorts_ ) {
    delete mit->second;
  }
  inPorts_.clear();

  for (PortType::iterator mit = outPorts_.begin(), mitend = outPorts_.end();
      mit != mitend; mit++) {
    //  for ( auto mit : outPorts_ ) {
    //      os_ << "\n:>> " << mit->first;
    delete mit->second;
  }
  outPorts_.clear();

  //    os_ << "\n\n:> Number of inout Ports: " << inoutPorts_.size();
  for (PortType::iterator mit = inoutPorts_.begin(), mitend = inoutPorts_.end();
      mit != mitend; mit++) {
    //  for ( auto mit : inoutPorts_ ) {
    //      os_ << "\n:>> " << mit->first;
    delete mit->second;
  }
  inoutPorts_.clear();

  for (PortType::iterator mit = inStreamPorts_.begin(), mitend = inStreamPorts_.end();
      mit != mitend; mit++) {
    delete mit->second;
  }
  inStreamPorts_.clear();


  for (PortType::iterator mit = outStreamPorts_.begin(), mitend = outStreamPorts_.end();
      mit != mitend; mit++) {
    delete mit->second;
  }
  outStreamPorts_.clear();

  for (PortType::iterator mit = otherVars_.begin(), mitend = otherVars_.end();
      mit != mitend; mit++) {
    //  for ( auto mit : otherVars_ ) {
    //      os_ << "\n:>> " << mit->first;
    delete mit->second;
  }
  otherVars_.clear();
  }
