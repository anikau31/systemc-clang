
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
  logic        in_last;
  logic [63:0] out_data;
  logic        out_valid;
  logic        out_ready;
  logic        out_last;

  axis_dwidth_32_32 in_traffic_32_64 (
    .aclk(clk25mhz_0),                    // input wire aclk
    .aresetn(peripheral_aresetn_0),              // input wire aresetn
    .s_axis_tvalid(M_AXIS_MM2S_0_tvalid),  // input wire s_axis_tvalid
    .s_axis_tready(M_AXIS_MM2S_0_tready),  // output wire s_axis_tready
    .s_axis_tdata(M_AXIS_MM2S_0_tdata),    // input wire [31 : 0] s_axis_tdata
    .s_axis_tlast(M_AXIS_MM2S_0_tlast),    // input wire [31 : 0] s_axis_tdata

    .m_axis_tvalid(in_valid),  // output wire m_axis_tvalid
    .m_axis_tready(in_ready),  // input wire m_axis_tready
    .m_axis_tdata(in_data),    // output wire [63 : 0] m_axis_tdata
    .m_axis_tlast(in_last)    // 
  );
  axis_dwidth_32_32 out_traffic_64_32 (
    .aclk(clk25mhz_0),                    // input wire aclk
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
  //
  // logic [0:0] c_driver_enc_scclang_global_7_data_tlast;

  mymodule_sc_module_0 dut ();
  assign dut.clk_scclang_global_0= clk25mhz_0;
  assign dut.reset_scclang_global_5 = peripheral_aresetn_0;

  assign dut.c_driver_enc_scclang_global_7_data_tdata = in_data;
  assign dut.c_driver_enc_scclang_global_7_valid = in_valid;
  assign in_ready = dut.c_driver_enc_scclang_global_7_ready;

  assign out_data = {dut.c_dut_fp_scclang_global_6_data_sign, dut.c_dut_fp_scclang_global_6_data_expo, dut.c_dut_fp_scclang_global_6_data_frac};

  assign out_last = 1;
  assign out_valid = dut.c_dut_fp_scclang_global_6_valid;
  assign dut.c_dut_fp_scclang_global_6_ready = out_ready;

  assign S_AXIS_S2MM_0_tdest = 0;
  assign S_AXIS_S2MM_0_tkeep = 4'hf;

endmodule
