module topadd2_0(
input wire [31:0] in_port_1,
input wire [31:0] in_port_2,
input wire clk,
output reg [31:0] out_port 
);


always @(posedge clk) begin: topEntry

out_port <= (in_port_1) + (in_port_2);
end // topEntry
endmodule // topadd2_0

