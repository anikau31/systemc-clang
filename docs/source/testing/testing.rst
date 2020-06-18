Testing
=======

systemc-clang uses `Catch2 <https://github.com/catchorg/Catch2>`_ to implement tests.
There are several examples under ``tests/`` that can be viewed.
This part of the documentation will go through creating a simple test, and integrating it into the build system.

Unit test for ``counter`` example
----------------------------------

In order to run systemc-clang from within a C++ program, and integrate it successfully into the build environment, we describe a few boilerplate items that are essential.

Boilerplate
^^^^^^^^^^^

Include files
"""""""""""""

We describe the ``#include`` files that are necessary.
We must include the main header file from where we can invoke the ``SystemCConsumer`` called ``SystemCClang.h``. 
We also include the ``catch.hpp``, which is necessary to use the features of Catch2.
Finally, there is an include file that is automatically generated called ``ClangArgs.h``. This is necessary to pass the arguments provided to the build environment to the test. 

.. code-block:: c++
  :linenos:

  #include "SystemCClang.h"
  #include "catch.hpp"

  // This is automatically generated from cmake.
  #include "ClangArgs.h"

Namespace to use
""""""""""""""""

systemc-clang is wrapped in a namespace called ``scpar``. Hence, we add the namespace. 

.. code-block:: c++
  :linenos:

  using namespace scpar;

The next step is in providing the SystemC source that we wish to parse. 
This can come from a file or embedded within the test program as a string. 
Note that this uses the ``R"`` approach to defining a ``std::string``.

.. code-block:: c++
  :linenos:

    std::string code = R"(
    #include <systemc.h>
    SC_MODULE(counter) {
      // clock
      sc_clk_in clk;

      // output port
      sc_out<sc_uint<32>> count_out;

      // member variable
      sc_uint<32> keep_count;

      SC_CTOR(counter) {
        SC_METHOD(count_up);
        sensitive << clk.pos();
      }

      void count_up() {
        keep_count = keep_count + 1;
        count_out.write( keep_count );
      }

    };

    // Top level module.
    SC_MODULE(DUT) {
      counter count;
      sc_clk clock;
      sc_signal< sc_uint<32> > counter_out;

      SC_CTOR(DUT): count{"counter_instance"} {
        // port bindings
        count.clk(clock);
        count.count_out(counter_out);
      };
    };

    int sc_main() {
      DUT dut{"design_under_test"};
      return 0;
    };
   )";

We can start creating our test using Catch2's ``TEST_CASE()``.
Please refer to the Catch2 documentation the usage of the specific Catch2 macros.
They key systemc-clang aspect we point out is that the tests create the AST from within the C++ program using ``ASTUnit`` as shown below.
The arguments to ``buildASTFromCodeWithArgs()`` include the string created earlier, and the arguments to successfully create the AST. 
For example, the location of the SystemC header files.
These are captured in ``systemc_clang::catch_test_args`` as a part of the ``ClangArgs.h``.

.. code-block:: c++
  :linenos:

  TEST_CASE("Basic parsing checks", "[parsing]") {
    ASTUnit *from_ast = 
      tooling::buildASTFromCodeWithArgs(code, systemc_clang::catch_test_args)
        .release();
            // Some more code here
  }




