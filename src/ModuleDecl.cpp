#include <string>
#include "ModuleDecl.h"

using namespace scpar;
using std::string;

ModuleDecl::ModuleDecl():
  _moduleName("NONE"),_classdecl(NULL) {

}

ModuleDecl::ModuleDecl(const string& name, CXXRecordDecl* decl) :
  _moduleName(name),_classdecl(decl) {

}

void ModuleDecl::setModuleName(const string &name) {
  _moduleName = name;
}

ModuleDecl::portMapType ModuleDecl::getOPorts(){
  return _oports;
}

string ModuleDecl::getName() {
  return _moduleName;
}
ModuleDecl::~ModuleDecl() {

  // Delete all pointers in ports.
  for(ModuleDecl::portMapType::iterator mit = _iports.begin();
      mit != _iports.end();
      mit++) {
    // Second is the PortDecl*.
    delete mit->second;
  }
  _iports.clear();

  for(ModuleDecl::portMapType::iterator mit = _oports.begin();
      mit != _oports.end();
      mit++) {
    // Second is the PortDecl*.
    delete mit->second;
  }
  _oports.clear();

  for(ModuleDecl::portMapType::iterator mit = _ioports.begin();
      mit != _ioports.end();
      mit++) {
    // Second is the PortDecl*.
    delete mit->second;
  }
  _ioports.clear();

  for (ModuleDecl::eventMapType::iterator eit = _events.begin();
       eit != _events.end();
       eit++) {
    delete eit->second;
  }
  _events.clear();

  for(ModuleDecl::dataMemberMapType::iterator dit = _dataMembers.begin();
	dit != _dataMembers.end();
	dit++) {
		delete dit->second;
	}
   _dataMembers.clear();
 
  for(ModuleDecl::classMemberFunctionsMapType::iterator cit = _memberFunctions.begin();
	cit != _memberFunctions.end();
	cit++) {
		delete cit->second;
	}
  _memberFunctions.clear();
}

bool ModuleDecl::isModuleClassDeclNull() {
  return (_classdecl==NULL);
}

CXXRecordDecl* ModuleDecl::getModuleClassDecl() {
  assert(!(_classdecl==NULL));
  return _classdecl;
}
//******************** add functions ***************************//
void ModuleDecl::addEvents(FindEvents::classEventMapType e) {
  for (FindEvents::classEventMapType::iterator eit = e.begin(), eite = e.end();
       eit != eite;
       eit++) {
    string n = eit->first;
    FieldDecl * fd = eit->second;
    EventDecl * ed = new EventDecl(n,fd);
    _events.insert(ModuleDecl::eventPairType(n, ed));
  }
}

void ModuleDecl::addInitiatorTargetSocketBind(FindSocketBinding::initiatorTargetSocketMapType initiatorTargetSocketMap) {

	_initiatorTargetSocketMap = initiatorTargetSocketMap;
}

void ModuleDecl::addSignals(FindSignals::signalMapType* s) {
  for (FindSignals::signalMapType::iterator sit = s->begin();
       sit != s->end();
       sit++) {
    string n = sit->first;
    // It is important to create new objects. This is because the objects created during Find*
    // may go outside scope, and free up allocated memory.
    SignalContainer* sc = new SignalContainer(*sit->second);
    Signal* sig = new Signal(n,sc);
    _signals.insert(ModuleDecl::signalPairType(n, sig));
    
  }
}

void ModuleDecl::addWaitCalls(const string& moduleName, const string& processName, FindWait::waitListType waitTimes, FindWait::waitTimesMapType waitTimesMap, FindWait::processWaitEventMapType processWaitEventMap) {

    WaitCalls* wc = new WaitCalls(processName, waitTimes, waitTimesMap, processWaitEventMap);
		
    _processWaitCall.insert(ModuleDecl::processWaitCallsPairType(moduleProcessPairType(moduleName, processName), wc));
}

void ModuleDecl::addInputPorts(FindPorts::portType p) {
  for(FindPorts::portType::iterator mit = p.begin(), mite = p.end();
      mit != mite;
      mit++) {
    string n = mit->first;
    FindTemplateTypes* tt = new FindTemplateTypes(*mit->second);
    PortDecl* pd = new PortDecl(n, tt);
    _iports.insert(portPairType(mit->first, pd ));
  }
}

