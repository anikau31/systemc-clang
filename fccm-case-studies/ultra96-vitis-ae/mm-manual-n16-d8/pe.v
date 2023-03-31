`timescale 1 ps / 1 ps

module pe
#(
    parameter   D_W_ACC  = 64, //accumulator data width
    parameter   D_W      = 32  //operand data width
)
(
    input   wire                    clk,
    input   wire                    rst,
    input   wire                    init,
    input   wire    [D_W-1:0]       in_a,
    input   wire    [D_W-1:0]       in_b,
    output  reg     [D_W-1:0]       out_b,
    output  reg     [D_W-1:0]       out_a,

    input   wire    [(D_W_ACC)-1:0] in_data,
    input   wire                    in_valid,
    output  reg     [(D_W_ACC)-1:0] out_data,
    output  reg                     out_valid
);

wire [2*D_W-1:0] mult_op;
reg [(D_W_ACC)-1:0] out_sum;

reg [(D_W_ACC)-1:0] in_data_r;
reg in_valid_r;

assign mult_op = in_a*in_b;

always @(posedge clk) begin
    if(rst) begin
        out_a   <= {D_W{1'b0}};
        out_b   <= {D_W{1'b0}};
        out_sum <= {D_W_ACC{1'b0}};
        
        in_valid_r <= 1'b0;
        in_data_r  <= {D_W_ACC{1'b0}};
        out_valid  <= 1'b0;
        out_data   <= {D_W_ACC{1'b0}};
    end else begin
        out_a   <= in_a;
        out_b   <= in_b;
        
        in_valid_r <= in_valid;
        in_data_r  <= in_data;
        
        if (init) begin
          out_sum   <= mult_op;
          out_valid <= 1'b1;
          out_data  <= out_sum;
        end else begin
          out_sum   <= out_sum + mult_op;
          out_valid <= in_valid_r;
          out_data  <= in_data_r;
        end
    end
end
 
endmodule
