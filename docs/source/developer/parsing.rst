Identifying SystemC constructs
==============================

SystemC Module Declarations
---------------------------

There are two ways in which a SystemC module can be declared: (1) using the ``SC_MODULE()`` macro or (2) inheriting from the ``sc_module`` class.
Both of these approaches are recognized by systemc-clang.

.. code-block:: c++
  :linenos:

  #include <systemc.h>
  SC_MODULE(counter) {
  // Some other code here.

  };

.. code-block:: c++
  :linenos:

  #include <systemc.h>
  class counter: public sc_module {
  // Some other code here.

  };


SystemC Constructor
-------------------
There are two ways in which the constructor can be used: (1) using the ``SC_CTOR()`` macro or (2) using the class constructor. 
Both of approaches are recognized by systemc-clang.

.. code-block:: c++
  :linenos:

  #include <systemc.h>
  SC_MODULE(counter) {
    sc_uint<32> keep_count;
    SC_CTOR(counter): keep_count{0} {
    // Some other code here.
    }
  };

.. code-block:: c++
  :linenos:

  #include <systemc.h>
  class counter: public sc_module {
    sc_uint<32> keep_count;
    counter(): keep_count{0}  {
    // Some other code here.
    }
  };


SystemC Processes and Sensitivity Lists
---------------------------------------

There are two process types that systemc-clang recognizes: (1) ``SC_METHOD()`` and  (2) ``SC_THREAD()``.
The only synthesizable process type is ``SC_METHOD()``. 
systemc-clang gives access to the function provided as the argument to the process macro.
Plugins can then use tihs to perform other operations on the implementation of the process' function such as generating Verilog or an intermediate representation.

.. code-block:: c++
  :linenos:

  #include <systemc.h>
  SC_MODULE(counter) {
    sc_in_clk clk;
    sc_uint<32> keep_count;

    SC_CTOR(counter): keep_count{0} {
      SC_METHOD(count_up);
      sensitive << clk.pos();
    }

    void count_up() {
    // Some code here.
    }

  };

.. code-block:: c++
  :linenos:

  #include <systemc.h>
  class counter: public sc_module {
    sc_in_clk clk;
    sc_uint<32> keep_count;

    SC_HAS_PROCESS(counter);
    counter(): keep_count{0}  {
      SC_METHOD(count_up);
      sensitive << clk.pos();
    }

    void count_up() {
    // Some code here.
    }
  };

SystemC Ports and Member Variables 
----------------------------------

SystemC ports and member fields can be a part of the module. In the counter example, there is a ``clk`` port of type ``sc_in_clk`` and there is a member variable called ``keep_count`` of type ``sc_uint<32>``. Note that the latter is a templated type. We extend this example further.

Note that a member variable can also be an ``sc_signal<>``, which we show in the next subsection.

.. code-block:: c++
  :linenos:

  #include <systemc.h>
  SC_MODULE(counter) {
    // clock
    sc_in_clk clk;

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

Nested Module Declarations & Port Binding
------------------------------------------

It is common to use a hierarchy of modules to describe a design. systemc-clang identifies this hierarchy.
For the example below, the nested module of type ``counter`` will be identified as being nested within the SystemC module of type ``DUT``. 
Within the constructor of the ``DUT`` module, we are instantiating the ``count`` member variable with a given name, and then binding the ports to the appropriate signals. 

This example also shows the use of a ``sc_signal<>``. 

.. code-block:: c++
  :linenos:

  #include <systemc.h>
  // Code from before
  SC_MODULE(counter) {
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
  };


Templated Modules
-----------------

The SystemC module declaration can be templated as well. We can extend our ``counter`` module to have it access a template argument.

.. code-block:: c++
  :linenos:

  #include <systemc.h>
  template <typename T>
  SC_MODULE(counter) {
    // clock
    sc_in_clk clk;

    // output port
    sc_out<T> count_out;

    // member variable
    T keep_count;

    SC_CTOR(counter) {
      SC_METHOD(count_up);
      sensitive << clk.pos();
    }

    void count_up() {
      // Ensure that type T supports +
      keep_count = keep_count + 1;
      count_out.write( keep_count );
    }

  };

User-defined Templated Datatypes
--------------------------------

.. note:: Todo

User-defined Channels
---------------------

.. note:: Todo

Typedefs and ``using``
----------------------

Typedefs and the use of type alias via ``using`` are parsed by systemc-clang. 
The parsing drills down to the most basic type, and desugars any sugared type. 
This means that any intermediate type (via indirection) is not captured, but only the final desugared type.
