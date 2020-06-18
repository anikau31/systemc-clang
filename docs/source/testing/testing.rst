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

The boilerplate allows one to integrate tests into the build environment for systemc-clang.

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

SystemC model to parse
""""""""""""""""""""""

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


Build the AST
""""""""""""""
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

If the SystemC model has compile errors, then they will be shown on the standard output.
Otherwise, ``from_ast`` will point to the AST of the SystemC model.

Running systemc-clang
"""""""""""""""""""""

Now that we have access to the AST, we can run systemc-clang.
Inline comments describe each of the lines.

.. code-block:: c++
  :linenos:

  // Instantiate consumer.
  SystemCConsumer consumer{from_ast};

  // Run systemc-clang.
  consumer.HandleTranslationUnit(from_ast->getASTContext());

  // Get access to the internal model.
  Model *model{consumer.getSystemCModel()};

  // Get instance with name "counter_instance".
  ModuleDecl *test_module{model->getInstance("counter_instance")};

The key observation is that ``model`` provides access to the parsed information.
This includes all the modules that were instantiated, the ports and signals within it, any nested sub-modules, etc. 

Writing assertions for parsed information
""""""""""""""""""""""""""""""""""""""""

The test can use ``REQIRE()`` macro from Catch2 to assert for the information found about the model.
The following assertion ensures that we are able to find an instance of a SystemC module that has the name ``counter_instance``.

.. code-block:: c++
  :linenos:

    REQUIRE(test_module != nullptr);

We can also check for the number of input ports found by systemc-clang, and likewise, the number of output ports.

.. code-block:: c++
  :linenos:

    auto input_ports{test_module->getIPorts()};
    REQUIRE(input_ports.size() == 1);

Similarly, using other member functions of the systemc-clang's ``Model`` class, one can access other components of a module instance.

.. code-block:: c++
  :linenos:

    auto output_ports{test_module->getOPorts()};
    REQUIRE(output_ports.size() == 1);

