module top(
  // Ports dedicated for PYNQ-Z1
  `ifdef pynqz1
  inout [14:0]DDR_addr,
  inout [2:0]DDR_ba,
  inout DDR_cas_n,
  inout DDR_ck_n,
  inout DDR_ck_p,
  inout DDR_cke,
  inout DDR_cs_n,
  inout [3:0]DDR_dm,
  inout [31:0]DDR_dq,
  inout [3:0]DDR_dqs_n,
  inout [3:0]DDR_dqs_p,
  inout DDR_odt,
  inout DDR_ras_n,
  inout DDR_reset_n,
  inout DDR_we_n,
  inout FIXED_IO_ddr_vrn,
  inout FIXED_IO_ddr_vrp,
  inout [53:0]FIXED_IO_mio,
  inout FIXED_IO_ps_clk,
  inout FIXED_IO_ps_porb,
  inout FIXED_IO_ps_srstb
  `endif
);
  // self contained
  logic [31:0] M_AXIS_MM2S_0_tdata;
  logic [3:0]  M_AXIS_MM2S_0_tdest;
  logic [7:0]  M_AXIS_MM2S_0_tid;
  logic [3:0]  M_AXIS_MM2S_0_tkeep;
  logic        M_AXIS_MM2S_0_tlast;
  logic        M_AXIS_MM2S_0_tready;
  logic [15:0] M_AXIS_MM2S_0_tuser;
  logic        M_AXIS_MM2S_0_tvalid;
  logic [31:0] S_AXIS_S2MM_0_tdata;
  logic [3:0]  S_AXIS_S2MM_0_tdest;
  logic [15:0] S_AXIS_S2MM_0_tid;
  logic [3:0]  S_AXIS_S2MM_0_tkeep;
  logic        S_AXIS_S2MM_0_tlast;
  logic        S_AXIS_S2MM_0_tready;
  logic [15:0] S_AXIS_S2MM_0_tuser;
  logic        S_AXIS_S2MM_0_tvalid;
  logic        clk100mhz_0;
  logic        clk25mhz_0;
  logic        clk50mhz_0;
  logic        locked_0;
  logic [0:0]  interconnect_aresetn_0;
  logic [0:0]  peripheral_aresetn_0;
  logic [0:0]  peripheral_reset_0;

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

  bd_wrapper sw(
    `ifdef pynqz1
    .DDR_addr         (DDR_addr         ),
    .DDR_ba           (DDR_ba           ),
    .DDR_cas_n        (DDR_cas_n        ),
    .DDR_ck_n         (DDR_ck_n         ),
    .DDR_ck_p         (DDR_ck_p         ),
    .DDR_cke          (DDR_cke          ),
    .DDR_cs_n         (DDR_cs_n         ),
    .DDR_dm           (DDR_dm           ),
    .DDR_dq           (DDR_dq           ),
    .DDR_dqs_n        (DDR_dqs_n        ),
    .DDR_dqs_p        (DDR_dqs_p        ),
    .DDR_odt          (DDR_odt          ),
    .DDR_ras_n        (DDR_ras_n        ),
    .DDR_reset_n      (DDR_reset_n      ),
    .DDR_we_n         (DDR_we_n         ),
    .FIXED_IO_ddr_vrn (FIXED_IO_ddr_vrn ),
    .FIXED_IO_ddr_vrp (FIXED_IO_ddr_vrp ),
    .FIXED_IO_mio     (FIXED_IO_mio     ),
    .FIXED_IO_ps_clk  (FIXED_IO_ps_clk  ),
    .FIXED_IO_ps_porb (FIXED_IO_ps_porb ),
    .FIXED_IO_ps_srstb(FIXED_IO_ps_srstb),
    `endif

    .M_AXIS_MM2S_0_tdata (M_AXIS_MM2S_0_tdata ),
    .M_AXIS_MM2S_0_tdest (M_AXIS_MM2S_0_tdest ),
    .M_AXIS_MM2S_0_tid   (M_AXIS_MM2S_0_tid   ),
    .M_AXIS_MM2S_0_tkeep (M_AXIS_MM2S_0_tkeep ),
    .M_AXIS_MM2S_0_tlast (M_AXIS_MM2S_0_tlast ),
    .M_AXIS_MM2S_0_tready(M_AXIS_MM2S_0_tready),
    .M_AXIS_MM2S_0_tuser (M_AXIS_MM2S_0_tuser ),
    .M_AXIS_MM2S_0_tvalid(M_AXIS_MM2S_0_tvalid),
    .S_AXIS_S2MM_0_tdata (S_AXIS_S2MM_0_tdata ),
    .S_AXIS_S2MM_0_tdest (S_AXIS_S2MM_0_tdest ),
    .S_AXIS_S2MM_0_tid   (S_AXIS_S2MM_0_tid   ),
    .S_AXIS_S2MM_0_tkeep (S_AXIS_S2MM_0_tkeep ),
    .S_AXIS_S2MM_0_tlast (S_AXIS_S2MM_0_tlast ),
    .S_AXIS_S2MM_0_tready(S_AXIS_S2MM_0_tready),
    .S_AXIS_S2MM_0_tuser (S_AXIS_S2MM_0_tuser ),
    .S_AXIS_S2MM_0_tvalid(S_AXIS_S2MM_0_tvalid),

    .clk100mhz_0(clk100mhz_0),
    .clk25mhz_0(clk25mhz_0),
    .clk50mhz_0(clk50mhz_0),
    .locked_0(),
    .interconnect_aresetn_0(interconnect_aresetn_0),
    .peripheral_aresetn_0(peripheral_aresetn_0),
    .peripheral_reset_0(peripheral_reset_0)
  );

  reconf_part_0 inst_rp(
    .*
  );

endmodule

`ifdef SYNTHESIS
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
endmodule
`endif
