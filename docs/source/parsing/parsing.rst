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

Nested Module Declarations
^^^^^^^^^^^^^^^^^^^^^^^^^^

It is common to use a hierarchy of modules to describe a design. systemc-clang identifies this hierarchy.
For the example below, the nested module of type ``counter`` will be identified as being nested within the SystemC module of type ``DUT``. 

.. code-block:: c++
  :linenos:

  #include <systemc.h>
  SC_MODULE(counter) {
  // Some other code here.
  };

  SC_MODULE(DUT) {
    counter count;

    SC_CTOR(DUT): count{"counter_instance"} {}; 
  };



SystemC Constructor
-------------------
There are two ways in which the constructor can be used: (1) using the ``SC_CTOR()`` macro or (2) using the class constructor when an initialization for member fields is required.
Both of approaches are recognized by systemc-clang.

.. code-block:: c++
  :linenos:

  #include <systemc.h>
  SC_MODULE(counter) {
    SC_CTOR(counter) {
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

.. code-block:: c++
  :linenos:

  #include <systemc.h>
  SC_MODULE(counter) {
    SC_CTOR(counter) {
    SC_METHOD(count_up);
    sensitive << clk.pos();
    }

  };


SystemC Sensitivity List
-------------------------

