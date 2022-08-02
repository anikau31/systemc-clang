module top;
  // self contained

  logic [511:0]AXI_STR_RXD_0_tdata;
  logic  AXI_STR_RXD_0_tlast;
  logic  AXI_STR_RXD_0_tready;
  logic  AXI_STR_RXD_0_tvalid;
  logic  [511:0]AXI_STR_TXD_0_tdata;
  logic  AXI_STR_TXD_0_tlast;
  logic  AXI_STR_TXD_0_tready;
  logic  AXI_STR_TXD_0_tvalid;
  logic  clk100mhz_0;
  logic  clk25mhz_0;
  logic  clk50mhz_0;
  logic  locked_0;
  logic  [0:0]peripheral_aresetn_0;
  logic  [0:0]peripheral_reset_0;




   wire S_BSCAN_drck;
   wire S_BSCAN_shift;
   wire S_BSCAN_tdi;
   wire S_BSCAN_update;
   wire S_BSCAN_sel;
   wire S_BSCAN_tdo;
   wire S_BSCAN_tms;
   wire S_BSCAN_tck;
   wire S_BSCAN_runtest;
   wire S_BSCAN_reset;
   wire S_BSCAN_capture;
   wire S_BSCAN_bscanid_en;

  shell_wrapper sw(
    .AXI_STR_RXD_0_tdata(AXI_STR_RXD_0_tdata),
    .AXI_STR_RXD_0_tlast(AXI_STR_RXD_0_tlast),
    .AXI_STR_RXD_0_tready(AXI_STR_RXD_0_tready),
    .AXI_STR_RXD_0_tvalid(AXI_STR_RXD_0_tvalid),
    .AXI_STR_TXD_0_tdata(AXI_STR_TXD_0_tdata),
    .AXI_STR_TXD_0_tlast(AXI_STR_TXD_0_tlast),
    .AXI_STR_TXD_0_tready(AXI_STR_TXD_0_tready),
    .AXI_STR_TXD_0_tvalid(AXI_STR_TXD_0_tvalid),
    .clk100mhz_0(clk100mhz_0),
    .clk25mhz_0(clk25mhz_0),
    .clk50mhz_0(clk50mhz_0),
    .locked_0(),
    .peripheral_aresetn_0(peripheral_aresetn_0),
    .peripheral_reset_0(peripheral_reset_0)
  );

  reconf_part_0 inst_rp(
    .*
  );

endmodule

`ifdef SYNTHESIS
module reconf_part_0(
  output [511:0]AXI_STR_RXD_0_tdata,
  output AXI_STR_RXD_0_tlast,
  input  AXI_STR_RXD_0_tready,
  output AXI_STR_RXD_0_tvalid,
  input  [511:0]AXI_STR_TXD_0_tdata,
  input  AXI_STR_TXD_0_tlast,
  output AXI_STR_TXD_0_tready,
  input  AXI_STR_TXD_0_tvalid,
  input  clk100mhz_0,
  input  clk25mhz_0,
  input  clk50mhz_0,
  input  locked_0,
  input  [0:0]peripheral_aresetn_0,
  input  [0:0]peripheral_reset_0,


   input  S_BSCAN_drck,
   input  S_BSCAN_shift,
   input  S_BSCAN_tdi,
   input  S_BSCAN_update,
   input  S_BSCAN_sel,
   output S_BSCAN_tdo,
   input  S_BSCAN_tms,
   input  S_BSCAN_tck,
   input  S_BSCAN_runtest,
   input  S_BSCAN_reset,
   input  S_BSCAN_capture,
   input  S_BSCAN_bscanid_en
);
endmodule
`endif
