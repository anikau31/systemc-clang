#include "SCModules.h"
#include "FindModule.h"
using namespace scpar;

SCModules::SCModules(TranslationUnitDecl* tuDecl, llvm::raw_ostream& os):
  _os(os) {
  assert(!(tuDecl == NULL));
  TraverseDecl(tuDecl);
}

bool SCModules::VisitCXXRecordDecl(CXXRecordDecl *cxxDecl) {
  FindModule mod(cxxDecl, _os);

  if (!mod.isSystemCModule()) {
    return true;
  }

	//mod.printSystemCModuleInformation();
  /// This is a systemc module.
 // mod.printSystemCModuleInformation();
  string modName = mod.getModuleName();
  if(!((modName == "tlm_req_rsp_channel" || modName == 
	"tlm2_base_protocol_checker" || modName == "tlm_slave_to_transport" || modName == "tlm_transport_channel" || modName == "tlm_transport_to_master" || modName == "sc_event_queue"))) {
	_moduleMap.insert(kvType(modName, cxxDecl));
	}
  return true;
}

SCModules::moduleMapType SCModules::getSystemCModulesMap() {
  return _moduleMap;
}

void SCModules::printSystemCModulesMap() {
  _os << "\n================= SCModules ================";  
  _os << "\n Print SystemC Module Map";
  for (moduleMapType::iterator mit = _moduleMap.begin();
       mit != _moduleMap.end(); mit++) {
    _os << "\n:> name: " << mit->first << ", CXXRecordDecl*: " << mit->second;
  }
  _os << "\n================= END SCModules ================";
  _os << "\n\n";  
}
