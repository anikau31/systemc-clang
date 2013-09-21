#ifndef _FIND_PORT_TRACING_H_
#define _FIND_PORT_TRACING_H_

#include <map>
#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "FindTemplateTypes.h"

namespace scpar {
  using namespace clang;
  using namespace std;
  
  class FindPortTracing: public RecursiveASTVisitor<FindPortTracing> {
  public:
    /// typedefs
		typedef pair <string, vector<string> > portTracingPairType;
		typedef map <string, vector<string> > portTracingMapType;

    FindPortTracing(CXXMethodDecl*, llvm::raw_ostream&);
    ~FindPortTracing();

		virtual bool VisitBinaryOperator(BinaryOperator *bo);
    void dump();
		portTracingMapType getPortTraceMap();
  private:
    llvm::raw_ostream &_os;
   	portTracingMapType _portTracingMap;
    //ReflectionContainerClass * _reflectionContainer;
  };
}
#endif
