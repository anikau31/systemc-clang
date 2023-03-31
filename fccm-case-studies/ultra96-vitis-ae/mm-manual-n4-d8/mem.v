module mem #
    (
    	parameter WIDTH = 32,
    	parameter DEPTH = 512
    )
	(
    input rst,
    input clkA,
    input clkB,
    input weA,
    input enA,
    input enB,
    input [$clog2(DEPTH)-1:0] addrA,
    input [$clog2(DEPTH)-1:0] addrB,
    input [WIDTH-1:0] dinA,
    output reg [WIDTH-1:0] doutB
  );

	reg [WIDTH-1:0] mem [0:DEPTH-1];

`ifndef SYNTHESIS
  integer r;
  initial begin
    for(r=0;r<DEPTH-1;r=r+1) begin
      mem[r] <= {WIDTH{1'b0}};
    end
  end
`endif

	always @(posedge clkA) begin
		if(enA) begin
			if(weA) begin
				mem[addrA] <= dinA;
			end
		end
	end

	always @(posedge clkB) begin
    if(rst) begin
      doutB <= {WIDTH{1'b0}};
    end else if(enB) begin
			doutB <= mem[addrB];
		end
	end

endmodule
