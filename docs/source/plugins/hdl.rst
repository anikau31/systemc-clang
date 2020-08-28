Generating HDL
==================
The HDL plugin translates a synthesizable subset of System C
constructs to hcode, a simple hardware description language neutral
representation. Hcode can be easily transcribed to Verilog or VHDL.

The root for HDL generation is the top level module, which is expected
to be the first module in the sc_module list constructed by the front
end. Hcode for each module instantiation is generated, to include ports,
signals, variables, sc_methods. Subsequently, sub-modules are instantiated, and
hcode for each sub-module instantiation is generated. Finally,
user-defined (possibly templated) types are generated.


Installation
------------

To build the HDL systemc-clang, a top level CMake with -DHDL=ON should
be performed.  The HDL plugin is compiled into systemc-clang using a
specialization of the driver-tooling pattern.
