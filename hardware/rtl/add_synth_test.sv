
module reconf_part_0(
  output logic [511:0]AXI_STR_RXD_0_tdata,
  output logic AXI_STR_RXD_0_tlast,
  input  logic AXI_STR_RXD_0_tready,
  output logic AXI_STR_RXD_0_tvalid,
  input  logic [511:0]AXI_STR_TXD_0_tdata,
  input  logic AXI_STR_TXD_0_tlast,
  output logic AXI_STR_TXD_0_tready,
  input  logic AXI_STR_TXD_0_tvalid,
  input  logic clk100mhz_0,
  input  logic clk25mhz_0,
  input  logic clk50mhz_0,
  input  logic locked_0,
  input  logic [0:0]peripheral_aresetn_0,
  input  logic [0:0]peripheral_reset_0,


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
  assign AXI_STR_TXD_0_tready = AXI_STR_RXD_0_tready;

  integer i;

  always @(posedge clk50mhz_0) begin
    if(peripheral_reset_0) begin
      AXI_STR_RXD_0_tvalid <= 0;
      AXI_STR_RXD_0_tlast  <= 0;
      AXI_STR_RXD_0_tdata  <= 0;
    end else begin
      AXI_STR_RXD_0_tvalid <= AXI_STR_TXD_0_tvalid;
      AXI_STR_RXD_0_tlast  <= AXI_STR_TXD_0_tlast;
      AXI_STR_RXD_0_tdata  <= AXI_STR_TXD_0_tdata + 32'hdeadbeef;
      for(i = 0; i < 4; i= i + 1) begin
        if(i == 2) continue;
        AXI_STR_RXD_0_tdata <= AXI_STR_TXD_0_tdata + i * 2;
      end
    end
  end
endmodule
