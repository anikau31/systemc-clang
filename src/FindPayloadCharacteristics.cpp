#include "FindPayloadCharacteristics.h"
#include "FindTemplateTypes.h"
#include "clang/AST/PrettyPrinter.h"
#include "FindArgument.h"
using namespace scpar;
using namespace std;
//////////////////////////////////////////////////////////////////////////////////////////////
Payload::Payload() {
}

Payload::Payload(string command, string address, string dataPtr, string 
dataLength, string streamingWidth, string byteEnablePtr, string DMIAllowed, 
string responseStatus) 
:_command(command)
 ,_address(address)
 ,_dataPtr(dataPtr)
 ,_dataLength(dataLength)
 ,_streamingWidth(streamingWidth)
 ,_byteEnablePtr(byteEnablePtr)
 ,_DMIAllowed(DMIAllowed)
 ,_responseStatus(responseStatus)
{

}

Payload::~Payload() {

	_command.clear();
	_address.clear();
	_dataPtr.clear();
	_dataLength.clear();
	_streamingWidth.clear();
	_byteEnablePtr.clear();
	_DMIAllowed.clear();
	_responseStatus.clear();
}

void Payload::dump(raw_ostream& os) {
	
	os<<"\n Command : " <<_command<<"\n Address : " <<_address<<"\n Data Pointer : "<<_dataPtr<<"\n Data Length : " <<_dataLength<<"\n Streaming Width : "<<_streamingWidth<<"\n Byte Enable Pointer : " <<_byteEnablePtr<<"\n DMI Allowed : " <<_DMIAllowed<<"\n Response Status : " <<_responseStatus;

}

/////////////////////////////////////////////////////////////////////////////////////////////
FindPayloadCharacteristics::FindPayloadCharacteristics(CXXMethodDecl* d, llvm::raw_ostream &os)
  :_os(os)
	, _d(d) 
	{
  TraverseDecl(_d);
}

FindPayloadCharacteristics::~FindPayloadCharacteristics() {
	
	for (payloadCharacteristicsMapType::iterator it = _payloadCharacteristicsMap.begin(),
	eit = _payloadCharacteristicsMap.end();
	it != eit;
	it++) {
		//it->first.clear();
		delete it->second;
	}
}

bool FindPayloadCharacteristics::VisitCXXMemberCallExpr(CXXMemberCallExpr *ce) {
	
	if(MemberExpr *me = dyn_cast<MemberExpr>(ce->getCallee()->IgnoreImpCasts())) {
		if(DeclRefExpr *de = dyn_cast<DeclRefExpr> (me->getBase()->IgnoreImpCasts())) {
			Payload *p = new Payload(_command, _address, _dataPtr, _dataLength, 
			_streamingWidth, _byteEnablePtr, _DMIAllowed, _responseStatus);
			if(_payloadCharacteristicsMap.find(de->getFoundDecl()->getNameAsString()) == 
			_payloadCharacteristicsMap.end()) {
				_payloadName = de->getFoundDecl()->getNameAsString();								
				_payloadCharacteristicsMap.insert(payloadCharacteristicsPairType(_payloadName, 
				p));					
			}
			else {				
				_payloadCharacteristicsMap.erase(_payloadName);
				_payloadCharacteristicsMap.insert(payloadCharacteristicsPairType(_payloadName, 	
				p));					
			}	
		}
	}

	if(ce->getMethodDecl()->getNameAsString() == "set_command") {	
		FindArgument fa(ce->getArg(0)->IgnoreImpCasts());
		_command = fa.getArgumentName();
	}

	else if (ce->getMethodDecl()->getNameAsString() == "set_address") {
		FindArgument fa(ce->getArg(0)->IgnoreImpCasts());
		_address = fa.getArgumentName();			
	}

	else if(ce->getMethodDecl()->getNameAsString() == "set_data_ptr") {
		FindArgument fa(ce->getArg(0)->IgnoreImpCasts());
		_dataPtr = fa.getArgumentName();
	}

	else if(ce->getMethodDecl()->getNameAsString() == "set_data_length") {
		FindArgument fa(ce->getArg(0)->IgnoreImpCasts());
		_dataLength = fa.getArgumentName();
	}

	else if(ce->getMethodDecl()->getNameAsString() == "set_streaming_width") {
		FindArgument fa(ce->getArg(0)->IgnoreImpCasts());
		_streamingWidth = fa.getArgumentName();
	}
	
	else if(ce->getMethodDecl()->getNameAsString() == "set_byte_enable_ptr") {		
		FindArgument fa(ce->getArg(0)->IgnoreImpCasts());
		_byteEnablePtr = fa.getArgumentName();
	}

	else if(ce->getMethodDecl()->getNameAsString() == "set_dmi_allowed") {
		FindArgument fa(ce->getArg(0)->IgnoreImpCasts());
		_DMIAllowed = fa.getArgumentName();
	}
	
	else if (ce->getMethodDecl()->getNameAsString() == "set_response_status") {
		FindArgument fa(ce->getArg(0)->IgnoreImpCasts());
		_responseStatus = fa.getArgumentName();
	}

	return true;
}

FindPayloadCharacteristics::payloadCharacteristicsMapType FindPayloadCharacteristics::returnPayloadCharacteristicsMap() {
	return _payloadCharacteristicsMap;
}

void FindPayloadCharacteristics::dump(raw_ostream& os) {
	for (payloadCharacteristicsMapType::iterator it = _payloadCharacteristicsMap.begin(),
	eit = _payloadCharacteristicsMap.end();
	it != eit;
	it++) {
		_os <<"\n Payload : " <<it->first;
		it->second->dump(os);
	}
}
