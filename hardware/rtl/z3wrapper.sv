
module reconf_part_0(
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

  input  clk100mhz_0,
  input  clk25mhz_0,
  input  clk50mhz_0,
  input  locked_0,
  input  [0:0]interconnect_aresetn_0,
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
  logic [63:0] in_data;
  logic        in_valid;
  logic        in_ready;
  logic [63:0] out_data;
  logic        out_valid;
  logic        out_ready;
  logic        out_last;

  axis_dwidth_32_64 in_traffic_32_64 (
    .aclk(clk50mhz_0),                    // input wire aclk
    .aresetn(peripheral_aresetn_0),              // input wire aresetn
    .s_axis_tvalid(M_AXIS_MM2S_0_tvalid),  // input wire s_axis_tvalid
    .s_axis_tready(M_AXIS_MM2S_0_tready),  // output wire s_axis_tready
    .s_axis_tdata(M_AXIS_MM2S_0_tdata),    // input wire [31 : 0] s_axis_tdata

    .m_axis_tvalid(in_valid),  // output wire m_axis_tvalid
    .m_axis_tready(in_ready),  // input wire m_axis_tready
    .m_axis_tdata(in_data)    // output wire [63 : 0] m_axis_tdata
  );
  axis_dwidth_64_32 out_traffic_64_32 (
    .aclk(clk50mhz_0),                    // input wire aclk
    .aresetn(peripheral_aresetn_0),              // input wire aresetn
    .s_axis_tvalid(out_valid),  // input wire s_axis_tvalid
    .s_axis_tready(out_ready),  // output wire s_axis_tready
    .s_axis_tdata (out_data),    // input wire [63 : 0] s_axis_tdata
    .s_axis_tlast (out_last),    // input wire s_axis_tlast

    .m_axis_tvalid(S_AXIS_S2MM_0_tvalid),  // output wire m_axis_tvalid
    .m_axis_tready(S_AXIS_S2MM_0_tready),  // input wire m_axis_tready
    .m_axis_tdata(S_AXIS_S2MM_0_tdata),    // output wire [31 : 0] m_axis_tdata
    .m_axis_tlast(S_AXIS_S2MM_0_tlast)    // output wire m_axis_tlast
  );

  // declarations

  mymodule_sc_module_0 dut ();
  assign dut.clk = clk50mhz_0;
  assign dut.reset = peripheral_aresetn_0;
  assign {dut.c_driver_fp_data_sign, dut.c_driver_fp_data_expo, dut.c_driver_fp_data_frac} = in_data;

  assign dut.c_driver_fp_valid = in_valid;
  assign in_ready = dut.c_driver_fp_ready;

  assign out_data  = dut.c_dut_enc_data_tdata;
  assign out_last = dut.c_dut_enc_data_tlast;
  assign out_valid = dut.c_dut_enc_valid;
  assign dut.c_dut_enc_ready = out_ready;

  assign S_AXIS_S2MM_0_tdest = 0;
  assign S_AXIS_S2MM_0_tkeep = 4'hf;

endmodule
