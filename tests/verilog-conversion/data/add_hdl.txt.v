interface topadd2_sc_module_0_interface;
  logic [0:0] clk;
  logic signed[31:0] in_port_1;
  logic signed[31:0] in_port_2;
  logic signed[31:0] out_port;
  modport port0(
    input clk,in_port_1,in_port_2,
    output out_port
  );
endinterface

module topadd2_sc_module_0 (
 topadd2_sc_module_0_interface.port0 itf
);
  always_ff @(posedge itf.clk) begin: topEntry
    
    
    itf.out_port <= (itf.in_port_1) + (itf.in_port_2);
  end
endmodule