`timescale 1ps / 1ps

module mm
    #
    (
        parameter   M       = 4,
        parameter   N1      = 4,
        parameter   N2      = 4,
        parameter   D_W     = 8,
        parameter   D_W_ACC = 16
    )
    (
    input  wire             mm_clk,
    input  wire             mm_fclk,
    input  wire             mm_rst_n,
    
    input  wire     [31:0]  s_axis_s2mm_tdata,
    input  wire     [3:0]   s_axis_s2mm_tkeep,
    input  wire             s_axis_s2mm_tlast,
    output wire             s_axis_s2mm_tready,
    input  wire             s_axis_s2mm_tvalid,

    output  wire    [31:0]  m_axis_mm2s_tdata,
    output  wire    [3:0]   m_axis_mm2s_tkeep,
    output  reg             m_axis_mm2s_tlast,
    input   wire            m_axis_mm2s_tready,
    output  wire            m_axis_mm2s_tvalid
    );
    
    wire    rst;
    assign  rst = ~mm_rst_n;
    wire    clk;
    assign  clk = ~mm_clk;
    wire    fclk;
    assign  fclk = ~mm_fclk;

    wire    [D_W-1:0]            A_bram         [N1-1:0];
    wire    [D_W-1:0]            B_bram         [N2-1:0];

    wire    [N1-1:0]              valid_D;
    wire    [D_W_ACC-1:0]        data_D 	[N1-1:0];

    wire  [$clog2(M)-1:0] pixel_cntr_A;
    wire  [($clog2(M/N1)?$clog2(M/N1):1)-1:0] slice_cntr_A;
    wire  [($clog2(M/N2)?$clog2(M/N2):1)-1:0] pixel_cntr_B;
    wire  [$clog2(M)-1:0] slice_cntr_B;
    wire  [$clog2(M*M/N1)-1:0] rd_addr_A;
    wire  [$clog2(M*M/N2)-1:0] rd_addr_B;

    reg [31:0]  patch = 1;
    reg         enable_row_count_A = 0;

    s2mm 
    #
    (
    	.M(M),
    	.N1(N1),
    	.N2(N2),
    	.D_W(D_W)
    )
    s2mm_inst
    (
    	.clk(clk),
	    .fclk(fclk),
    	.rst(rst),
    	.s_axis_s2mm_tdata(s_axis_s2mm_tdata),
    	.s_axis_s2mm_tkeep(s_axis_s2mm_tkeep),
    	.s_axis_s2mm_tlast(s_axis_s2mm_tlast),
    	.s_axis_s2mm_tready(s_axis_s2mm_tready),
    	.s_axis_s2mm_tvalid(s_axis_s2mm_tvalid),
      .rd_addr_A(rd_addr_A),
      .rd_addr_B(rd_addr_B),
    	.A_bram(A_bram),
    	.B_bram(B_bram)
    );

    mm2s
    #
    (
    	.M(M),
    	.N1(N1),
    	.N2(N2),
    	.D_W_ACC(D_W_ACC)
    )
    mm2s_inst
    (
    	.clk(clk),
	    .fclk(fclk),
    	.rst(rst),
    	.m_axis_mm2s_tdata(m_axis_mm2s_tdata),
    	.m_axis_mm2s_tkeep(m_axis_mm2s_tkeep),
    	.m_axis_mm2s_tlast(m_axis_mm2s_tlast),
    	.m_axis_mm2s_tready(m_axis_mm2s_tready),
    	.m_axis_mm2s_tvalid(m_axis_mm2s_tvalid),
    	.valid_D(valid_D),
      .data_D(data_D)
    );

    systolic
    #
    (
        .D_W     (D_W),
        .D_W_ACC (D_W_ACC),
        .N1       (N1),
        .N2       (N2),
        .M       (M)
    )
    systolic_inst
    (
        .clk                   (fclk),                       //
        .rst                   (rst | s_axis_s2mm_tready),   //
        .enable_row_count_A    (enable_row_count_A),         //
        .pixel_cntr_A          (pixel_cntr_A),               //
        .slice_cntr_A          (slice_cntr_A),               //
        .pixel_cntr_B          (pixel_cntr_B),               //
        .slice_cntr_B          (slice_cntr_B),               //
        .rd_addr_A             (rd_addr_A),                  //
        .rd_addr_B             (rd_addr_B),                  //
        .A                     (A_bram),                     //
        .B                     (B_bram),                     //
        .D                     (data_D),                     //
        .valid_D               (valid_D)                     //
    );
    
    always@(posedge fclk)
    begin
        if(rst) begin
            enable_row_count_A <= 1'b0;
            patch <= 1;
        end else begin
            if (enable_row_count_A == 1'b1) begin
                enable_row_count_A <= 1'b0;
            end else if (pixel_cntr_A == M-2 && patch == (M/N2)) begin
                patch <= 1;
                enable_row_count_A <= ~enable_row_count_A;
            end else if (pixel_cntr_A == M-2) begin
                patch <= patch + 1 ;
            end
        end
    end
    
endmodule
