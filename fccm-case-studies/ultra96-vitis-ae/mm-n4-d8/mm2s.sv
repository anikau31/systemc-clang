`timescale 1ps / 1ps

module mm2s
#
(
  parameter M   = 8,
  parameter N1  = 4,
  parameter N2  = 4,
  parameter D_W_ACC = 8
)
(
  input  wire clk,
  input  wire fclk,
  input  wire rst,
  output wire [31:0] m_axis_mm2s_tdata,
  output wire [3:0] m_axis_mm2s_tkeep,
  output reg m_axis_mm2s_tlast,
  input  wire m_axis_mm2s_tready,
  output wire m_axis_mm2s_tvalid,

  input  wire [N1-1:0] valid_D,
  input  wire [D_W_ACC-1:0] data_D [0:N1-1]
);

reg done_multiply, done_multiply_fclk, done_multiply_r, done_multiply_r1;
reg [$clog2(M*M):0] read_addr; // +1 to detect rollver

wire    [$clog2((M*M)/N1)-1:0]   wr_addr_D_bram            [N1-1:0];
wire    [$clog2((M*M)/N1)-1:0]   rd_addr_D_bram            [N1-1:0];
wire    [N1-1:0]                 wr_en_D_bram;
wire    [D_W_ACC-1:0]           wr_data_D_bram            [0:N1-1];
wire    [N1-1:0]                 activate_D;
reg     [N1-1:0]                 activate_D_reg;

wire    [D_W_ACC-1:0]           m2_PORTA_dout              [N1-1:0];

reg     [D_W_ACC-1:0]           reg_banked_data_D         [N1-1:0];
reg     [$clog2(M*M)-1:0]       reg_banked_read_addr_D    [N1-1:0];

reg     [N1+1:0]                 reg_banked_valid_D; // extra pipe stage for BRAM output register

always@(posedge clk)
begin
  if(rst) begin
    read_addr   <= 0;
  end else if (done_multiply && m_axis_mm2s_tready && read_addr < M*M) begin
    read_addr   <= read_addr + 1;
  end
end

genvar x;
for (x=0;x<N1;x=x+1)
begin: ram_D
  assign activate_D[x]     = ( x*((M*M)/N1) <= reg_banked_read_addr_D[x] && reg_banked_read_addr_D[x] < (x+1)*((M*M)/N1) ) ? 1 : 0;
  assign rd_addr_D_bram[x] = reg_banked_read_addr_D[x] -((x*M*M)/N1);

  mem 
  #(
    .WIDTH(D_W_ACC),
    .DEPTH(M*M/N1)
  )
  write_ram_D
  (
    .rst(rst),
    .clkA(fclk),
    .clkB(clk),
    .weA(1'b1),
    .enA(wr_en_D_bram[x]),
    .enB(activate_D[x] & reg_banked_valid_D[x] & m_axis_mm2s_tready),
    .addrA(wr_addr_D_bram[x]),
    .addrB(rd_addr_D_bram[x]),
    .dinA(wr_data_D_bram[x]),
    .doutB(m2_PORTA_dout[x])
  );

  if (x==0)
  begin
    always@(posedge clk)
    begin
      if(m_axis_mm2s_tready ) begin
        reg_banked_data_D[x]       <= m2_PORTA_dout[x];
        reg_banked_read_addr_D[x]  <= read_addr;
        reg_banked_valid_D[x]      <= done_multiply; 
      end
    end
  end
  else
  begin
    always@(posedge clk)
    begin
      if(m_axis_mm2s_tready ) begin
        reg_banked_data_D[x]       <= ( activate_D_reg[x] ==1 ) ? m2_PORTA_dout[x] : reg_banked_data_D[x-1];
        reg_banked_read_addr_D[x]  <= reg_banked_read_addr_D[x-1];
        reg_banked_valid_D[x]      <= reg_banked_valid_D[x-1]; 
      end
    end
  end
  always@(posedge clk)
  begin
    if(m_axis_mm2s_tready ) begin
      activate_D_reg[x]  <= activate_D[x];
    end
  end
end

always @(posedge clk) begin
  if(m_axis_mm2s_tready ) begin
    reg_banked_valid_D[N1]      <= reg_banked_valid_D[N1-1]; 
    reg_banked_valid_D[N1+1]      <= reg_banked_valid_D[N1]; 
  end
end

mem_write
#
(
  .D_W     (D_W_ACC),
  .N       (N1),
  .M       (M)
)
mem_write_D
(
  .clk            (fclk                  ),//
  .rst            (rst | done_multiply   ),//
  .in_valid       (valid_D               ),//
  .in_data        (data_D                ),//
  .wr_addr_bram   (wr_addr_D_bram        ),//
  .wr_data_bram   (wr_data_D_bram        ),//
  .wr_en_bram     (wr_en_D_bram          ) //
);

always@(posedge fclk) begin
  if(rst) begin
    done_multiply_fclk       <= 0;
  end else begin
    if (wr_addr_D_bram[N1-1] == (M*M)/N1-1) begin
      done_multiply_fclk   <= 1;
    end
  end
end

always@(posedge clk)
begin
  if(rst) begin
    done_multiply         <= 0;
    done_multiply_r       <= 0;
    done_multiply_r1	    <= 0;
  end else begin
    done_multiply_r	    <= done_multiply;
    done_multiply_r1    <= done_multiply_r;
    if (done_multiply_fclk == 1'b1) begin
      done_multiply   <= 1;
    end
  end
end

reg last_beat;
reg done = 0;

always@(posedge clk) begin
  if(m_axis_mm2s_tready ) begin
    last_beat           <= (reg_banked_read_addr_D[N1-1] == (M*M)-1) ? 1 : 0;
    m_axis_mm2s_tlast   <= last_beat;
    if (m_axis_mm2s_tlast == 1) begin
      done    <= 1;
    end
  end
end

assign m_axis_mm2s_tdata   = reg_banked_data_D[N1-1];
assign m_axis_mm2s_tkeep   = 4'b1111;
assign m_axis_mm2s_tvalid  = reg_banked_valid_D[N1+1] && ~done; 

endmodule