void ModuleDecl::addOutputPorts(FindPorts::portType p) {
  for(FindPorts::portType::iterator mit = p.begin(), mite = p.end();
      mit != mite;
      mit++) {
    string n = mit->first;
    FindTemplateTypes* tt = new FindTemplateTypes(*mit->second);
    PortDecl* pd = new PortDecl(n, tt);
    _oports.insert(portPairType(n, pd));
  }
}

void ModuleDecl::addDataMembers(const string& moduleName, 
FindClassDataMembers::classMemberMapType c, 
FindClassDataMembers::classMemberSizeMapType s, raw_ostream &os) {

	for (FindClassDataMembers::classMemberMapType::iterator it = c.begin(),
		eit = c.end();
		it != eit;
		it++) {
		double totalSize = 1;
		string n = it->first;

    if(s.find(n) != s.end()) {
			
    	FindClassDataMembers::classMemberSizeMapType::iterator sizeFound = s.find(n);
			double tmpSize = 1;
      for (unsigned int i = 0; i<sizeFound->second.size();i++) {
		     tmpSize = tmpSize*sizeFound->second.at(i);
			}
				 totalSize = totalSize + tmpSize;
      
			string dataType;

			if(it->second->getType().getTypePtr()->isPointerType()) {
				dataType = it->second->getType().getTypePtr()->getPointeeType().getAsString();
			}
			else {		
				dataType = it->second->getType().getAsString();
			}

		if((dataType == "int") || (dataType == "float") || (dataType == "unsigned int") || (dataType == "sc_in<int>") || (dataType == "sc_out<int>") || (dataType == "sc_inout<int") || (dataType == "sc_in<float>") || (dataType == "sc_out<float>") || (dataType == "sc_inout<float>") || (dataType == "sc_in<unsigned int>") || (dataType == "sc_out<unsigned int>") || (dataType == "sc_inout<unsigned int>")) {
			totalSize = totalSize * 4;
		}
		else if((dataType == "double") || (dataType == "long") || (dataType == "sc_in<double>") || (dataType == "sc_out<double>") || (dataType == "sc_inout<double>") || (dataType == "sc_in<long>") || (dataType == "sc_out<long>") || (dataType == "sc_inout<long>")) {
			totalSize = totalSize * 8;
		}
		
		ClassDataMembers *d = new ClassDataMembers(n, totalSize, it->second);
		_dataMembers.insert(dataMemberPairType(n, d));	       
		}
		else {
			ClassDataMembers *d = new ClassDataMembers(n, 1, it->second);
			_dataMembers.insert(dataMemberPairType(n, d));
		}
	}
}

void ModuleDecl::addSockets(FindClassDataMembers::socketCharacteristicsMapType 
socketCharacteristicsMap) {

	for (FindClassDataMembers::socketCharacteristicsMapType::iterator it = 
	socketCharacteristicsMap.begin(),
	eit = socketCharacteristicsMap.end();
	it != eit;
	it++) {
		string socketName = it->first;
		string socketType = it->second->getSocketType();
		vector<string> registerCallbackMethods = 
		it->second->getRegisterCallbackMethods();
		Sockets *s = new Sockets(socketName, socketType, registerCallbackMethods);
		_socketsMap.insert(socketsPairType(socketName, s));	
	}
}

void ModuleDecl::addInputOutputPorts(FindPorts::portType p) {
  for(FindPorts::portType::iterator mit = p.begin(), mite = p.end();
      mit != mite;
      mit++) {
    _ioports.insert(portPairType(mit->first,new PortDecl(mit->first,mit->second) ));
  }
}


void ModuleDecl::addMemberFunctions(FindMemberFunctions::functionMapType f) {
	for (FindMemberFunctions::functionMapType::iterator it = f.begin(), eit = f.end();
		it != eit;
		it++) {	
		
		string n = it->first;
		ClassMemberFunctions *c = new ClassMemberFunctions(n, it->second.getEntryMethod()->getMethodDecl());
		_memberFunctions.insert(classMemberFunctionsPairType(n, c));					
	}

}
void ModuleDecl::addProcess(FindEntryFunctions::entryFunctionVectorType* efv) {
  for (unsigned int i = 0; i < efv->size(); i++) {
    
    EntryFunctionContainer* ef = (*efv)[i];
    
    // Set the entry name.
    string entryName = ef->_entryName;
    string entryType = "";
    // Set the process type
    switch(ef->_procType) {
      case THREAD: 
        entryType = "SC_THREAD";
        break;
      case METHOD: 
        entryType = "SC_METHOD";
        break;
      case CTHREAD: 
        entryType = "SC_CTHREAD";
        break;
      default: 
        entryType = "ERROR";
        break;
    }

   _processes.insert(processPairType(entryName, new ProcessDecl(entryType, entryName, ef->_entryMethodDecl, ef->_constructorStmt, ef)));
  }
}
void ModuleDecl::addNotifyCalls(const string& moduleName, const string& processName, FindNotify::notifyCallListType notifyCallList, FindNotify::processNotifyEventMapType processNotifyEventMap) {

    NotifyCalls* nc = new NotifyCalls(processName, notifyCallList, processNotifyEventMap);
		
    _processNotifyCall.insert(ModuleDecl::processNotifyCallsPairType(moduleProcessPairType(moduleName, processName), nc));
}
// ********************** end add functions ***********************************

