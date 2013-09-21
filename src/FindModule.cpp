#include "FindModule.h"
#include "FindTemplateTypes.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/Type.h"
#include "clang/Basic/SourceManager.h"

using namespace scpar;

string FindModule::getModuleName() {
  return _moduleName;
}

string FindModule::getTransportInterface() {
	return _transportInterface;
}

FindModule::FindModule(CXXRecordDecl *d, llvm::raw_ostream &os): _decl(d), _os(os),_isSystemCModule(false) {
  if (d->hasDefinition() == true) {
    //    _os << "\n ============== FindModule ===============";
    TraverseDecl(d);

  }
}

bool FindModule::VisitCXXRecordDecl(CXXRecordDecl *d) {
//  _os <<"\n Dumping CXXRecordDecls \n";
//	d->dump();
	
	if (_decl->getNumBases() <= 0 ) {
    return true;
  }

  /// There is a base class.  We are looking for sc_module as base class.
  for (CXXRecordDecl::base_class_iterator bi = _decl->bases_begin(), be = _decl->bases_end();
       bi != be; ++bi) {
    QualType q = bi->getType();
    //    _os << "\n+ Base class: " << q.getAsString();

    string baseName = q.getAsString();
		//_os <<"\n Base name : " <<baseName;
   if (baseName == "::sc_core::sc_module" || baseName == "sc_core::sc_module" || baseName == "class sc_core::sc_module" || baseName == "tlm::tlm_bw_transport_if<>" || baseName == "tlm::tlm_fw_transport_if<>") {
      _isSystemCModule = true;    
		  IdentifierInfo *info = _decl->getIdentifier();
  		if (info != NULL) {
     //   _os << "\n+ sc_module name: " << info->getNameStart();				
    		_moduleName = info->getNameStart();
				if(baseName == "tlm::tlm_bw_transport_if<>") {
					_transportInterface = "Backward";	
				}
				else if(baseName == "tlm::tlm_fw_transport_if<>") {
					_transportInterface = "Forward";	
				}
  		}
		}
  }

  if (_isSystemCModule == false) {
    return false;
  }

  /// This is an sc_module class


  return false;
}

bool FindModule::isSystemCModule() const {
	return _isSystemCModule;
}

void FindModule::printSystemCModuleInformation() {
  _os << "\n ============== FindModule ===============";
  _os << "\n:> module name: " << _moduleName << ", CXXRecordDecl*: " << _decl << ", isSCModule: " << _isSystemCModule;
	if(_transportInterface.size() != 0) {
		_os <<", Transport Interface implemented : " <<_transportInterface;
	}
  _os << "\n ============== END FindModule ===============";
}
