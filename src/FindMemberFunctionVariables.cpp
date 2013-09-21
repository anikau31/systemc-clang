#include "FindMemberFunctionVariables.h"
#include "llvm/Support/raw_ostream.h"

using namespace scpar;
using namespace std;

FindMemberFunctionVariables::FindMemberFunctionVariables(CXXMethodDecl *d, llvm::raw_ostream &os)
:_func(d), 
_os(os) {
    
      TraverseDecl(d);
       
  }

FindMemberFunctionVariables::~FindMemberFunctionVariables() {

	_functionVarMap.clear();	

}

bool FindMemberFunctionVariables::VisitVarDecl(VarDecl *v) {

	bool duplicateFound = false;

	string funcName = _func->getNameInfo().getAsString();
	string variableName = v->getNameAsString();
	string type = v->getType().getAsString();
	
	if(_functionVarMap.find(funcName) != _functionVarMap.end()) {
		functionVarMapType::iterator index = _functionVarMap.find(funcName);
		vector<string> tmp = index->second;
		
		for (unsigned int i = 0; i<tmp.size();i++) {
			if(tmp.at(i) == variableName) {
				duplicateFound = true;
				break;
			}		
		}

		if(duplicateFound == false) {
			_varTypeMap.insert(varTypePair(variableName, type));
			index->second.push_back(variableName);
		}
	
	_functionVarMap.insert(functionVarPairType(funcName, index->second));	
	}
	else {
		vector<string> tmp;
		_varTypeMap.insert(varTypePair(variableName, type));
		tmp.push_back(variableName);
		_functionVarMap.insert(functionVarPairType(funcName, tmp));
	}
	return true;
}

FindMemberFunctionVariables::functionVarMapType FindMemberFunctionVariables::returnFunctionVarMap() {
	return _functionVarMap;
}
void FindMemberFunctionVariables::dump() {
	for (functionVarMapType::iterator it = _functionVarMap.begin(), eit = _functionVarMap.end();
	it != eit;
	it++) {
		_os <<"\n Function Name : " <<it->first;
		_os <<"\n Local variables : ";
		for (int i = 0; i<it->second.size(); i++){
			_os <<it->second.at(i)<<" ";
		}
		_os <<"\n";
	}
}
