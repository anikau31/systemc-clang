#ifndef _FIND_MODULE_H_
#define _FIND_MODULE_H_

#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"

namespace scpar {

	using namespace clang;
	using namespace std;

	class FindModule: public RecursiveASTVisitor < FindModule > {
	public:
		FindModule(CXXRecordDecl *, llvm::raw_ostream &);
		virtual bool VisitCXXRecordDecl(CXXRecordDecl *decl);

    virtual ~FindModule();
    
		void printSystemCModuleInformation();
		string getModuleName();

 		bool isSystemCModule() const;
    
	private:
    CXXRecordDecl * _decl;
    llvm::raw_ostream & _os;
		bool _isSystemCModule;
		string _moduleName;

	};
}
#endif
