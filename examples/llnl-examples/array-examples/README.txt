Known issues that may not be synthesizable are:
1) Use of SC_CTHREAD
2) Use of sc_vector
 
In the short term, these constructs could be commented out to test parsing arrays of streams. The code is not really that far from being synthesizable. Refinement is one of the strengths of SystemC.
 
Given the known issues, the example should still be useful for demonstrating arrays of sc_stream ports. The "axi_tc_*” type used for ports is just a typedef for a specialized sc_stream type.
