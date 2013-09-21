#include "FindClassDataMembers.h"
#include "FindTemplateTypes.h"
#include "FindSocketRegisterMethods.h"
using namespace scpar;

SocketCharacteristics::SocketCharacteristics(string socketType, vector<string> callbackMethods):
_socketType(socketType),
_callbackMethods(callbackMethods) {
	
}

SocketCharacteristics::~SocketCharacteristics() {
	
	_socketType.clear();
	_callbackMethods.clear();

}

string SocketCharacteristics::getSocketType() {
	return _socketType;
}

SocketCharacteristics::registerCallbackMethods 
SocketCharacteristics::getRegisterCallbackMethods(){
	return _callbackMethods;
}

///////////////////////////////////////////////////////////////////////////////////
FindClassDataMembers::FindClassDataMembers(CXXRecordDecl* d, llvm::raw_ostream &os):
 _d(d),
 _os(os),
 pass(1),
 _constructorStmt(NULL)
{

  TraverseDecl(_d);

  pass = 2;
  TraverseStmt(_constructorStmt);
//  printInClassMembers();
}

FindClassDataMembers::~FindClassDataMembers() {
	for (socketCharacteristicsMapType::iterator it = 
	_socketCharacteristicsMap.begin(),
	eit = _socketCharacteristicsMap.end();
	it  != eit;
	it++) {
		delete it->second;
	}
}

bool FindClassDataMembers::VisitCXXMethodDecl(CXXMethodDecl *md) {
  switch(pass) {
  case 1: {
     
      if (CXXConstructorDecl* cd = dyn_cast<CXXConstructorDecl>(md)) {
      const FunctionDecl* fd = NULL;
      cd->getBody(fd);

      if (cd->hasBody()) {
        _constructorStmt = cd->getBody();
      }

    }
    break;
   } 
   default: break;
 }
return true;
}

bool FindClassDataMembers::VisitFieldDecl(FieldDecl* fd) {

  switch(pass) {
  case 1: {
		QualType q = fd->getType(); 

   // FIXME: We may want to ignore a subset ...?
   //  if (q.getAsString() == "class sc_core::sc_event"){
     if (IdentifierInfo *info = fd->getIdentifier()) {
             //_os << "\n+ Name: " << info->getNameStart();
             //_os << "\n+ Type: 

		 	string tmp = returnSocketType(q.getAsString());
			if((tmp == "tlm_utils::simple_initiator_socket") || (tmp == 
			"tlm_utils::tlm_initiator_socket") || 
			(tmp=="tlm_utils::tlm_target_socket") || (tmp == 
			"tlm_utils::simple_initiator_socket_tagged") || (tmp == 
			"tlm_utils::simple_target_socket") ||(tmp == 
			"tlm_utils::simple_target_socket_tagged") || (tmp == 
			"tlm_utils::passthrough_target_socket") || (tmp == 
			"tlm_utils::passthrough_target_socket_tagged") || (tmp == 
			"tlm_utils::multi_passthrough_initiator_socket") || (tmp == 
			"tlm_utils::multi_passthrough_target_socket") || 
(tmp=="tlm_utils::tlm_analysis_port"))
			{
						FindSocketRegisterMethods findSocketRegisterMethods(_d, _os);
						SocketCharacteristics::registerCallbackMethods tmpVector = 
						findSocketRegisterMethods.getCallbackMethods();
						SocketCharacteristics *sc = new SocketCharacteristics(tmp, tmpVector);
						_socketCharacteristicsMap.insert(socketCharacteristicsPairType(info->getNameStart(), 
						sc));
	 		}
			_inClassMembers.insert(kvType(info->getNameStart(), fd));
     }
   } 	
 	} 
  return true;
}

bool FindClassDataMembers::VisitIntegerLiteral(IntegerLiteral *il) {

	if(il->getValue().getLimitedValue() == 0) {
		return true;
	}
	unsigned int size;
	switch(pass) {
		case 2: {
			//il->dump();
			size = il->getValue().getLimitedValue();

			if(_classMemberSize.find(_memberName) != _classMemberSize.end()) {				
				classMemberSizeMapType::iterator found = _classMemberSize.find(_memberName);
				found->second.push_back(size);		
		//		_classMemberSize.erase(_memberName);
				_classMemberSize.insert(classMemberSizePairType(_memberName, found->second));
			}
			else {
				vector<unsigned int> tmp;
				tmp.push_back(size);
				_classMemberSize.insert(classMemberSizePairType(_memberName, tmp));
			}
			break;
		}
		default: break;
	}
         		
	return true;
}


bool FindClassDataMembers::VisitBinaryOperator(BinaryOperator *b) {

	switch(pass) {
  	case 2: {
			if(MemberExpr *me = dyn_cast<MemberExpr>(b->getLHS())) {
				_memberName = me->getMemberNameInfo().getAsString();	
//				_classMemberSize.insert(classMemberSizePairType(_memberName, _sizeArray));
			}
			break;					 
     }         
		 default: break;
  }
	return true;
}

string FindClassDataMembers::returnSocketType(string qualType) {

	unsigned found = qualType.find_first_of("<");
	string tmp;

	if(found!= string::npos) {
		tmp.assign(qualType, 0, found);
	}
	else {		
		tmp.clear();	
	}
	return tmp;
}

FindClassDataMembers::classMemberMapType FindClassDataMembers::getInClassMembers() {
  return _inClassMembers;
}

FindClassDataMembers::socketCharacteristicsMapType 
FindClassDataMembers::getSocketCharacteristicsMap() {
	return _socketCharacteristicsMap;
}

string FindClassDataMembers::getDataMembertype(FieldDecl* fd) {
  
   QualType q = fd->getType();

		return q.getAsString();
    //  _os << "\n+ Name: " << info->getNameStart();
  //    _os << "\n+ Type: " << q.getAsString();
  
}

FindClassDataMembers::classMemberSizeMapType FindClassDataMembers::getClassMemberSize() {
  return _classMemberSize;
}

void FindClassDataMembers::dump() {
  _os << "\n ============== FindClassDataMembers  ===============";
  _os << "\n:> Print in-class sc_event data members";
  for(classMemberMapType::iterator vit = _inClassMembers.begin(); vit!= _inClassMembers.end(); vit++) {
    _os << "\n:> name: " << vit->first << ", type: " << getDataMembertype(vit->second) <<", FieldDecl*: " << vit->second ;
  }
	
	_os<<"\n For SC_MODULE : " <<_d->getNameAsString(); 
	for(socketCharacteristicsMapType::iterator it = 
	_socketCharacteristicsMap.begin(), eit = _socketCharacteristicsMap.end();
	it != eit;
	it++) {
		_os <<"\n Socket : " <<it->first;
		SocketCharacteristics *tmp = it->second;
		_os <<"\n Type : " <<tmp->getSocketType();
		_os<<"\n Register Call Back Methods : \n";
		vector<string> tmpVector = tmp->getRegisterCallbackMethods();
		for (int i = 0; i<tmpVector.size(); i++) {
			_os <<tmpVector.at(i)<<" ";
		}
	}
  _os << "\n ============== END FindClassDataMembers ===============";  
}