// ********************** access functions ************************************
ModuleDecl::processWaitCallsMapType ModuleDecl::getProcessWaitCallsMap(){
	return _processWaitCall;
}

ModuleDecl::dataMemberMapType ModuleDecl::getDataMemberMap(){
	return _dataMembers;
}

ModuleDecl::processMapType ModuleDecl::getProcessMap() {
	return _processes;
}

ModuleDecl::eventMapType ModuleDecl::getEventMap() {
	return _events;
}

ModuleDecl::processNotifyCallsMapType ModuleDecl::getProcessNotifyCallsMap(){
	return _processNotifyCall;
}

FindSocketBinding::initiatorTargetSocketMapType ModuleDecl::getInitiatorTargetSocketMap() {
	return _initiatorTargetSocketMap;
}

ModuleDecl::socketsMapType ModuleDecl::getSocketsMap() {
	return _socketsMap;
}
// ********************** access functions ************************************
void ModuleDecl::dumpProcesses(raw_ostream& os, int tabn) {
  if(_processes.size() == 0) {
    os<<"    <<<NULL>>>\n";
  }
  else {
	  for (processMapType::iterator pit = _processes.begin();
	       pit != _processes.end();
	       pit++) {
        
	    ProcessDecl* pd = pit->second;
	    pd->dump(os, tabn);
	    os << "\n";
  	}
  }
  os<<"\n";
}


void ModuleDecl::dumpPorts(raw_ostream& os, int tabn) {

  for (int i = 0; i<tabn; i++) {
    os<<" ";
  }
  os<< "Input ports:\n ";

  if(_iports.size() == 0) {
  	os << "   <<<NULL>>>\n";
  }
  else {
 	 for(ModuleDecl::portMapType::iterator mit = _iports.begin();
	      	mit != _iports.end();
	        mit++) {

	    mit->second->dump(os, tabn);
	    os << "\n ";
  	}
  os<<"\n";
  }

  for (int i = 0; i<tabn; i++) {
    os<<" ";
  }
  os<< "Output ports:\n ";
    if(_oports.size() == 0) {
  	os << "   <<<NULL>>>\n";
  }
  else{
	  for(ModuleDecl::portMapType::iterator mit = _oports.begin();
	      mit != _oports.end();
	      mit++) {

	    mit->second->dump(os,tabn);
       	    os << "\n ";    
  os<<"\n";	}
  }

  for (int i = 0; i<tabn; i++) {
    os<<" ";
  }
  os<< "Inout ports:\n ";

  if(_ioports.size() == 0) {
        os << "    <<<NULL>>>\n";
  }
  else {
	  for(ModuleDecl::portMapType::iterator mit = _ioports.begin();
	      mit != _ioports.end();
	      mit++) {

	    mit->second->dump(os, tabn);
	    os << "\n ";    
	  }
  }
 	os<<"\n";
}
void ModuleDecl::dumpSignals(raw_ostream& os, int tabn) {
  
  if(_signals.size() == 0) {
     os << "     <<<NULL>>>\n";
  }
  else {
	  for (ModuleDecl::signalMapType::iterator sit = _signals.begin();
	       sit != _signals.end();
	       sit++) {
	    Signal * s = sit->second;
	    s->dump(os, tabn);
	    os << "\n";
  	}
  }
 os<<"\n";
}
void ModuleDecl::dumpWaitCalls(raw_ostream& os, int tabn) {
	if (_processWaitCall.size() == 0) {	
     os <<"     <<<NULL>>>\n";
	}
	else {
		for(ModuleDecl::processWaitCallsMapType::iterator it = _processWaitCall.begin(), eit = _processWaitCall.end();
				it != eit;
				it++) {
				moduleProcessPairType tmp = it->first;
				os <<"    Process Name : " <<tmp.second;
				WaitCalls * wc = it->second;
				wc->dump(os, tabn);
				os <<"\n";
			}
	}
	os <<"\n";
}

