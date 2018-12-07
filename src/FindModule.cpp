#include "FindModule.h"
#include "FindTemplateTypes.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/Type.h"
#include "clang/Basic/SourceManager.h"

using namespace scpar;

string FindModule::getModuleName() {
	return _moduleName;
}

FindModule::FindModule (CXXRecordDecl *d, llvm::raw_ostream &os):
  _decl(d),
  _os (os),
  _isSystemCModule(false) {
	if (d->hasDefinition () == true) {
			TraverseDecl(d);
		}
}

 
bool FindModule::VisitCXXRecordDecl (CXXRecordDecl *d) {
	if (_decl->getNumBases () <= 0) {
			return true;
		}

	for (CXXRecordDecl::base_class_iterator bi = _decl->bases_begin(),
         be = _decl->bases_end(); bi != be; ++bi)		{
			QualType q = bi->getType();
			string baseName = q.getAsString();

			if (baseName == "::sc_core::sc_module"
          || baseName == "sc_core::sc_module"
					|| baseName == "class sc_core::sc_module")		{

					_isSystemCModule = true;
					IdentifierInfo *info = _decl->getIdentifier();

					if (info != NULL)	{
							_moduleName = info->getNameStart();
						}
				}
		}

	if (_isSystemCModule == false)	{
			return true;
		}

	return false;
}

FindModule::~FindModule() {
  _decl = nullptr;
}


// Member functions

bool FindModule::isSystemCModule() const{
			 return _isSystemCModule;
} 
  
void FindModule::printSystemCModuleInformation() {
	_os << "\n ============== FindModule ===============";
	_os << "\n:> module name: " << _moduleName << ", CXXRecordDecl*: " <<
		_decl << ", isSCModule: " << _isSystemCModule;
	_os << "\n ============== END FindModule ===============";
}
