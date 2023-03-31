`timescale 1ps / 1ps

module s2mm
#
(
  parameter M   = 8,
  parameter N1  = 4,
  parameter N2  = 4,
  parameter D_W = 8
)
(
  input  wire clk,
  input  wire fclk,
  input  wire rst,
  input  wire [31:0] s_axis_s2mm_tdata,
  input  wire [3:0]  s_axis_s2mm_tkeep,
  input  wire s_axis_s2mm_tlast,
  output wire s_axis_s2mm_tready,
  input  wire s_axis_s2mm_tvalid,

  input  wire [$clog2((M*M)/N1)-1:0] rd_addr_A,
  input  wire [$clog2((M*M)/N2)-1:0] rd_addr_B,

  output wire [D_W-1:0] A_bram [N1-1:0],
  output wire [D_W-1:0] B_bram [N2-1:0]
);


reg [$clog2(2*M*M)-1:0] write_addr;

always@(posedge clk)
begin
  if(rst) begin
    write_addr  <= 0;
  end else if (s_axis_s2mm_tready && s_axis_s2mm_tvalid) begin
    write_addr  <= write_addr + 1;
  end
end

reg  [$clog2(2*M*M)-1:0]     reg_banked_write_addr_A   	[N1-1:0];
reg                        	 reg_banked_valid_A        	[N1-1:0];
reg                        	 reg_banked_ready_A        	[N1-1:0];
reg  [D_W-1:0]               reg_banked_data_A        	[N1-1:0];

wire [$clog2((M*M)/N1)-1:0]   rd_addr_A_bram 		[N1-1:0];
wire [$clog2((M*M)/N1)-1:0]   wr_addr_A_bram 		[N1-1:0];
wire [N1-1:0]                 rd_en_A_bram;
wire [N1-1:0]                 activate_A;

wire [31:0] A_bram_data [N1-1:0];
wire [31:0] B_bram_data [N1-1:0];

genvar x;
for(x = 0; x < N1 ; x = x +1)
begin: ram_A
  assign  activate_A[x]     = ( x*((M*M)/N1) <= reg_banked_write_addr_A[x] && reg_banked_write_addr_A[x] < (x+1)*((M*M)/N1) ) ? 1 : 0;
  assign  wr_addr_A_bram[x] = reg_banked_write_addr_A[x] - ((x*M*M)/N1);
  mem 
  #(
    .WIDTH(32),
    .DEPTH(M*M/N1)
  )
  read_ram_A
  (
    .rst(rst),
    .clkA(clk),
    .clkB(fclk),
    .weA(reg_banked_valid_A[x]),
    .enA(activate_A[x]),
    .enB(rd_en_A_bram[x]),
    .addrA(wr_addr_A_bram[x]),
    .addrB(rd_addr_A_bram[x]),
    .dinA({{32-D_W{1'b0}},reg_banked_data_A[x]}),
    .doutB(A_bram_data[x])
  );

  assign A_bram[x] = A_bram_data[x][7:0];

  if (x==0)
  begin
    always@(posedge clk)
    begin
      reg_banked_write_addr_A[x]<= write_addr;
      reg_banked_valid_A[x]     <= s_axis_s2mm_tvalid;
      reg_banked_ready_A[x]     <= s_axis_s2mm_tready;
      reg_banked_data_A[x]      <= s_axis_s2mm_tdata;
    end   
  end
  else
  begin
    always@(posedge clk)
    begin
      reg_banked_write_addr_A[x]<= reg_banked_write_addr_A[x-1];
      reg_banked_valid_A[x]     <= reg_banked_valid_A[x-1];
      reg_banked_ready_A[x]     <= reg_banked_ready_A[x-1];
      reg_banked_data_A[x]      <= reg_banked_data_A[x-1];
    end   
  end
end

reg  [$clog2(2*M*M)-1:0]     reg_banked_write_addr_B   	[N2-1:0];
reg                          reg_banked_valid_B       	[N2-1:0];
reg                          reg_banked_ready_B       	[N2-1:0];
reg  [D_W-1:0]               reg_banked_data_B         	[N2-1:0];

wire    [$clog2((M*M)/N2)-1:0]   rd_addr_B_bram 		[N2-1:0];
wire    [$clog2((M*M)/N2)-1:0]   wr_addr_B_bram 		[N2-1:0];
wire    [N2-1:0]                 rd_en_B_bram;
wire    [N2-1:0]                 activate_B;

for(x = 0; x < N2 ; x = x +1)
begin: ram_B
  assign  activate_B[x]     = ( x*((M*M)/N2) + (M*M) <= reg_banked_write_addr_B[x] && reg_banked_write_addr_B[x] < (x+1)*((M*M)/N2)  + (M*M ) ) ? 1 : 0;
  assign  wr_addr_B_bram[x] = reg_banked_write_addr_B[x] - ((x*M*M)/N2) - M*M;

  mem 
  #(
    .WIDTH(32),
    .DEPTH(M*M/N2)
  )
  read_ram_B
  (
    .rst(rst),
    .clkA(clk),
    .clkB(fclk),
    .weA(reg_banked_valid_B[x]),
    .enA(activate_B[x]),
    .enB(rd_en_B_bram[x]),
    .addrA(wr_addr_B_bram[x]),
    .addrB(rd_addr_B_bram[x]),
    .dinA({{32-D_W{1'b0}},reg_banked_data_B[x]}),
    .doutB(B_bram_data[x])
  );

  assign B_bram[x] = B_bram_data[x][7:0];

  if (x==0)
  begin
    always@(posedge clk)
    begin
      reg_banked_write_addr_B[x]<= reg_banked_write_addr_A[N1-1];
      reg_banked_valid_B[x]     <= reg_banked_valid_A[N1-1];
      reg_banked_ready_B[x]     <= reg_banked_ready_A[N1-1];
      reg_banked_data_B[x]      <= reg_banked_data_A[N1-1];
    end   
  end
  else
  begin
    always@(posedge clk)
    begin
      reg_banked_write_addr_B[x]<= reg_banked_write_addr_B[x-1];
      reg_banked_valid_B[x]     <= reg_banked_valid_B[x-1];
      reg_banked_ready_B[x]     <= reg_banked_ready_B[x-1];
      reg_banked_data_B[x]      <= reg_banked_data_B[x-1];
    end   
  end
end

reg start_multiply=1'b0;

always@(posedge fclk)
begin
  if(rst) begin
    start_multiply  <= 0;
  end if (reg_banked_write_addr_B[N2-1] == 2*M*M - 1) begin
    start_multiply  <= 1;
  end
end 

assign s_axis_s2mm_tready = ~start_multiply;

mem_read_A
#
(
  .D_W    (D_W),
  .N      (N1),
  .M      (M)
)
mem_read_A_inst
(
  .clk            (fclk                       ),//
  .rd_addr        (rd_addr_A                 ),
  .rd_en          (~(rst) & start_multiply   ),//
  .rd_addr_bram   (rd_addr_A_bram),       //
  .rd_en_bram     (rd_en_A_bram)          //
);

mem_read_B
#
(
  .D_W    (D_W),
  .N      (N2),
  .M      (M)
)
mem_read_B_inst
(
  .clk            (fclk                       ),//
  .rd_addr        (rd_addr_B                 ),
  .rd_en          (~(rst) & start_multiply   ),//
  .rd_addr_bram   (rd_addr_B_bram),       //
  .rd_en_bram     (rd_en_B_bram)          //
);

endmodule