void ModuleDecl::dumpMemberFunctions(raw_ostream &os, int tabn) {
  if(_memberFunctions.size() == 0) {
     os <<"     <<<NULL>>>\n";
  }
  else {
	  for (ModuleDecl::classMemberFunctionsMapType::iterator it = _memberFunctions.begin();
		it != _memberFunctions.end();
		it++) {
	
			ClassMemberFunctions *c = it->second;
			c->dump(os, tabn);
			os << "\n";
		}
	}
  os<<"\n";
}

void ModuleDecl::dumpEvents(raw_ostream& os, int tabn) {
  if(_events.size() == 0) {
     os <<"     <<<NULL>>>\n";
  }
  else {
	  for (ModuleDecl::eventMapType::iterator eit = _events.begin();
	       eit != _events.end();
       		eit++) {
	    EventDecl * e = eit->second;
	    e->dump(os,tabn);
	    os << "\n";
  	}	
  }
 os<<"\n";
}

void ModuleDecl::dumpClassMembers(raw_ostream& os, int tabn) {
  if(_dataMembers.size() == 0) {
     os <<"     <<<NULL>>>\n";
  }
  else {
	  for(ModuleDecl::dataMemberMapType::iterator it = _dataMembers.begin(), eit = _dataMembers.end();
		it != eit;
		it++) {
			ClassDataMembers *c = it->second;
			c->dump(os,tabn);
			os<<"\n\n";
		}
	}
 os<<"\n";
}

void ModuleDecl::dumpSockets(raw_ostream& os, int tabn) {
	if(_socketsMap.size() == 0) {
		os <<"      <<<NULL>>>\n";
	}
	else {
		for (ModuleDecl::socketsMapType::iterator it = _socketsMap.begin(),
		eit = _socketsMap.end();
		it != eit;
		it++) {
			Sockets *s = it->second;
			s->dump(os, tabn);
			os<<"\n";
		}
	}
	os<<"\n";
}

void ModuleDecl::dumpNotifyCalls(raw_ostream& os, int tabn) {
	if(_processNotifyCall.size() == 0) {		
     os <<"     <<<NULL>>>\n";
	}
	else {
		for(ModuleDecl::processNotifyCallsMapType::iterator it = _processNotifyCall.begin(), eit = _processNotifyCall.end();
			it != eit;
			it++) {
			moduleProcessPairType tmp = it->first;
			os <<"    Process Name : " <<tmp.second;
			NotifyCalls * nc = it->second;
			nc->dump(os, tabn);
			os <<"\n";
		}
	}
	os <<"\n";
}

void ModuleDecl::dumpBindMap(raw_ostream& os, int tabn) {
	if(_initiatorTargetSocketMap.size() == 0) {	
     os <<"     <<<NULL>>>\n";
	}
	else {
		for (FindSocketBinding::initiatorTargetSocketMapType::iterator it = _initiatorTargetSocketMap.begin(), 
		eit = _initiatorTargetSocketMap.end();
		it != eit;
		it++) {
			FindSocketBinding::initiatorTargetPairType tmp = it->first;
			os <<"    Initiator : "<<tmp.first<< " Target : "<<tmp.second<<" Bind object : "<<it->second<<"\n";
		}
	}
}

void ModuleDecl::dump(raw_ostream& os) {

  os <<"\n=======================================================\n";
  os << " ModuleDecl " << this << " " << _moduleName << " CXXRecordDecl " << _classdecl << "\n";

  os <<"  Port Declaration:>\n";  
  dumpPorts(os, 4);
  os <<"  Class Members:> \n";
  dumpClassMembers(os, 4);
  os <<"  Sockets:> \n";
	dumpSockets(os, 4);
	os <<"  Signal Declaration:>\n";
  dumpSignals(os, 4);
  os <<"  Processes:>\n";
  dumpProcesses(os, 4);
  os <<"  Initiator Target and Socket Bind:>\n";
	dumpBindMap(os, 4);
	os <<"  Wait Calls:>\n";
	dumpWaitCalls(os, 4);
	os <<"  Notify Calls:>\n";
	dumpNotifyCalls(os, 4);
  os <<"  Events:>\n";
  dumpEvents(os,4);
  os <<"  Member Functions:>\n";
  dumpMemberFunctions(os, 4);
	
 
  os <<"\n=======================================================\n";
}

