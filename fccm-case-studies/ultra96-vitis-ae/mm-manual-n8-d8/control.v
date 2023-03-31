`timescale 1ps / 1ps

module control
  #(
    parameter   N1  = 4,
    parameter   N2  = 4,
    parameter   M   = 8,
    parameter   D_W = 8,        //operand data width
    parameter   D_W_ACC = 16

  )
  (
    input   wire                                        clk,
    input   wire                                        rst,
    input   wire                                        enable_row_count,
    output  wire    [$clog2(M)-1:0]                     pixel_cntr_A,
    output  wire    [($clog2(M/N1)?$clog2(M/N1):1)-1:0] slice_cntr_A,
    output  wire    [($clog2(M/N2)?$clog2(M/N2):1)-1:0] pixel_cntr_B,
    output  wire    [$clog2(M)-1:0]                     slice_cntr_B,
    output  wire    [$clog2((M*M)/N1)-1:0]              rd_addr_A,
    output  wire    [$clog2((M*M)/N2)-1:0]              rd_addr_B
  );

// insert your RTL for rd_addr_A, rd_addr_B here
assign  rd_addr_A = (slice_cntr_A*(M)+pixel_cntr_A);
assign  rd_addr_B = (pixel_cntr_B*(M)+slice_cntr_B);



counter#
(
  .WIDTH  (M),
  .HEIGHT (M/N2)
)
counter_B
(

  .clk                  (clk),
  .rst                  (rst),
  .enable_row_count     (1'b1),
  .pixel_cntr           (slice_cntr_B),
  .slice_cntr           (pixel_cntr_B)
);


counter#
(
  .WIDTH  (M),
  .HEIGHT (M/N1)
)
counter_A
(

  .clk                  (clk),
  .rst                  (rst),
  .enable_row_count     (enable_row_count),
  .pixel_cntr           (pixel_cntr_A),
  .slice_cntr           (slice_cntr_A)
);

endmodule
