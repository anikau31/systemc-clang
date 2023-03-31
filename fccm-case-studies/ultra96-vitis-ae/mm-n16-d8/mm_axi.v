
module mm_axi 
#(
	parameter M=16,
	parameter N1=16,
	parameter N2=16
) (
        clk,
        // fclk,
        rst_n,
	// incoming stream
        x_TDATA,
        x_TVALID,
        x_TREADY,
        x_TLAST,
	// outgoing stream
        y_TDATA,
        y_TVALID,
        y_TREADY,
        y_TLAST
);

input   clk;
// input   fclk;
input   rst_n;
input  [31:0] x_TDATA;
input   x_TVALID;
output   x_TREADY;
input  [0:0] x_TLAST;
output  [31:0] y_TDATA;
output   y_TVALID;
input   y_TREADY;
output  [0:0] y_TLAST;

mm #(.M(M), .N1(N1), .N2(N2), .D_W(8), .D_W_ACC(16))
mm_inst (
	.mm_clk(clk),
	.mm_fclk(clk),
	.mm_rst_n(rst_n),

	.s_axis_s2mm_tdata(x_TDATA),
	.s_axis_s2mm_tkeep(),
	.s_axis_s2mm_tlast(x_TLAST),
	.s_axis_s2mm_tready(x_TREADY),
	.s_axis_s2mm_tvalid(x_TVALID),

	.m_axis_mm2s_tdata(y_TDATA),
	.m_axis_mm2s_tkeep(),
	.m_axis_mm2s_tlast(y_TLAST),
	.m_axis_mm2s_tready(y_TREADY),
	.m_axis_mm2s_tvalid(y_TVALID)
);

endmodule //mm_axi

