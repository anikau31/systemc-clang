#ifndef _FIND_PORTS_H_
#define _FIND_PORTS_H_

#include <map>
#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "FindTemplateTypes.h"

namespace scpar {
  using namespace clang;
  using namespace std;

  class FindPorts : public RecursiveASTVisitor < FindPorts > {
  public:
    /// typedefs
    typedef map < string, FindTemplateTypes * > PortType;
    typedef pair < string, FindTemplateTypes * >kvType;


    FindPorts( CXXRecordDecl *, llvm::raw_ostream &);
    virtual ~FindPorts();
    virtual bool VisitFieldDecl( FieldDecl * );

    PortType getInputPorts() const;
    PortType getOutputPorts() const;
    PortType getInputOutputPorts() const;
    void dump();

  private:
    llvm::raw_ostream & os_;
    PortType _inPorts;
    PortType _outPorts;
    PortType _inoutPorts;

  };

}
#endif
