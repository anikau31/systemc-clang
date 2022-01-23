module topadd2_sc_module_0 (
  input logic [0:0] clk,
  input logic signed[31:0] in_port_1,
  input logic signed[31:0] in_port_2,
  output logic signed[31:0] out_port
);
  always_ff @(posedge clk) begin: topEntry
    
    
    out_port <= (in_port_1) + (in_port_2);
  end
endmodule
