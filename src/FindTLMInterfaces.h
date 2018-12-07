#ifndef _FIND_TLM_INTERFACES_H_
#define _FIND_TLM_INTERFACES_H_

#include <map>
#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "FindTemplateTypes.h"

namespace scpar
{
	using namespace clang;
	using namespace std;

	class FindTLMInterfaces:public RecursiveASTVisitor < FindTLMInterfaces >
	{
  public:
		// / typedefs
		typedef map < string, FindTemplateTypes * >interfaceType;
		typedef pair < string, FindTemplateTypes * >kvType;

		FindTLMInterfaces(CXXRecordDecl *, llvm::raw_ostream &);
    virtual ~FindTLMInterfaces();
		
    virtual bool VisitFieldDecl(FieldDecl *);
  
    // ANI : Identifying only FIFO interfaces. 
    // Need to add the other interfaces
		interfaceType getInputInterfaces();
		interfaceType getOutputInterfaces();
		interfaceType getInputOutputInterfaces();

		void dump();

  private:
    llvm::raw_ostream & _os;
		interfaceType _inInterfaces;
		interfaceType _outInterfaces;
		interfaceType _inoutInterfaces;

	};
}
#endif
