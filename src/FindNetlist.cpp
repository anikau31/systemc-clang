#include "FindNetlist.h"
using namespace scpar;

FindNetlist::FindNetlist(FunctionDecl * fnDecl):
_pass(1)
{

	TraverseDecl(fnDecl);
	_pass = 2;
}


FindNetlist::~FindNetlist()
{
 _instanceModuleMap.clear();
 _portSignalMap.clear();
 _instancePortSignalMap.clear();
}

FindNetlist::FindNetlist(const FindNetlist &f) {

 _pass = f._pass;
 _instanceModuleMap = f._instanceModuleMap;
_portSignalMap = f._portSignalMap;
_instancePortSignalMap = f._instancePortSignalMap;
_instanceListModuleMap = f._instanceListModuleMap;
}


void FindNetlist::updateInstanceListModuleMap(string instanceName, string moduleName) {
  if (_instanceListModuleMap.find(moduleName) == _instanceListModuleMap.end()) {
    vector<string> instanceList;
    instanceList.push_back(instanceName);
    _instanceListModuleMap.insert(instanceListModulePairType(moduleName, instanceList));  
  }
  else {
    instanceListModuleMapType::iterator instanceListModuleMapFound = _instanceListModuleMap.find(moduleName);
    vector<string> instanceList = instanceListModuleMapFound->second;
    instanceList.push_back(instanceName);
    _instanceListModuleMap.erase(moduleName);
    _instanceListModuleMap.insert(instanceListModulePairType(moduleName, instanceList));
  }
}
 
bool FindNetlist::VisitCXXOperatorCallExpr(CXXOperatorCallExpr * ce)
{

	bool foundME = false;

	string moduleName;
	string instanceName;
	string portName;
	string signalName;

	for (Stmt::child_iterator it = ce->IgnoreImpCasts()->child_begin(), eit =
		 ce->IgnoreImpCasts()->child_end(); it != eit; it++)
	{
		if (Expr * e = dyn_cast < Expr > (*it))
		{
			if (MemberExpr * me =
				dyn_cast < MemberExpr > (e->IgnoreImpCasts()))
			{
				if (DeclRefExpr * de =
					dyn_cast < DeclRefExpr > (me->getBase()->IgnoreImpCasts()))
				{
					moduleName =
						de->getDecl()->getType().getBaseTypeIdentifier()->
						getName();
					instanceName = de->getFoundDecl()->getNameAsString();
				}
				portName = me->getMemberDecl()->getNameAsString();
				foundME = true;
			}
			if (DeclRefExpr * de =
				dyn_cast < DeclRefExpr > (e->IgnoreImpCasts()))
			{
				if (foundME)
				{
					signalName = de->getNameInfo().getAsString();
					foundME = false;
				}
			}
		}
	}

	if (_instanceModuleMap.find(instanceName) == _instanceModuleMap.end())
	{
		_instanceModuleMap.
			insert(instanceModulePairType(instanceName, moduleName));
		_portSignalMap.clear();
	 updateInstanceListModuleMap(instanceName, moduleName);
 }

	_portSignalMap.insert(portSignalPairType(portName, signalName));


	if (_instancePortSignalMap.find(instanceName) ==
		_instancePortSignalMap.end())
	{
		_instancePortSignalMap.
			insert(instancePortSignalPairType(instanceName, _portSignalMap));
	}
	else
	{
		_instancePortSignalMap.erase(instanceName);
		_instancePortSignalMap.
			insert(instancePortSignalPairType(instanceName, _portSignalMap));
	}

	return true;
}

int FindNetlist::getNumInstances(string moduleName) { 
  int counter = 0;
  for (instanceModuleMapType::iterator it = _instanceModuleMap.begin(), eit = _instanceModuleMap.end();
     it != eit;
     it++) {
   string modName = it->second;
   if (modName == moduleName) {
     counter++;
   }
 }
  return counter;
}

FindNetlist::portSignalMapType FindNetlist::getPortSignalMap() {
 return _portSignalMap;
}

FindNetlist::instanceListModuleMapType FindNetlist::getInstanceListModuleMap() {
 return _instanceListModuleMap;
}

FindNetlist::instanceModuleMapType FindNetlist::getInstanceModuleMap()
{
	return _instanceModuleMap;
}

FindNetlist::instancePortSignalMapType FindNetlist::getInstancePortSignalMap()
{
	return _instancePortSignalMap;
}

void FindNetlist::dump()
{
  llvm::errs() << "\n ----------------- Netlist dump ----------------- \n";

 for (instanceModuleMapType::iterator it = _instanceModuleMap.begin(), eit =
		 _instanceModuleMap.end(); it != eit; it++)
	{
   llvm::errs() << "\n Instance Name : " << it->first << " Module Name : " << it->
			second;
		string instanceName = it->first;
		if (_instancePortSignalMap.find(instanceName) !=
			_instancePortSignalMap.end())
		{
			instancePortSignalMapType::iterator instancePortSignalMapFound =
				_instancePortSignalMap.find(instanceName);
			portSignalMapType portSignalMap =
				instancePortSignalMapFound->second;
			for (portSignalMapType::iterator pit =
				 portSignalMap.begin(), pite = portSignalMap.end();
				 pit != pite; pit++)
			{
     llvm::errs() << "\n Port : " << pit->
					first << " ------> " << pit->second;
			}
		}
		else
		{
    llvm::errs() << "\n No instance name found ////// weird.";
		}
  llvm::errs() << "\n ------------------------------------------------------\n";
	}
}
