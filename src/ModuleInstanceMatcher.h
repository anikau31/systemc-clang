#ifndef _MODULE_INSTANCE_MATCHER_H
#define _MODULE_INSTANCE_MATCHER_H

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang;
using namespace clang::ast_matchers;

class ModuleInstanceMatcher: public MatchFinder::MatchCallback {

  public:
    virtual void run( const MatchFinder::MatchResult &result );
   };


#endif

