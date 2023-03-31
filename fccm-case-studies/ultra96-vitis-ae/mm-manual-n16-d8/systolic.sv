`default_nettype none

module systolic
#
(
    parameter   D_W  = 8, //operand data width
    parameter   D_W_ACC = 16, //accumulator data width
    parameter   N1   = 4,
    parameter   N2   = 4,
    parameter   M    = 8
)
(
    input   wire                                        clk,
    input   wire                                        rst,
    input   wire                                        enable_row_count_A,
    output  wire    [$clog2(M)-1:0]                     pixel_cntr_A,
    output  wire    [($clog2(M/N1)?$clog2(M/N1):1)-1:0] slice_cntr_A,
    output  wire    [($clog2(M/N2)?$clog2(M/N2):1)-1:0] pixel_cntr_B,
    output  wire    [$clog2(M)-1:0]                     slice_cntr_B,
    output  wire    [$clog2((M*M)/N1)-1:0]              rd_addr_A,
    output  wire    [$clog2((M*M)/N2)-1:0]              rd_addr_B,
    input   wire    [D_W-1:0]                           A [N1-1:0], //m0
    input   wire    [D_W-1:0]                           B [N2-1:0], //m1
    output  wire    [D_W_ACC-1:0]                       D [N1-1:0], //m2
    output  wire    [N1-1:0]                             valid_D
);


wire    [D_W-1:0]       a_wire   	[N1-1:0][N2:0];
wire    [D_W-1:0]       b_wire   	[N1:0][N2-1:0];
wire    [N2:0]          valid_wire   	[N1-1:0];
wire    [(D_W_ACC)-1:0] data_wire    	[N1-1:0][N2:0];

reg [2*N1-1:0] shift;
reg [$clog2((M*M)/(N1*N2))-1:0]   patch = 0;
integer r;

control #
(
  .N1       (N1),
  .N2       (N2),
  .M        (M),
  .D_W      (D_W),
  .D_W_ACC  (D_W_ACC)
)
control_inst
(

  .clk                  (clk),
  .rst                  (rst),
  .enable_row_count     (enable_row_count_A),

  .pixel_cntr_B         (pixel_cntr_B),
  .slice_cntr_B         (slice_cntr_B),

  .pixel_cntr_A         (pixel_cntr_A),
  .slice_cntr_A         (slice_cntr_A),

  .rd_addr_A            (rd_addr_A),
  .rd_addr_B            (rd_addr_B)
);

always@(posedge clk) begin
  if(rst) begin
    shift <= {2*N1{1'b0}};
  end else begin
    if (pixel_cntr_A==M-1 && patch < (M*M)/(N1*N2)) begin
      patch   <= patch + 1;
      shift[0] <= 1'b1;
    end else begin
      shift[0] <= 1'b0;
    end

    for(r = 1; r < 2*N1; r = r+1) begin
      shift[r] <= shift[r-1];
    end
  end
end

genvar i, j;
generate for (i=0; i<N1;i=i+1) begin

  //assign a_wire[i][0] = A[i];
  //assign b_wire[0][i] = B[i];
  assign valid_wire[i][0] = 1'b0;
  assign data_wire[i][0] = {D_W{1'b0}};

  for (j=0; j<N2;j=j+1) begin

    pe # (.D_W (D_W),.D_W_ACC (D_W_ACC))
    pe_inst (
      .clk        (clk),
      .rst        (rst),
      .init       (shift[i+j+1]),
      .in_a       (j==0?A[i]:a_wire[i][j]),
      .in_b       (i==0?B[j]:b_wire[i][j]),
      .out_a      (a_wire[i][j+1]),
      .out_b      (b_wire[i+1][j]),
      .in_valid   (valid_wire[i][j]),
      .in_data    (data_wire[i][j]),
      .out_valid  (valid_wire[i][j+1]),
      .out_data   (data_wire[i][j+1])
    );

  end

  assign  valid_D[i] = valid_wire[i][N2];
  assign  D[i]  = data_wire[i][N2];      

end endgenerate

endmodule
