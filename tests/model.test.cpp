#include "catch.hpp"

#include "Model.h"
using namespace systemc_clang;

TEST_CASE( "systemc-clang Model copying tests", "[model]") {

  Model model_;

  SECTION( "checking constructor defaults") {
    REQUIRE( model_.getModuleDecl().size() == 0 );
  }

}
