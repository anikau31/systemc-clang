Generating HDL
==================
The HDL plugin translates a synthesizable subset of SystemC constructs to `Hcode`, a simple hardware description language neutral
representation. `Hcode` can be easily transcribed to Verilog or VHDL.

The root for HDL generation is the top level module, which is expected
to be the first module in the sc_module list constructed by the front
end. Hcode for each module instantiation is generated, to include ports,
signals, variables, sc_methods. Subsequently, sub-modules are instantiated, and
hcode for each sub-module instantiation is generated. Finally,
user-defined (possibly templated) types are generated.

Installation
^^^^^^^^^^^^^^

To build the HDL plugin for systemc-clang, a top level `cmake` with `-DHDL=ON` should
be performed.  The HDL plugin is compiled into systemc-clang using a
specialization of the driver-tooling pattern.

Limitations and special cases
^^^^^^^^^^^^^^^^^^^^^^^^^^^^
* Switch case clauses must have one statement, which can be a compound statement "{}"
 * User types should not be in the SystemC core namespace or have `sc_` prefix. We assume those qualifiers are part of the SystemC
   simulation library.
* Assignment operators for user-defined structs should follow the normal
  semantics of copying all the fields. This behavior is assumed and
  not checked.
