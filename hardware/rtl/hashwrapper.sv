
module reconf_part_0(
  // we will need some way to direct
  // the streams
  input [31:0]  M_AXIS_MM2S_0_tdata,
  input [3:0]   M_AXIS_MM2S_0_tdest,
  input [7:0]   M_AXIS_MM2S_0_tid,
  input [3:0]   M_AXIS_MM2S_0_tkeep,
  input         M_AXIS_MM2S_0_tlast,
  output        M_AXIS_MM2S_0_tready,
  input [15:0]  M_AXIS_MM2S_0_tuser,
  input         M_AXIS_MM2S_0_tvalid,

  output [31:0] S_AXIS_S2MM_0_tdata,
  output [3:0]  S_AXIS_S2MM_0_tdest,
  output [15:0] S_AXIS_S2MM_0_tid,
  output [3:0]  S_AXIS_S2MM_0_tkeep,
  output        S_AXIS_S2MM_0_tlast,
  input         S_AXIS_S2MM_0_tready,
  output [3:0]  S_AXIS_S2MM_0_tuser,
  output        S_AXIS_S2MM_0_tvalid,

  input  logic clk100mhz_0,
  input  logic clk25mhz_0,
  input  logic clk50mhz_0,
  input  logic locked_0,
  input  logic [0:0]interconnect_aresetn_0,
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

  // Width converter
  logic          M00_AXIS_TVALID;
  logic          M00_AXIS_TREADY;
  logic [31 : 0] M00_AXIS_TDATA;
  logic [3 : 0]  M00_AXIS_TKEEP;
  logic          M00_AXIS_TLAST;  
  logic [0 : 0]  M00_AXIS_TDEST;
  
  logic          M01_AXIS_TVALID;
  logic          M01_AXIS_TREADY;
  logic [31 : 0] M01_AXIS_TDATA;
  logic [3 : 0]  M01_AXIS_TKEEP;
  logic          M01_AXIS_TLAST;
  logic [0 : 0]  M01_AXIS_TDEST;

  logic           hdata_tvalid;
  logic           hdata_tready;
  logic [135 : 0] hdata_tdata;
  logic [16 : 0]  hdata_tkeep;

  logic           htaptoshash_tvalid;
  logic           htaptoshash_tready;
  logic [255 : 0] htaptoshash_tdata;

  logic           htapfromshash_tvalid;
  logic           htapfromshash_tready;
  logic [255 : 0] htapfromshash_tdata;

  // logic           m_axis_tvalid
  // logic           m_axis_tready
  // logic [31 : 0]  m_axis_tdata

  // input stream director
  //----------- Begin Cut here for INSTANTIATION Template ---// INST_TAG
  axis_interconnect_1_2 in_traffic_switch (
    .ACLK(clk50mhz_0),                          // input wire ACLK
    .ARESETN(interconnect_aresetn_0),                    // input wire ARESETN
    .S00_AXIS_ACLK(clk50mhz_0),        // input wire S00_AXIS_ACLK
    .S00_AXIS_ARESETN(peripheral_aresetn_0),  // input wire S00_AXIS_ARESETN
    .S00_AXIS_TVALID(M_AXIS_MM2S_0_tvalid),    // input wire S00_AXIS_TVALID
    .S00_AXIS_TREADY(M_AXIS_MM2S_0_tready),    // output wire S00_AXIS_TREADY
    .S00_AXIS_TDATA(M_AXIS_MM2S_0_tdata),      // input wire [31 : 0] S00_AXIS_TDATA
    .S00_AXIS_TKEEP(M_AXIS_MM2S_0_tkeep),      // input wire [3 : 0] S00_AXIS_TKEEP
    .S00_AXIS_TLAST(M_AXIS_MM2S_0_tlast),      // input wire S00_AXIS_TLAST
    .S00_AXIS_TDEST(M_AXIS_MM2S_0_tdest),      // input wire [0 : 0] S00_AXIS_TDEST

    .M00_AXIS_ACLK(clk50mhz_0),        // input wire M00_AXIS_ACLK
    .M00_AXIS_ARESETN(peripheral_aresetn_0),  // input wire M00_AXIS_ARESETN
    .M00_AXIS_TVALID(M00_AXIS_TVALID),     // output wire M00_AXIS_TVALID
    .M00_AXIS_TREADY(M00_AXIS_TREADY),     // input wire M00_AXIS_TREADY
    .M00_AXIS_TDATA (M00_AXIS_TDATA),      // output wire [31 : 0] M00_AXIS_TDATA
    .M00_AXIS_TKEEP (M00_AXIS_TKEEP),      // output wire [3 : 0] M00_AXIS_TKEEP
    .M00_AXIS_TLAST (M00_AXIS_TLAST),      // output wire M00_AXIS_TLAST
    .M00_AXIS_TDEST (M00_AXIS_TDEST),      // output wire [0 : 0] M00_AXIS_TDEST

    .M01_AXIS_ACLK(clk50mhz_0),        // input wire M01_AXIS_ACLK
    .M01_AXIS_ARESETN(peripheral_aresetn_0),  // input wire M01_AXIS_ARESETN
    .M01_AXIS_TVALID(M01_AXIS_TVALID),     // output wire M01_AXIS_TVALID
    .M01_AXIS_TREADY(M01_AXIS_TREADY),     // input wire M01_AXIS_TREADY
    .M01_AXIS_TDATA (M01_AXIS_TDATA),      // output wire [31 : 0] M01_AXIS_TDATA
    .M01_AXIS_TKEEP (M01_AXIS_TKEEP),      // output wire [3 : 0] M01_AXIS_TKEEP
    .M01_AXIS_TLAST (M01_AXIS_TLAST),      // output wire M01_AXIS_TLAST
    .M01_AXIS_TDEST (M01_AXIS_TDEST),      // output wire [0 : 0] M01_AXIS_TDEST

    .S00_DECODE_ERR(S00_DECODE_ERR)      // output wire S00_DECODE_ERR
  );


  mymodule_sc_module_0 dut ();
  assign dut.clk = clk50mhz_0;
  assign dut.reset = peripheral_aresetn_0;
  // assign dut.hdata_data  = hdata_tvalid ? hdata_tdata : 32'h0;
  assign dut.hdata_data  = hdata_tdata;
  assign dut.hdata_valid = hdata_tvalid;
  assign hdata_tready    = dut.hdata_ready;

  `ifndef SYNTHESIS
  always @(posedge clk50mhz_0) begin
    if(hdata_tready && hdata_tvalid) begin
      if(hdata_tkeep != ~17'b0) begin
        $error("hdata_tkeep incorrect for a data flit, got %b", hdata_tkeep);
      end
    end
  end
  `endif

  // assign dut.htaptoshash_data  = htaptoshash_tvalid ? htaptoshash_tdata : 32'h0;
  assign dut.htaptoshash_data  = htaptoshash_tdata;
  assign dut.htaptoshash_valid = htaptoshash_tvalid;
  assign htaptoshash_tready    = dut.htaptoshash_ready;

  assign htapfromshash_tdata      = dut.htapfromshash_data ;
  assign htapfromshash_tvalid     = dut.htapfromshash_valid;
  assign dut.htapfromshash_ready = htapfromshash_tready;


  // INST_TAG_END ------ End INSTANTIATION Template ---------
  // M00 32 -> 136
  axis_dwidth_32_136 in_traffic_32_136 (
  .aclk(clk50mhz_0),                    // input wire aclk
  .aresetn(peripheral_aresetn_0),              // input wire aresetn
  .s_axis_tvalid(M00_AXIS_TVALID),  // input wire s_axis_tvalid
  .s_axis_tready(M00_AXIS_TREADY),  // output wire s_axis_tready
  .s_axis_tdata (M00_AXIS_TDATA),    // input wire [31 : 0] s_axis_tdata
  .s_axis_tkeep (M00_AXIS_TKEEP),    // input wire [31 : 0] s_axis_tdata
  .s_axis_tlast (M00_AXIS_TLAST),    // input wire [31 : 0] s_axis_tdata

  .m_axis_tvalid(hdata_tvalid),  // output wire m_axis_tvalid
  .m_axis_tready(hdata_tready),  // input  wire m_axis_tready
  .m_axis_tdata (hdata_tdata),   // output wire [135 : 0] m_axis_tdata
  .m_axis_tkeep (hdata_tkeep)    // output wire [16 : 0] m_axis_tkeep
  );
  // M01 32 -> 256
  axis_dwidth_32_256 in_traffic_32_256 (
  .aclk(clk50mhz_0),                    // input wire aclk
  .aresetn(peripheral_aresetn_0),              // input wire aresetn
  .s_axis_tvalid(M01_AXIS_TVALID),  // input wire s_axis_tvalid
  .s_axis_tready(M01_AXIS_TREADY),  // output wire s_axis_tready
  .s_axis_tdata (M01_AXIS_TDATA),    // input wire [31 : 0] s_axis_tdata

  .m_axis_tvalid(htaptoshash_tvalid),  // output wire m_axis_tvalid
  .m_axis_tready(htaptoshash_tready),  // input wire m_axis_tready
  .m_axis_tdata (htaptoshash_tdata)    // output wire [255 : 0] m_axis_tdata
  );
  //
  // S  256 -> 32

  assign S_AXIS_S2MM_0_tdest = 0;
  assign S_AXIS_S2MM_0_tkeep = 4'hF;
  assign S_AXIS_S2MM_0_tlast = 0;
  assign S_AXIS_S2MM_0_tid   = 16'h0;
  assign S_AXIS_S2MM_0_tuser = 0;

  axis_dwidth_256_32 out_traffic_256_32 (
  .aclk(clk50mhz_0),                    // input wire aclk
  .aresetn(peripheral_aresetn_0),              // input wire aresetn
  .s_axis_tvalid(htapfromshash_tvalid),  // input wire s_axis_tvalid
  .s_axis_tready(htapfromshash_tready),  // output wire s_axis_tready
  .s_axis_tdata (htapfromshash_tdata),   // input wire [255 : 0] s_axis_tdata
   
  .m_axis_tvalid(S_AXIS_S2MM_0_tvalid),  // output wire m_axis_tvalid
  .m_axis_tready(S_AXIS_S2MM_0_tready),  // input wire m_axis_tready
  .m_axis_tdata (S_AXIS_S2MM_0_tdata)     // output wire [31 : 0] m_axis_tdata
  );
  // assign {dut.c_driver_fp_data_sign, dut.c_driver_fp_data_expo, dut.c_driver_fp_data_frac} = AXI_STR_TXD_0_tdata[63:0];

  // assign dut.c_driver_fp_valid = AXI_STR_TXD_0_tvalid;
  // assign AXI_STR_TXD_0_tready = dut.c_driver_fp_ready;

  // assign AXI_STR_RXD_0_tdata[63:0] = dut.c_dut_enc_data_tdata;
  // assign AXI_STR_RXD_0_tdata[511:64] = 0;
  // assign AXI_STR_RXD_0_tlast = dut.c_dut_enc_data_tlast;
  // assign AXI_STR_RXD_0_tvalid = dut.c_dut_enc_valid;
  // assign dut.c_dut_enc_ready = AXI_STR_RXD_0_tready;

  /**
  logic [0:0] clk;
  logic [0:0] reset;
  logic [135:0] hdata_data;
  logic [0:0] hdata_valid;
  logic [0:0] hdata_ready;
  logic [255:0] htaptoshash_data;
  logic [0:0] htaptoshash_valid;
  logic [0:0] htaptoshash_ready;
  logic [255:0] htapfromshash_data;
  logic [0:0] htapfromshash_valid;
  logic [0:0] htapfromshash_ready;
  */
endmodule
