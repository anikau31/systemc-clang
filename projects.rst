Project Ideas
##############

We list some interesting projects that could help improve ``systemc-clang``.  If you are willing to contribute then please contact us. 

* *Re-order assignments in combinational processes*
  ** A combinational SystemC process is one whose sensitivity list has all signals that are on the right-hand side of assignment statements. The SystemC scheduler would re-evaluate this process whenever any of the signals on the sensitivity list change. Although this models how hardware works, it results in multiple re-evaluations of the process leading to inefficiency in simulation. 
  ** This project would analyze combinational SystemC processes and reorder the statements such that it reduces the need for re-evaluating the process. 
