interface decode_sc_module_1_interface;
  logic [0:0] clk;
  logic [0:0] reset;
  logic [15:0] maxbits;
  logic [15:0] minbits;
  logic [15:0] maxprec;
  logic signed[15:0] minexp;
  logic [63:0] s_bits_data_tdata;
  logic [0:0] s_bits_data_tlast;
  logic [0:0] s_bits_valid;
  logic [0:0] s_bits_ready;
  logic [51:0] m_stream_data_frac;
  logic [10:0] m_stream_data_expo;
  logic [0:0] m_stream_data_sign;
  logic [0:0] m_stream_valid;
  logic [0:0] m_stream_ready;
  modport port0(
    input clk,reset,maxbits,minbits,maxprec,minexp,s_bits_data_tdata,s_bits_data_tlast,s_bits_valid,m_stream_ready,
    output s_bits_ready,m_stream_data_frac,m_stream_data_expo,m_stream_data_sign,m_stream_valid
  );
endinterface

interface decode_stream_sc_module_2_interface;
  logic [0:0] clk;
  logic [0:0] reset;
  logic [15:0] s_minbits;
  logic [15:0] s_maxbits;
  logic [15:0] s_maxprec;
  logic signed[15:0] s_minexp;
  logic [0:0] s_blk_start_data;
  logic [0:0] s_blk_start_valid;
  logic [0:0] s_blk_start_ready;
  logic [5:0] s_bc_data;
  logic [0:0] s_bc_valid;
  logic [0:0] s_bc_ready;
  logic [63:0] s_bits_data_tdata;
  logic [0:0] s_bits_data_tlast;
  logic [0:0] s_bits_valid;
  logic [0:0] s_bits_ready;
  logic [15:0] m_block_maxprec;
  logic [0:0] m_bhdr_data_zb;
  logic [10:0] m_bhdr_data_exp;
  logic [0:0] m_bhdr_valid;
  logic [0:0] m_bhdr_ready;
  logic [31:0] m_bp_data;
  logic [0:0] m_bp_valid;
  logic [0:0] m_bp_ready;
  modport port0(
    input clk,reset,s_minbits,s_maxbits,s_maxprec,s_minexp,s_blk_start_data,s_blk_start_valid,s_bc_data,s_bc_valid,s_bits_data_tdata,s_bits_data_tlast,s_bits_valid,m_bhdr_ready,m_bp_ready,
    output s_blk_start_ready,s_bc_ready,s_bits_ready,m_block_maxprec,m_bhdr_data_zb,m_bhdr_data_exp,m_bhdr_valid,m_bp_data,m_bp_valid
  );
endinterface

interface rvfifo_cc_sc_module_9_interface;
  logic [0:0] clk;
  logic [0:0] reset;
  logic [63:0] s_port_data_tdata;
  logic [0:0] s_port_data_tlast;
  logic [0:0] s_port_valid;
  logic [0:0] s_port_ready;
  logic [63:0] m_port_data_tdata;
  logic [0:0] m_port_data_tlast;
  logic [0:0] m_port_valid;
  logic [0:0] m_port_ready;
  modport port0(
    input clk,reset,s_port_data_tdata,s_port_data_tlast,s_port_valid,m_port_ready,
    output s_port_ready,m_port_data_tdata,m_port_data_tlast,m_port_valid
  );
endinterface

interface fifo_cc_sc_module_10_interface;
  logic [0:0] clk;
  logic [0:0] reset;
  logic [63:0] din_tdata;
  logic [0:0] din_tlast;
  logic [0:0] wr_en;
  logic [0:0] rd_en;
  logic [0:0] full;
  logic [63:0] dout_tdata;
  logic [0:0] dout_tlast;
  logic [0:0] empty;
  modport port0(
    input clk,reset,din_tdata,din_tlast,wr_en,rd_en,
    output full,dout_tdata,dout_tlast,empty
  );
endinterface

interface rvfifo_cc_sc_module_3_interface;
  logic [0:0] clk;
  logic [0:0] reset;
  logic [0:0] s_port_data_zb;
  logic [10:0] s_port_data_exp;
  logic [0:0] s_port_valid;
  logic [0:0] s_port_ready;
  logic [0:0] m_port_data_zb;
  logic [10:0] m_port_data_exp;
  logic [0:0] m_port_valid;
  logic [0:0] m_port_ready;
  modport port0(
    input clk,reset,s_port_data_zb,s_port_data_exp,s_port_valid,m_port_ready,
    output s_port_ready,m_port_data_zb,m_port_data_exp,m_port_valid
  );
endinterface

interface fifo_cc_sc_module_11_interface;
  logic [0:0] clk;
  logic [0:0] reset;
  logic [0:0] din_zb;
  logic [10:0] din_exp;
  logic [0:0] wr_en;
  logic [0:0] rd_en;
  logic [0:0] full;
  logic [0:0] dout_zb;
  logic [10:0] dout_exp;
  logic [0:0] empty;
  modport port0(
    input clk,reset,din_zb,din_exp,wr_en,rd_en,
    output full,dout_zb,dout_exp,empty
  );
endinterface

interface decode_ints_sc_module_4_interface;
  logic [0:0] clk;
  logic [0:0] reset;
  logic [15:0] s_maxbits;
  logic [15:0] s_maxprec;
  logic [15:0] s_minbits;
  logic [0:0] m_ready;
  logic [31:0] s_bp_data;
  logic [0:0] s_bp_valid;
  logic [0:0] s_bp_ready;
  logic [63:0] m_block[0:15];
  logic [0:0] m_valid;
  logic [5:0] m_bc_data;
  logic [0:0] m_bc_valid;
  logic [0:0] m_bc_ready;
  logic [0:0] m_blk_start_data;
  logic [0:0] m_blk_start_valid;
  logic [0:0] m_blk_start_ready;
  modport port0(
    input clk,reset,s_maxbits,s_maxprec,s_minbits,m_ready,s_bp_data,s_bp_valid,m_bc_ready,m_blk_start_ready,
    output s_bp_ready,m_block,m_valid,m_bc_data,m_bc_valid,m_blk_start_data,m_blk_start_valid
  );
endinterface

interface block_buffer_sc_module_5_interface;
  logic [0:0] clk;
  logic [0:0] reset;
  logic [63:0] s_block[0:15];
  logic [0:0] s_valid;
  logic [0:0] m_ready;
  logic [0:0] s_ready;
  logic [63:0] m_block[0:15];
  logic [0:0] m_valid;
  modport port0(
    input clk,reset,s_block,s_valid,m_ready,
    output s_ready,m_block,m_valid
  );
endinterface

interface fifo_cc_sc_module_12_interface;
  logic [0:0] clk;
  logic [0:0] reset;
  logic [1023:0] din;
  logic [0:0] wr_en;
  logic [0:0] rd_en;
  logic [0:0] full;
  logic [1023:0] dout;
  logic [0:0] empty;
  modport port0(
    input clk,reset,din,wr_en,rd_en,
    output full,dout,empty
  );
endinterface

interface decode_block_sc_module_6_interface;
  logic [0:0] clk;
  logic [0:0] reset;
  logic [63:0] s_block[0:15];
  logic [0:0] s_valid;
  logic [0:0] m_ready;
  logic [0:0] s_ready;
  logic signed[63:0] m_block[0:15];
  logic [0:0] m_valid;
  modport port0(
    input clk,reset,s_block,s_valid,m_ready,
    output s_ready,m_block,m_valid
  );
endinterface

interface inv_lift_sc_module_13_interface;
  logic [0:0] clk;
  logic [0:0] reset;
  logic signed[63:0] s_port[0:3];
  logic [0:0] s_valid;
  logic [0:0] m_ready;
  logic [0:0] s_ready;
  logic signed[63:0] m_port[0:3];
  logic [0:0] m_valid;
  modport port0(
    input clk,reset,s_port,s_valid,m_ready,
    output s_ready,m_port,m_valid
  );
endinterface

interface inv_lift_sc_module_14_interface;
  logic [0:0] clk;
  logic [0:0] reset;
  logic signed[63:0] s_port[0:3];
  logic [0:0] s_valid;
  logic [0:0] m_ready;
  logic [0:0] s_ready;
  logic signed[63:0] m_port[0:3];
  logic [0:0] m_valid;
  modport port0(
    input clk,reset,s_port,s_valid,m_ready,
    output s_ready,m_port,m_valid
  );
endinterface

interface block_buffer_sc_module_7_interface;
  logic [0:0] clk;
  logic [0:0] reset;
  logic signed[63:0] s_block[0:15];
  logic [0:0] s_valid;
  logic [0:0] m_ready;
  logic [0:0] s_ready;
  logic signed[63:0] m_block[0:15];
  logic [0:0] m_valid;
  modport port0(
    input clk,reset,s_block,s_valid,m_ready,
    output s_ready,m_block,m_valid
  );
endinterface

interface fifo_cc_sc_module_15_interface;
  logic [0:0] clk;
  logic [0:0] reset;
  logic [1023:0] din;
  logic [0:0] wr_en;
  logic [0:0] rd_en;
  logic [0:0] full;
  logic [1023:0] dout;
  logic [0:0] empty;
  modport port0(
    input clk,reset,din,wr_en,rd_en,
    output full,dout,empty
  );
endinterface

interface inv_cast_sc_module_8_interface;
  logic [0:0] clk;
  logic [0:0] reset;
  logic signed[63:0] s_block[0:15];
  logic [0:0] s_valid;
  logic [0:0] s_bhdr_data_zb;
  logic [10:0] s_bhdr_data_exp;
  logic [0:0] s_bhdr_valid;
  logic [0:0] s_bhdr_ready;
  logic [0:0] s_ready;
  logic [51:0] m_stream_data_frac;
  logic [10:0] m_stream_data_expo;
  logic [0:0] m_stream_data_sign;
  logic [0:0] m_stream_valid;
  logic [0:0] m_stream_ready;
  modport port0(
    input clk,reset,s_block,s_valid,s_bhdr_data_zb,s_bhdr_data_exp,s_bhdr_valid,m_stream_ready,
    output s_bhdr_ready,s_ready,m_stream_data_frac,m_stream_data_expo,m_stream_data_sign,m_stream_valid
  );
endinterface

module mymodule_sc_module_0 (
);
  logic [0:0] clk_scclang_global_0;
  logic [15:0] maxbits_scclang_global_1;
  logic [15:0] maxprec_scclang_global_2;
  logic [15:0] minbits_scclang_global_3;
  logic signed[15:0] minexp_scclang_global_4;
  logic [0:0] reset_scclang_global_5;
  logic [51:0] c_dut_fp_scclang_global_6_data_frac;
  logic [10:0] c_dut_fp_scclang_global_6_data_expo;
  logic [0:0] c_dut_fp_scclang_global_6_data_sign;
  logic [0:0] c_dut_fp_scclang_global_6_valid;
  logic [0:0] c_dut_fp_scclang_global_6_ready;
  logic [63:0] c_driver_enc_scclang_global_7_data_tdata;
  logic [0:0] c_driver_enc_scclang_global_7_data_tlast;
  logic [0:0] c_driver_enc_scclang_global_7_valid;
  logic [0:0] c_driver_enc_scclang_global_7_ready;
  initial begin
    minbits_scclang_global_3 = 32;
    maxbits_scclang_global_1 = 32;
    maxprec_scclang_global_2 = 64;
    minexp_scclang_global_4 = 64462;
  end
  decode_sc_module_1_interface u_dut_itf();

  decode_sc_module_1 u_dut(
    u_dut_itf
  );
    assign u_dut_itf.clk = clk_scclang_global_0;
    assign u_dut_itf.reset = reset_scclang_global_5;
    assign u_dut_itf.minbits = minbits_scclang_global_3;
    assign u_dut_itf.maxbits = maxbits_scclang_global_1;
    assign u_dut_itf.maxprec = maxprec_scclang_global_2;
    assign u_dut_itf.minexp = minexp_scclang_global_4;
    assign u_dut_itf.s_bits_data_tdata = c_driver_enc_scclang_global_7_data_tdata;
    assign u_dut_itf.s_bits_data_tlast = c_driver_enc_scclang_global_7_data_tlast;
    assign u_dut_itf.s_bits_valid = c_driver_enc_scclang_global_7_valid;
    assign c_driver_enc_scclang_global_7_ready = u_dut_itf.s_bits_ready;
    assign c_dut_fp_scclang_global_6_data_frac = u_dut_itf.m_stream_data_frac;
    assign c_dut_fp_scclang_global_6_data_expo = u_dut_itf.m_stream_data_expo;
    assign c_dut_fp_scclang_global_6_data_sign = u_dut_itf.m_stream_data_sign;
    assign c_dut_fp_scclang_global_6_valid = u_dut_itf.m_stream_valid;
    assign u_dut_itf.m_stream_ready = c_dut_fp_scclang_global_6_ready;
  always @(*) begin
  end

endmodule
module decode_sc_module_1 (
 decode_sc_module_1_interface.port0 itf
);
  logic [63:0] c_b_m_block_scclang_global_0[0:15];
  logic [0:0] c_b_m_ready_scclang_global_1;
  logic [0:0] c_b_m_valid_scclang_global_2;
  logic [15:0] c_block_maxprec_scclang_global_3;
  logic signed[63:0] c_c_s_block_scclang_global_4[0:15];
  logic [0:0] c_c_s_ready_scclang_global_5;
  logic [0:0] c_c_s_valid_scclang_global_6;
  logic [63:0] c_i_m_block_scclang_global_7[0:15];
  logic [0:0] c_i_m_ready_scclang_global_8;
  logic [0:0] c_i_m_valid_scclang_global_9;
  logic signed[63:0] c_l_m_block_scclang_global_10[0:15];
  logic [0:0] c_l_m_ready_scclang_global_11;
  logic [0:0] c_l_m_valid_scclang_global_12;
  logic [63:0] _c_s_bits_scclang_global_13_data_tdata;
  logic [0:0] _c_s_bits_scclang_global_13_data_tlast;
  logic [0:0] _c_s_bits_scclang_global_13_valid;
  logic [0:0] _c_s_bits_scclang_global_13_ready;
  logic [5:0] c_bc_scclang_global_14_data;
  logic [0:0] c_bc_scclang_global_14_valid;
  logic [0:0] c_bc_scclang_global_14_ready;
  logic [31:0] c_bp_scclang_global_15_data;
  logic [0:0] c_bp_scclang_global_15_valid;
  logic [0:0] c_bp_scclang_global_15_ready;
  logic [0:0] c_shdr_scclang_global_16_data_zb;
  logic [10:0] c_shdr_scclang_global_16_data_exp;
  logic [0:0] c_shdr_scclang_global_16_valid;
  logic [0:0] c_shdr_scclang_global_16_ready;
  logic [0:0] c_fhdr_scclang_global_17_data_zb;
  logic [10:0] c_fhdr_scclang_global_17_data_exp;
  logic [0:0] c_fhdr_scclang_global_17_valid;
  logic [0:0] c_fhdr_scclang_global_17_ready;
  logic [0:0] c_blk_start_scclang_global_18_data;
  logic [0:0] c_blk_start_scclang_global_18_valid;
  logic [0:0] c_blk_start_scclang_global_18_ready;
  initial begin

  end
  decode_stream_sc_module_2_interface u_decode_stream_itf();

  decode_stream_sc_module_2 u_decode_stream(
    u_decode_stream_itf
  );
    assign u_decode_stream_itf.clk = itf.clk;
    assign u_decode_stream_itf.reset = itf.reset;
    assign u_decode_stream_itf.s_maxbits = itf.maxbits;
    assign u_decode_stream_itf.s_maxprec = itf.maxprec;
    assign u_decode_stream_itf.s_minbits = itf.minbits;
    assign u_decode_stream_itf.s_minexp = itf.minexp;
    assign u_decode_stream_itf.s_blk_start_data = c_blk_start_scclang_global_18_data;
    assign u_decode_stream_itf.s_blk_start_valid = c_blk_start_scclang_global_18_valid;
    assign c_blk_start_scclang_global_18_ready = u_decode_stream_itf.s_blk_start_ready;
    assign u_decode_stream_itf.s_bits_data_tdata = itf.s_bits_data_tdata;
    assign u_decode_stream_itf.s_bits_data_tlast = itf.s_bits_data_tlast;
    assign u_decode_stream_itf.s_bits_valid = itf.s_bits_valid;
    assign itf.s_bits_ready = u_decode_stream_itf.s_bits_ready;
    assign u_decode_stream_itf.s_bc_data = c_bc_scclang_global_14_data;
    assign u_decode_stream_itf.s_bc_valid = c_bc_scclang_global_14_valid;
    assign c_bc_scclang_global_14_ready = u_decode_stream_itf.s_bc_ready;
    assign c_bp_scclang_global_15_data = u_decode_stream_itf.m_bp_data;
    assign c_bp_scclang_global_15_valid = u_decode_stream_itf.m_bp_valid;
    assign u_decode_stream_itf.m_bp_ready = c_bp_scclang_global_15_ready;
    assign c_shdr_scclang_global_16_data_zb = u_decode_stream_itf.m_bhdr_data_zb;
    assign c_shdr_scclang_global_16_data_exp = u_decode_stream_itf.m_bhdr_data_exp;
    assign c_shdr_scclang_global_16_valid = u_decode_stream_itf.m_bhdr_valid;
    assign u_decode_stream_itf.m_bhdr_ready = c_shdr_scclang_global_16_ready;
    assign c_block_maxprec_scclang_global_3 = u_decode_stream_itf.m_block_maxprec;
  always @(*) begin
  end

  rvfifo_cc_sc_module_3_interface u_hfifo_itf();

  rvfifo_cc_sc_module_3 u_hfifo(
    u_hfifo_itf
  );
    assign u_hfifo_itf.clk = itf.clk;
    assign u_hfifo_itf.reset = itf.reset;
    assign u_hfifo_itf.s_port_data_zb = c_shdr_scclang_global_16_data_zb;
    assign u_hfifo_itf.s_port_data_exp = c_shdr_scclang_global_16_data_exp;
    assign u_hfifo_itf.s_port_valid = c_shdr_scclang_global_16_valid;
    assign c_shdr_scclang_global_16_ready = u_hfifo_itf.s_port_ready;
    assign c_fhdr_scclang_global_17_data_zb = u_hfifo_itf.m_port_data_zb;
    assign c_fhdr_scclang_global_17_data_exp = u_hfifo_itf.m_port_data_exp;
    assign c_fhdr_scclang_global_17_valid = u_hfifo_itf.m_port_valid;
    assign u_hfifo_itf.m_port_ready = c_fhdr_scclang_global_17_ready;
  always @(*) begin
  end

  decode_ints_sc_module_4_interface u_decode_ints_itf();

  decode_ints_sc_module_4 u_decode_ints(
    u_decode_ints_itf
  );
    assign u_decode_ints_itf.clk = itf.clk;
    assign u_decode_ints_itf.reset = itf.reset;
    assign u_decode_ints_itf.s_bp_data = c_bp_scclang_global_15_data;
    assign u_decode_ints_itf.s_bp_valid = c_bp_scclang_global_15_valid;
    assign c_bp_scclang_global_15_ready = u_decode_ints_itf.s_bp_ready;
    assign u_decode_ints_itf.s_maxbits = itf.maxbits;
    assign u_decode_ints_itf.s_minbits = itf.minbits;
    assign u_decode_ints_itf.s_maxprec = c_block_maxprec_scclang_global_3;
    assign c_bc_scclang_global_14_data = u_decode_ints_itf.m_bc_data;
    assign c_bc_scclang_global_14_valid = u_decode_ints_itf.m_bc_valid;
    assign u_decode_ints_itf.m_bc_ready = c_bc_scclang_global_14_ready;
    assign c_blk_start_scclang_global_18_data = u_decode_ints_itf.m_blk_start_data;
    assign c_blk_start_scclang_global_18_valid = u_decode_ints_itf.m_blk_start_valid;
    assign u_decode_ints_itf.m_blk_start_ready = c_blk_start_scclang_global_18_ready;
    assign c_i_m_valid_scclang_global_9 = u_decode_ints_itf.m_valid;
    assign u_decode_ints_itf.m_ready = c_i_m_ready_scclang_global_8;
  always @(*) begin
  end

  block_buffer_sc_module_5_interface u_block_buffer_itf();

  block_buffer_sc_module_5 u_block_buffer(
    u_block_buffer_itf
  );
    assign u_block_buffer_itf.clk = itf.clk;
    assign u_block_buffer_itf.reset = itf.reset;
    assign u_block_buffer_itf.s_valid = c_i_m_valid_scclang_global_9;
    assign c_i_m_ready_scclang_global_8 = u_block_buffer_itf.s_ready;
    assign c_b_m_valid_scclang_global_2 = u_block_buffer_itf.m_valid;
    assign u_block_buffer_itf.m_ready = c_b_m_ready_scclang_global_1;
  always @(*) begin
  end

  decode_block_sc_module_6_interface u_decode_block_itf();

  decode_block_sc_module_6 u_decode_block(
    u_decode_block_itf
  );
    assign u_decode_block_itf.clk = itf.clk;
    assign u_decode_block_itf.reset = itf.reset;
    assign u_decode_block_itf.s_valid = c_b_m_valid_scclang_global_2;
    assign c_b_m_ready_scclang_global_1 = u_decode_block_itf.s_ready;
    assign c_l_m_valid_scclang_global_12 = u_decode_block_itf.m_valid;
    assign u_decode_block_itf.m_ready = c_l_m_ready_scclang_global_11;
  always @(*) begin
  end

  block_buffer_sc_module_7_interface u_cast_buffer_itf();

  block_buffer_sc_module_7 u_cast_buffer(
    u_cast_buffer_itf
  );
    assign u_cast_buffer_itf.clk = itf.clk;
    assign u_cast_buffer_itf.reset = itf.reset;
    assign u_cast_buffer_itf.s_valid = c_l_m_valid_scclang_global_12;
    assign c_l_m_ready_scclang_global_11 = u_cast_buffer_itf.s_ready;
    assign c_c_s_valid_scclang_global_6 = u_cast_buffer_itf.m_valid;
    assign u_cast_buffer_itf.m_ready = c_c_s_ready_scclang_global_5;
  always @(*) begin
  end

  inv_cast_sc_module_8_interface u_inv_cast_itf();

  inv_cast_sc_module_8 u_inv_cast(
    u_inv_cast_itf
  );
    assign u_inv_cast_itf.clk = itf.clk;
    assign u_inv_cast_itf.reset = itf.reset;
    assign u_inv_cast_itf.s_bhdr_data_zb = c_fhdr_scclang_global_17_data_zb;
    assign u_inv_cast_itf.s_bhdr_data_exp = c_fhdr_scclang_global_17_data_exp;
    assign u_inv_cast_itf.s_bhdr_valid = c_fhdr_scclang_global_17_valid;
    assign c_fhdr_scclang_global_17_ready = u_inv_cast_itf.s_bhdr_ready;
    assign u_inv_cast_itf.s_valid = c_c_s_valid_scclang_global_6;
    assign c_c_s_ready_scclang_global_5 = u_inv_cast_itf.s_ready;
    assign itf.m_stream_data_frac = u_inv_cast_itf.m_stream_data_frac;
    assign itf.m_stream_data_expo = u_inv_cast_itf.m_stream_data_expo;
    assign itf.m_stream_data_sign = u_inv_cast_itf.m_stream_data_sign;
    assign itf.m_stream_valid = u_inv_cast_itf.m_stream_valid;
    assign u_inv_cast_itf.m_stream_ready = itf.m_stream_ready;
  always @(*) begin
  end

  genvar i_u_dut_local_7;
  genvar i_u_dut_local_6;
  genvar i_u_dut_local_1;
  genvar i_u_dut_local_0;
  genvar i_u_dut_local_3;
  genvar i_u_dut_local_5;
  genvar i_u_dut_local_4;
  genvar i_u_dut_local_2;
  for (i_u_dut_local_0 = 0;(i_u_dut_local_0) < (16);i_u_dut_local_0++) begin
    assign c_i_m_block_scclang_global_7[i_u_dut_local_0] = u_decode_ints_itf.m_block[i_u_dut_local_0];
  end
  for (i_u_dut_local_1 = 0;(i_u_dut_local_1) < (16);i_u_dut_local_1++) begin
        assign u_block_buffer_itf.s_block[i_u_dut_local_1] = c_i_m_block_scclang_global_7[i_u_dut_local_1];
  end
  for (i_u_dut_local_2 = 0;(i_u_dut_local_2) < (16);i_u_dut_local_2++) begin
    assign c_b_m_block_scclang_global_0[i_u_dut_local_2] = u_block_buffer_itf.m_block[i_u_dut_local_2];
  end
  for (i_u_dut_local_3 = 0;(i_u_dut_local_3) < (16);i_u_dut_local_3++) begin
        assign u_decode_block_itf.s_block[i_u_dut_local_3] = c_b_m_block_scclang_global_0[i_u_dut_local_3];
  end
  for (i_u_dut_local_4 = 0;(i_u_dut_local_4) < (16);i_u_dut_local_4++) begin
    assign c_l_m_block_scclang_global_10[i_u_dut_local_4] = u_decode_block_itf.m_block[i_u_dut_local_4];
  end
  for (i_u_dut_local_5 = 0;(i_u_dut_local_5) < (16);i_u_dut_local_5++) begin
        assign u_cast_buffer_itf.s_block[i_u_dut_local_5] = c_l_m_block_scclang_global_10[i_u_dut_local_5];
  end
  for (i_u_dut_local_6 = 0;(i_u_dut_local_6) < (16);i_u_dut_local_6++) begin
    assign c_c_s_block_scclang_global_4[i_u_dut_local_6] = u_cast_buffer_itf.m_block[i_u_dut_local_6];
  end
  for (i_u_dut_local_7 = 0;(i_u_dut_local_7) < (16);i_u_dut_local_7++) begin
        assign u_inv_cast_itf.s_block[i_u_dut_local_7] = c_c_s_block_scclang_global_4[i_u_dut_local_7];
  end
endmodule
module decode_stream_sc_module_2 (
 decode_stream_sc_module_2_interface.port0 itf
);
  logic [0:0] b_c_scclang_global_0_f[0:3];
  logic [31:0] b_c_scclang_global_0_w[0:3];
  logic [15:0] bits_scclang_global_1;
  logic signed[15:0] c_rembits_scclang_global_2;
  logic [7:0] c_wordoff_scclang_global_3;
  logic [0:0] csync_scclang_global_4;
  logic [15:0] minbits_scclang_global_5;
  logic [0:0] skpbts_scclang_global_6;
  logic [63:0] c_s_bfifo_scclang_global_7_data_tdata;
  logic [0:0] c_s_bfifo_scclang_global_7_data_tlast;
  logic [0:0] c_s_bfifo_scclang_global_7_valid;
  logic [0:0] c_s_bfifo_scclang_global_7_ready;
  logic [63:0] c_m_bfifo_scclang_global_8_data_tdata;
  logic [0:0] c_m_bfifo_scclang_global_8_data_tlast;
  logic [0:0] c_m_bfifo_scclang_global_8_valid;
  logic [0:0] c_m_bfifo_scclang_global_8_ready;
  logic [6:0] bitoff_actual_scclang_global_9;
  logic [0:0] _zb_actual_scclang_global_10;
  logic [0:0] _zb_actual_scclang_global_11;
  logic [10:0] _exp_actual_scclang_global_12;
  logic [0:0] _zb_actual_scclang_global_13;
  logic [10:0] maxexp_actual_scclang_global_14;
  initial begin

  end
  rvfifo_cc_sc_module_9_interface u_bfifo_itf();

  rvfifo_cc_sc_module_9 u_bfifo(
    u_bfifo_itf
  );
    assign u_bfifo_itf.clk = itf.clk;
    assign u_bfifo_itf.reset = itf.reset;
    assign u_bfifo_itf.s_port_data_tdata = c_s_bfifo_scclang_global_7_data_tdata;
    assign u_bfifo_itf.s_port_data_tlast = c_s_bfifo_scclang_global_7_data_tlast;
    assign u_bfifo_itf.s_port_valid = c_s_bfifo_scclang_global_7_valid;
    assign c_s_bfifo_scclang_global_7_ready = u_bfifo_itf.s_port_ready;
    assign c_m_bfifo_scclang_global_8_data_tdata = u_bfifo_itf.m_port_data_tdata;
    assign c_m_bfifo_scclang_global_8_data_tlast = u_bfifo_itf.m_port_data_tlast;
    assign c_m_bfifo_scclang_global_8_valid = u_bfifo_itf.m_port_valid;
    assign u_bfifo_itf.m_port_ready = c_m_bfifo_scclang_global_8_ready;
  always @(*) begin
  end

  always @(itf.s_bits_valid or itf.s_bits_data_tdata or itf.s_bits_data_tlast or itf.s_bc_valid or c_s_bfifo_scclang_global_7_ready or itf.m_bhdr_ready or itf.m_bp_ready or itf.s_minbits or itf.s_maxbits or itf.s_maxprec or itf.s_minexp or itf.s_blk_start_valid or itf.s_blk_start_data or c_rembits_scclang_global_2) begin: mc_proc
    logic [0:0] _s_blk_cycle_mc_proc_local_3;
    logic [0:0] stall_for_fifo_mc_proc_local_4;
    logic [0:0] stall_for_exp_mc_proc_local_5;
    
    _s_blk_cycle_mc_proc_local_3 = ((itf.s_blk_start_data) == (1)) && (itf.s_blk_start_valid);
    stall_for_fifo_mc_proc_local_4 = !(b_c_scclang_global_0_f[1]);
    stall_for_exp_mc_proc_local_5 = (_s_blk_cycle_mc_proc_local_3) & (!(itf.m_bhdr_ready));
    csync_scclang_global_4 <= (stall_for_fifo_mc_proc_local_4) || (stall_for_exp_mc_proc_local_5);
    c_s_bfifo_scclang_global_7_data_tdata <= itf.s_bits_data_tdata;
    c_s_bfifo_scclang_global_7_data_tlast <= itf.s_bits_data_tlast;
    itf.s_bits_ready <= c_s_bfifo_scclang_global_7_ready;
    c_s_bfifo_scclang_global_7_valid <= itf.s_bits_valid;
  end
  always_ff @(negedge itf.clk) begin: ms_proc
    logic [0:0] b_c_init_v_ms_proc_local_6_f;
    logic [31:0] b_c_init_v_ms_proc_local_6_w;
    logic signed[31:0] i_ms_proc_local_7;
    logic [0:0] b_wrk_ms_proc_local_8_f[0:3];
    logic [31:0] b_wrk_ms_proc_local_8_w[0:3];
    logic [63:0] i_ms_proc_local_9;
    logic [7:0] w_wordoff_ms_proc_local_10;
    logic [0:0] _s_blk_cycle_ms_proc_local_11;
    logic [0:0] bhdr_ms_proc_local_12_zb;
    logic [10:0] bhdr_ms_proc_local_12_exp;
    logic signed[15:0] w_rembits_ms_proc_local_13;
    logic signed[15:0] dreg_bits_ms_proc_local_14;
    logic [10:0] blockexpt_ms_proc_local_15;
    logic [31:0] planewdw_ms_proc_local_16;
    logic [0:0] b_c_init_v_ms_proc_local_6_f__ref_0;
    logic [10:0] bhdr_ms_proc_local_12_exp__ref_0;
    logic [31:0] b_wrk_ms_proc_local_8_w__ref_0[0:3];
    logic [0:0] bhdr_ms_proc_local_12_zb__ref_0;
    logic [0:0] b_wrk_ms_proc_local_8_f__ref_0[0:3];
    logic [31:0] b_c_init_v_ms_proc_local_6_w__ref_0;
    
    b_c_init_v_ms_proc_local_6_w__ref_0 = b_c_init_v_ms_proc_local_6_w;
    b_wrk_ms_proc_local_8_f__ref_0 = b_wrk_ms_proc_local_8_f;
    bhdr_ms_proc_local_12_zb__ref_0 = bhdr_ms_proc_local_12_zb;
    b_wrk_ms_proc_local_8_w__ref_0 = b_wrk_ms_proc_local_8_w;
    bhdr_ms_proc_local_12_exp__ref_0 = bhdr_ms_proc_local_12_exp;
    b_c_init_v_ms_proc_local_6_f__ref_0 = b_c_init_v_ms_proc_local_6_f;
    if ((itf.reset) == (0)) begin
      minbits_scclang_global_5 <= (itf.s_maxbits) - (itf.s_minbits);
      c_m_bfifo_scclang_global_8_ready <= 0;
      skpbts_scclang_global_6 <= 0;
      itf.s_bc_ready <= 0;
      itf.m_bp_valid <= 0;
      c_wordoff_scclang_global_3 <= 8'd0;
      c_rembits_scclang_global_2 <= 0;
      // TODO: fix this function call [Token('ID', 'zhw__plane_reg2__plane_reg_func_0'), 'b_c_init_v_ms_proc_local_6_f__ref_0', 'b_c_init_v_ms_proc_local_6_w__ref_0', 'b_c_init_v_ms_proc_local_6_f__ref_0', 'b_c_init_v_ms_proc_local_6_w__ref_0'];
      b_c_init_v_ms_proc_local_6_f__ref_0 = 0;
      b_c_init_v_ms_proc_local_6_w__ref_0 = 0;
      for (i_ms_proc_local_7 = 0;(i_ms_proc_local_7) < (4);i_ms_proc_local_7++) begin
        b_c_scclang_global_0_f[i_ms_proc_local_7] <= b_c_init_v_ms_proc_local_6_f__ref_0;
        b_c_scclang_global_0_w[i_ms_proc_local_7] <= b_c_init_v_ms_proc_local_6_w__ref_0;
      end
      bits_scclang_global_1 <= 0;
    end else begin
      // TODO: fix this function call [Token('ID', 'zhw__plane_reg2__plane_reg_func_0'), 'b_wrk_ms_proc_local_8_f__ref_0', 'b_wrk_ms_proc_local_8_w__ref_0', 'b_wrk_ms_proc_local_8_f__ref_0', 'b_wrk_ms_proc_local_8_w__ref_0'];
      for (i_ms_proc_local_9 = 0;(i_ms_proc_local_9) < (4);i_ms_proc_local_9++) begin
        b_wrk_ms_proc_local_8_f__ref_0[i_ms_proc_local_9] = b_c_scclang_global_0_f[i_ms_proc_local_9];
        b_wrk_ms_proc_local_8_w__ref_0[i_ms_proc_local_9] = b_c_scclang_global_0_w[i_ms_proc_local_9];
      end
      w_wordoff_ms_proc_local_10 = c_wordoff_scclang_global_3;
      _s_blk_cycle_ms_proc_local_11 = ((itf.s_blk_start_data) == (1)) && (itf.s_blk_start_valid);
      zhw__block_headerfp_t11_52__block_header_func_1(bhdr_ms_proc_local_12_zb__ref_0,bhdr_ms_proc_local_12_exp__ref_0,1,bhdr_ms_proc_local_12_zb__ref_0,bhdr_ms_proc_local_12_exp__ref_0);
      zhw__block_headerfp_t11_52__init_func_2(bhdr_ms_proc_local_12_zb__ref_0,bhdr_ms_proc_local_12_exp__ref_0,1,bhdr_ms_proc_local_12_zb__ref_0,bhdr_ms_proc_local_12_exp__ref_0);
      if ((!(csync_scclang_global_4)) && ((_s_blk_cycle_ms_proc_local_11) || (skpbts_scclang_global_6))) begin
        w_rembits_ms_proc_local_13 = c_rembits_scclang_global_2;
        if ((w_rembits_ms_proc_local_13) > (0)) begin
          if ((w_rembits_ms_proc_local_13) / (32)) begin
            dreg_bits_ms_proc_local_14 = 32;
            zhw__get_window_func_3(b_wrk_ms_proc_local_8_f__ref_0,b_wrk_ms_proc_local_8_w__ref_0,dreg_bits_ms_proc_local_14,b_wrk_ms_proc_local_8_f__ref_0,b_wrk_ms_proc_local_8_w__ref_0);
          end else begin
            dreg_bits_ms_proc_local_14 = w_rembits_ms_proc_local_13;
            zhw__get_window_func_3(b_wrk_ms_proc_local_8_f__ref_0,b_wrk_ms_proc_local_8_w__ref_0,(c_wordoff_scclang_global_3) + (dreg_bits_ms_proc_local_14),b_wrk_ms_proc_local_8_f__ref_0,b_wrk_ms_proc_local_8_w__ref_0);
          end

          c_rembits_scclang_global_2 <= (w_rembits_ms_proc_local_13) - (dreg_bits_ms_proc_local_14);
          if ((w_rembits_ms_proc_local_13) > (32)) begin
            skpbts_scclang_global_6 <= 1;
          end else begin
            c_wordoff_scclang_global_3 <= ((c_wordoff_scclang_global_3) + (dreg_bits_ms_proc_local_14)) % (32);
            skpbts_scclang_global_6 <= 0;
          end

        end else begin
          skpbts_scclang_global_6 <= 0;
        end

      end
      if ((((!(csync_scclang_global_4)) && (_s_blk_cycle_ms_proc_local_11)) && (!(skpbts_scclang_global_6))) && ((itf.m_bp_ready) && (itf.s_bc_valid))) begin
        if ((c_rembits_scclang_global_2) == (0)) begin
          w_rembits_ms_proc_local_13 = itf.s_maxbits;
          zhw__block_headerfp_t11_52__set_zb_func_4(bhdr_ms_proc_local_12_zb__ref_0,bhdr_ms_proc_local_12_exp__ref_0,!(zhw__get_window_func_3(b_wrk_ms_proc_local_8_f__ref_0,b_wrk_ms_proc_local_8_w__ref_0,w_wordoff_ms_proc_local_10,b_wrk_ms_proc_local_8_f__ref_0,b_wrk_ms_proc_local_8_w__ref_0) & (1 << 0)),bhdr_ms_proc_local_12_zb__ref_0,bhdr_ms_proc_local_12_exp__ref_0);
          w_wordoff_ms_proc_local_10 = (w_wordoff_ms_proc_local_10) + (1);
          w_wordoff_ms_proc_local_10 = (w_wordoff_ms_proc_local_10) % (32);
          w_rembits_ms_proc_local_13 = (w_rembits_ms_proc_local_13) - (1);
          if (!(bhdr_ms_proc_local_12_zb__ref_0)) begin
            blockexpt_ms_proc_local_15 = zhw__get_window_func_3(b_wrk_ms_proc_local_8_f__ref_0,b_wrk_ms_proc_local_8_w__ref_0,w_wordoff_ms_proc_local_10,b_wrk_ms_proc_local_8_f__ref_0,b_wrk_ms_proc_local_8_w__ref_0);
            w_wordoff_ms_proc_local_10 = (w_wordoff_ms_proc_local_10) + (11);
            w_wordoff_ms_proc_local_10 = (w_wordoff_ms_proc_local_10) % (32);
            w_rembits_ms_proc_local_13 = (w_rembits_ms_proc_local_13) - (11);
            itf.m_block_maxprec <= zhw__decode_streamfp_t11_52_bits_t64_2__get_block_maxprec_func_5(blockexpt_ms_proc_local_15);
            zhw__block_headerfp_t11_52__set_exp_func_6(bhdr_ms_proc_local_12_zb__ref_0,bhdr_ms_proc_local_12_exp__ref_0,blockexpt_ms_proc_local_15,bhdr_ms_proc_local_12_zb__ref_0,bhdr_ms_proc_local_12_exp__ref_0);
            itf.s_blk_start_ready <= 1;
          end else begin
            itf.m_bp_valid <= 0;
            itf.s_bc_ready <= 0;
            skpbts_scclang_global_6 <= 1;
          end

          c_wordoff_scclang_global_3 <= w_wordoff_ms_proc_local_10;
          c_rembits_scclang_global_2 <= w_rembits_ms_proc_local_13;
          itf.m_bhdr_data_zb <= bhdr_ms_proc_local_12_zb__ref_0;
          itf.m_bhdr_data_exp <= bhdr_ms_proc_local_12_exp__ref_0;
          itf.m_bhdr_valid <= 1;
        end else begin
          zhw__block_headerfp_t11_52__set_zb_func_4(bhdr_ms_proc_local_12_zb__ref_0,bhdr_ms_proc_local_12_exp__ref_0,1,bhdr_ms_proc_local_12_zb__ref_0,bhdr_ms_proc_local_12_exp__ref_0);
          itf.m_bhdr_valid <= 0;
          itf.m_bp_valid <= 0;
          itf.s_bc_ready <= 0;
          skpbts_scclang_global_6 <= 1;
        end

      end else begin
        if (itf.m_bhdr_ready) begin
          itf.m_bhdr_valid <= 0;
        end
        itf.s_blk_start_ready <= 0;
      end

      if ((((!(csync_scclang_global_4)) && ((!(_s_blk_cycle_ms_proc_local_11)) || (!(bhdr_ms_proc_local_12_zb__ref_0)))) && (!(skpbts_scclang_global_6))) && ((itf.m_bp_ready) && (itf.s_bc_valid))) begin
        if (!(_s_blk_cycle_ms_proc_local_11)) begin
          w_rembits_ms_proc_local_13 = (c_rembits_scclang_global_2) - (itf.s_bc_data);
        end
        if (!(_s_blk_cycle_ms_proc_local_11)) begin
          w_wordoff_ms_proc_local_10 = (w_wordoff_ms_proc_local_10) + (itf.s_bc_data);
        end
        planewdw_ms_proc_local_16 = zhw__get_window_func_3(b_wrk_ms_proc_local_8_f__ref_0,b_wrk_ms_proc_local_8_w__ref_0,w_wordoff_ms_proc_local_10,b_wrk_ms_proc_local_8_f__ref_0,b_wrk_ms_proc_local_8_w__ref_0);
        w_wordoff_ms_proc_local_10 = (w_wordoff_ms_proc_local_10) % (32);
        itf.m_bp_data <= planewdw_ms_proc_local_16;
        itf.m_bp_valid <= 1;
        itf.s_bc_ready <= 1;
        c_wordoff_scclang_global_3 <= w_wordoff_ms_proc_local_10;
        c_rembits_scclang_global_2 <= w_rembits_ms_proc_local_13;
      end else begin
        if ((itf.m_bp_ready) || (itf.s_bc_valid)) begin
          itf.m_bp_valid <= 0;
          itf.s_bc_ready <= 0;
        end
      end

      c_m_bfifo_scclang_global_8_ready <= zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7(b_wrk_ms_proc_local_8_f__ref_0,b_wrk_ms_proc_local_8_w__ref_0,b_wrk_ms_proc_local_8_f__ref_0,b_wrk_ms_proc_local_8_w__ref_0);
    end

    b_c_init_v_ms_proc_local_6_f = b_c_init_v_ms_proc_local_6_f__ref_0;
    bhdr_ms_proc_local_12_exp = bhdr_ms_proc_local_12_exp__ref_0;
    b_wrk_ms_proc_local_8_w = b_wrk_ms_proc_local_8_w__ref_0;
    bhdr_ms_proc_local_12_zb = bhdr_ms_proc_local_12_zb__ref_0;
    b_wrk_ms_proc_local_8_f = b_wrk_ms_proc_local_8_f__ref_0;
    b_c_init_v_ms_proc_local_6_w = b_c_init_v_ms_proc_local_6_w__ref_0;
  end
  function automatic logic [31:0] zhw__get_window_func_3 (input logic [0:0] wb_c_f[0:3], input logic [31:0] wb_c_w[0:3], input logic [6:0] bitoff, inout logic [0:0] wb_c_f__ref_0[0:3], inout logic [31:0] wb_c_w__ref_0[0:3]);
  begin
    logic [5:0] wordoff__local_17;
    logic signed[31:0] i__local_18;
    logic [31:0] b1__local_19;
    logic [31:0] b2__local_20;
    logic [31:0] window__local_21;
    logic [2:0] i__local_22;
    logic [6:0] b1rshift__local_23;
    logic [6:0] b2lshift__local_24;
    bitoff_actual_scclang_global_9 = bitoff;
    wordoff__local_17 = (bitoff_actual_scclang_global_9) / (32);
    for (i__local_18 = 0;(i__local_18) < (4);i__local_18++) begin
      if (!(wb_c_f[i__local_18])) begin
        wordoff__local_17++;
      end
      if (wb_c_f[i__local_18]) begin
        break;
      end
    end
    b1__local_19 = wb_c_w[wordoff__local_17];
    b2__local_20 = wb_c_w[(wordoff__local_17) + (1)];
    for (i__local_22 = 3'd0;(i__local_22) < (4);i__local_22++) begin
      if ((i__local_22) < (wordoff__local_17)) begin
        wb_c_f__ref_0[i__local_22] = 0;
      end
    end
    b1rshift__local_23 = (bitoff_actual_scclang_global_9) % (32);
    b2lshift__local_24 = (32) - (b1rshift__local_23);
    if ((b1rshift__local_23) != (0)) begin
      window__local_21 = ((b1__local_19) >>> (b1rshift__local_23)) | ((b2__local_20) << (b2lshift__local_24));
    end else begin
      window__local_21 = b1__local_19;
    end

    return window__local_21;
  end
  endfunction
  function automatic void zhw__plane_reg2__plane_reg_func_0 (input logic [0:0] hthis_f, input logic [31:0] hthis_w, inout logic [0:0] hthis_f__ref_0, inout logic [31:0] hthis_w__ref_0);
  begin

    hthis_f__ref_0 = 0;
    hthis_w__ref_0 = 0;
  end
  endfunction
  function automatic void zhw__block_headerfp_t11_52__block_header_func_1 (input logic [0:0] hthis_zb, input logic [10:0] hthis_exp, input logic [0:0] _zb, inout logic [0:0] hthis_zb__ref_0, inout logic [10:0] hthis_exp__ref_0);
  begin

    _zb_actual_scclang_global_10 = _zb;
    hthis_zb__ref_0 = _zb_actual_scclang_global_10;
    hthis_exp__ref_0 = 0;
  end
  endfunction
  function automatic void zhw__block_headerfp_t11_52__init_func_2 (input logic [0:0] hthis_zb, input logic [10:0] hthis_exp, input logic [0:0] _zb, inout logic [0:0] hthis_zb__ref_0, inout logic [10:0] hthis_exp__ref_0);
  begin

    _zb_actual_scclang_global_11 = _zb;
    hthis_zb__ref_0 = _zb_actual_scclang_global_11;
    hthis_exp__ref_0 = 0;
  end
  endfunction
  function automatic void zhw__block_headerfp_t11_52__set_exp_func_6 (input logic [0:0] hthis_zb, input logic [10:0] hthis_exp, input logic [10:0] _exp, inout logic [0:0] hthis_zb__ref_0, inout logic [10:0] hthis_exp__ref_0);
  begin

    _exp_actual_scclang_global_12 = _exp;
    hthis_exp__ref_0 = _exp_actual_scclang_global_12;
  end
  endfunction
  function automatic void zhw__block_headerfp_t11_52__set_zb_func_4 (input logic [0:0] hthis_zb, input logic [10:0] hthis_exp, input logic [0:0] _zb, inout logic [0:0] hthis_zb__ref_0, inout logic [10:0] hthis_exp__ref_0);
  begin

    _zb_actual_scclang_global_13 = _zb;
    hthis_zb__ref_0 = _zb_actual_scclang_global_13;
  end
  endfunction
  function automatic logic [0:0] zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7 (input logic [0:0] pb_c_f[0:3], input logic [31:0] pb_c_w[0:3], inout logic [0:0] pb_c_f__ref_0[0:3], inout logic [31:0] pb_c_w__ref_0[0:3]);
  begin
    logic signed[31:0] reg_thresh_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_25;
    logic [63:0] word_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_26_tdata;
    logic [0:0] word_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_26_tlast;
    logic [0:0] w_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_27_f[0:1];
    logic [31:0] w_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_27_w[0:1];
    logic [0:0] empty_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_28_f;
    logic [31:0] empty_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_28_w;
    logic [0:0] tmp_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_29_f[0:3];
    logic [31:0] tmp_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_29_w[0:3];
    logic [0:0] read_data_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_30;
    logic [63:0] i_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_31;
    logic [2:0] srcreg_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_32;
    logic [2:0] tgtreg_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_33;
    logic [63:0] i_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_34;
    logic [63:0] i_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_35;
    logic [63:0] i_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_36;
    logic [31:0] w_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_27_w__ref_0[0:1];
    logic [0:0] empty_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_28_f__ref_0;
    logic [31:0] tmp_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_29_w__ref_0[0:3];
    logic [31:0] empty_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_28_w__ref_0;
    logic [0:0] tmp_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_29_f__ref_0[0:3];
    logic [0:0] w_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_27_f__ref_0[0:1];
    reg_thresh_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_25 = 3;
    word_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_26_tdata = c_m_bfifo_scclang_global_8_data_tdata;
    word_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_26_tlast = c_m_bfifo_scclang_global_8_data_tlast;
    // TODO: fix this function call [Token('ID', 'zhw__plane_reg2__plane_reg_func_0'), 'w_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_27_f', 'w_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_27_w', 'w_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_27_f__ref_0', 'w_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_27_w__ref_0'];
    // TODO: fix this function call [Token('ID', 'zhw__plane_reg2__plane_reg_func_0'), 'empty_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_28_f', 'empty_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_28_w', 'empty_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_28_f__ref_0', 'empty_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_28_w__ref_0'];
    // TODO: fix this function call [Token('ID', 'zhw__plane_reg2__plane_reg_func_0'), 'tmp_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_29_f', 'tmp_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_29_w', 'tmp_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_29_f__ref_0', 'tmp_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_29_w__ref_0'];
    read_data_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_30 = 0;
    if (c_m_bfifo_scclang_global_8_valid) begin
      if (c_m_bfifo_scclang_global_8_ready) begin
        for (i_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_31 = 0;(i_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_31) < ((64) / (32));i_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_31++) begin
          w_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_27_f[i_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_31] = 1;
          w_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_27_w[i_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_31] = (word_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_26_tdata) >>> ((32) * (i_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_31));
        end
      end
      for (srcreg_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_32 = 0;(srcreg_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_32) < (4);srcreg_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_32++) begin
        if (pb_c_f[srcreg_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_32]) begin
          break;
        end
      end
      for (tgtreg_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_33 = 0;(tgtreg_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_33) < (4);tgtreg_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_33++) begin
        if ((srcreg_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_32) >= (4)) begin
          break;
        end
        if (!(pb_c_f[srcreg_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_32])) begin
          break;
        end else begin
          tmp_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_29_f[tgtreg_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_33] = pb_c_f[srcreg_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_32];
          tmp_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_29_w[tgtreg_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_33] = pb_c_w[srcreg_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_32];
          srcreg_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_32++;
        end

      end
      if ((tgtreg_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_33) < (3)) begin
        for (i_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_34 = 0;(i_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_34) < ((64) / (32));i_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_34++) begin
          tmp_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_29_f[tgtreg_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_33] = w_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_27_f[i_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_34];
          tmp_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_29_w[tgtreg_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_33] = w_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_27_w[i_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_34];
          tgtreg_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_33++;
        end
      end
    end else begin
      for (i_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_35 = 0;(i_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_35) < (4);i_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_35++) begin
        tmp_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_29_f[i_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_35] = pb_c_f[i_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_35];
        tmp_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_29_w[i_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_35] = pb_c_w[i_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_35];
      end
    end

    for (i_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_36 = 0;(i_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_36) < (4);i_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_36++) begin
      b_c_scclang_global_0_f[i_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_36] <= tmp_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_29_f[i_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_36];
      b_c_scclang_global_0_w[i_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_36] <= tmp_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_29_w[i_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_36];
    end
    if (!(tmp_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_29_f[(3) - (1)])) begin
      read_data_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_30 = 1;
    end else begin
      read_data_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_30 = 0;
    end

    return read_data_zhw__decode_streamfp_t11_52_bits_t64_2__refresh_next_bs_regs_func_7_local_30;
  end
  endfunction
  function automatic logic [15:0] zhw__decode_streamfp_t11_52_bits_t64_2__get_block_maxprec_func_5 (input logic [10:0] maxexp);
  begin
    logic signed[15:0] _MAX__local_37;
    maxexp_actual_scclang_global_14 = maxexp;
    _MAX__local_37 = (((maxexp_actual_scclang_global_14) - (itf.s_minexp)) + ((2) * (2))) + (2);
    if ((_MAX__local_37) < (0)) begin
      _MAX__local_37 = 0;
    end
    if ((_MAX__local_37) < (itf.s_maxprec)) begin
      return _MAX__local_37;
    end else begin
      return itf.s_maxprec;
    end

  end
  endfunction
  genvar i_u_decode_stream_local_1;
  for (i_u_decode_stream_local_1 = 0;(i_u_decode_stream_local_1) < (4);i_u_decode_stream_local_1++) begin

  end
endmodule
module rvfifo_cc_sc_module_9 (
 rvfifo_cc_sc_module_9_interface.port0 itf
);
  logic signed[31:0] MAX_DEPTH_scclang_global_0 = 4;
  initial begin

  end
  fifo_cc_sc_module_10_interface u_fifo_itf();

  fifo_cc_sc_module_10 u_fifo(
    u_fifo_itf
  );
    assign u_fifo_itf.clk = itf.clk;
    assign u_fifo_itf.reset = itf.reset;
    assign u_fifo_itf.din_tdata = itf.s_port_data_tdata;
    assign u_fifo_itf.din_tlast = itf.s_port_data_tlast;
    assign u_fifo_itf.wr_en = itf.s_port_valid;
    assign itf.s_port_ready = u_fifo_itf.full;
    assign itf.m_port_data_tdata = u_fifo_itf.dout_tdata;
    assign itf.m_port_data_tlast = u_fifo_itf.dout_tlast;
    assign u_fifo_itf.rd_en = itf.m_port_ready;
    assign itf.m_port_valid = u_fifo_itf.empty;
  always @(*) begin
  end

endmodule
module fifo_cc_sc_module_10 (
 fifo_cc_sc_module_10_interface.port0 itf
);
  logic [63:0] data_scclang_global_0_tdata[0:3];
  logic [0:0] data_scclang_global_0_tlast[0:3];
  logic [0:0] empty_i_scclang_global_1;
  logic [0:0] full_i_scclang_global_2;
  logic [0:0] rd_en_i_scclang_global_3;
  logic [1:0] rd_idx_scclang_global_4;
  logic [0:0] wr_en_i_scclang_global_5;
  logic [1:0] wr_idx_scclang_global_6;
  logic [31:0] depth_scclang_global_7;
  logic signed[31:0] MAX_DEPTH_scclang_global_8 = 4;
  initial begin

  end
  always @(itf.wr_en or itf.rd_en or full_i_scclang_global_2 or empty_i_scclang_global_1 or rd_idx_scclang_global_4) begin: mc_proc
    
    
    if (1) begin
      itf.dout_tdata <= data_scclang_global_0_tdata[rd_idx_scclang_global_4];
      itf.dout_tlast <= data_scclang_global_0_tlast[rd_idx_scclang_global_4];
    end
    wr_en_i_scclang_global_5 <= (itf.wr_en) && (!(full_i_scclang_global_2));
    rd_en_i_scclang_global_3 <= (itf.rd_en) && (!(empty_i_scclang_global_1));
    itf.full <= (full_i_scclang_global_2) == (0);
    itf.empty <= (empty_i_scclang_global_1) == (0);
  end
  always_ff @(posedge itf.clk) begin: ms_proc
    logic [1:0] wr_inc_ms_proc_local_3;
    logic [1:0] rd_inc_ms_proc_local_4;
    logic [31:0] i_ms_proc_local_5;
    
    wr_inc_ms_proc_local_3 = ((wr_idx_scclang_global_6) + (1)) % (4);
    rd_inc_ms_proc_local_4 = ((rd_idx_scclang_global_4) + (1)) % (4);
    if ((itf.reset) == (0)) begin
      if (!(1)) begin
        itf.dout_tdata <= 0;
        itf.dout_tlast <= 0;
      end
      for (i_ms_proc_local_5 = 0;(i_ms_proc_local_5) < (4);i_ms_proc_local_5++) begin
        data_scclang_global_0_tdata[i_ms_proc_local_5] <= 0;
        data_scclang_global_0_tlast[i_ms_proc_local_5] <= 0;
      end
      rd_idx_scclang_global_4 <= 2'd0;
      wr_idx_scclang_global_6 <= 2'd0;
      full_i_scclang_global_2 <= 0;
      empty_i_scclang_global_1 <= 1;
    end else begin
      if (!(1)) begin
        if (rd_en_i_scclang_global_3) begin
          itf.dout_tdata <= data_scclang_global_0_tdata[rd_idx_scclang_global_4];
          itf.dout_tlast <= data_scclang_global_0_tlast[rd_idx_scclang_global_4];
        end
      end
      if (wr_en_i_scclang_global_5) begin
        data_scclang_global_0_tdata[wr_idx_scclang_global_6] <= itf.din_tdata;
        data_scclang_global_0_tlast[wr_idx_scclang_global_6] <= itf.din_tlast;
        wr_idx_scclang_global_6 <= wr_inc_ms_proc_local_3;
        if (!(rd_en_i_scclang_global_3)) begin
          if ((wr_inc_ms_proc_local_3) == (rd_idx_scclang_global_4)) begin
            full_i_scclang_global_2 <= 1;
          end
          empty_i_scclang_global_1 <= 0;
        end
      end
      if (rd_en_i_scclang_global_3) begin
        rd_idx_scclang_global_4 <= rd_inc_ms_proc_local_4;
        if (!(wr_en_i_scclang_global_5)) begin
          full_i_scclang_global_2 <= 0;
          if ((rd_inc_ms_proc_local_4) == (wr_idx_scclang_global_6)) begin
            empty_i_scclang_global_1 <= 1;
          end
        end
      end
    end

  end
genvar i_u_fifo_local_1;
  /*generate*/ for (i_u_fifo_local_1 = 0;(i_u_fifo_local_1) < (4);i_u_fifo_local_1++) begin
    always @(data_scclang_global_0_tdata[(i_u_fifo_local_1)]) begin: mc_proc
      
      
      if (1) begin
        itf.dout_tdata <= data_scclang_global_0_tdata[rd_idx_scclang_global_4];
        itf.dout_tlast <= data_scclang_global_0_tlast[rd_idx_scclang_global_4];
      end
      wr_en_i_scclang_global_5 <= (itf.wr_en) && (!(full_i_scclang_global_2));
      rd_en_i_scclang_global_3 <= (itf.rd_en) && (!(empty_i_scclang_global_1));
      itf.full <= (full_i_scclang_global_2) == (0);
      itf.empty <= (empty_i_scclang_global_1) == (0);
    end
  end
  /*generate*/ for (i_u_fifo_local_1 = 0;(i_u_fifo_local_1) < (4);i_u_fifo_local_1++) begin
    always @(data_scclang_global_0_tlast[(i_u_fifo_local_1)]) begin: mc_proc
      
      
      if (1) begin
        itf.dout_tdata <= data_scclang_global_0_tdata[rd_idx_scclang_global_4];
        itf.dout_tlast <= data_scclang_global_0_tlast[rd_idx_scclang_global_4];
      end
      wr_en_i_scclang_global_5 <= (itf.wr_en) && (!(full_i_scclang_global_2));
      rd_en_i_scclang_global_3 <= (itf.rd_en) && (!(empty_i_scclang_global_1));
      itf.full <= (full_i_scclang_global_2) == (0);
      itf.empty <= (empty_i_scclang_global_1) == (0);
    end
  end
endmodule
module rvfifo_cc_sc_module_3 (
 rvfifo_cc_sc_module_3_interface.port0 itf
);
  logic signed[31:0] MAX_DEPTH_scclang_global_0 = 4;
  initial begin

  end
  fifo_cc_sc_module_11_interface u_fifo_itf();

  fifo_cc_sc_module_11 u_fifo(
    u_fifo_itf
  );
    assign u_fifo_itf.clk = itf.clk;
    assign u_fifo_itf.reset = itf.reset;
    assign u_fifo_itf.din_zb = itf.s_port_data_zb;
    assign u_fifo_itf.din_exp = itf.s_port_data_exp;
    assign u_fifo_itf.wr_en = itf.s_port_valid;
    assign itf.s_port_ready = u_fifo_itf.full;
    assign itf.m_port_data_zb = u_fifo_itf.dout_zb;
    assign itf.m_port_data_exp = u_fifo_itf.dout_exp;
    assign u_fifo_itf.rd_en = itf.m_port_ready;
    assign itf.m_port_valid = u_fifo_itf.empty;
  always @(*) begin
  end

endmodule
module fifo_cc_sc_module_11 (
 fifo_cc_sc_module_11_interface.port0 itf
);
  logic [0:0] data_scclang_global_0_zb[0:3];
  logic [10:0] data_scclang_global_0_exp[0:3];
  logic [0:0] empty_i_scclang_global_1;
  logic [0:0] full_i_scclang_global_2;
  logic [0:0] rd_en_i_scclang_global_3;
  logic [1:0] rd_idx_scclang_global_4;
  logic [0:0] wr_en_i_scclang_global_5;
  logic [1:0] wr_idx_scclang_global_6;
  logic [31:0] depth_scclang_global_7;
  logic signed[31:0] MAX_DEPTH_scclang_global_8 = 4;
  initial begin

  end
  always @(itf.wr_en or itf.rd_en or full_i_scclang_global_2 or empty_i_scclang_global_1 or rd_idx_scclang_global_4) begin: mc_proc
    
    
    if (1) begin
      itf.dout_zb <= data_scclang_global_0_zb[rd_idx_scclang_global_4];
      itf.dout_exp <= data_scclang_global_0_exp[rd_idx_scclang_global_4];
    end
    wr_en_i_scclang_global_5 <= (itf.wr_en) && (!(full_i_scclang_global_2));
    rd_en_i_scclang_global_3 <= (itf.rd_en) && (!(empty_i_scclang_global_1));
    itf.full <= (full_i_scclang_global_2) == (0);
    itf.empty <= (empty_i_scclang_global_1) == (0);
  end
  always_ff @(posedge itf.clk) begin: ms_proc
    logic [1:0] wr_inc_ms_proc_local_3;
    logic [1:0] rd_inc_ms_proc_local_4;
    logic [31:0] i_ms_proc_local_5;
    
    wr_inc_ms_proc_local_3 = ((wr_idx_scclang_global_6) + (1)) % (4);
    rd_inc_ms_proc_local_4 = ((rd_idx_scclang_global_4) + (1)) % (4);
    if ((itf.reset) == (0)) begin
      if (!(1)) begin
        itf.dout_zb <= 0;
        itf.dout_exp <= 0;
      end
      for (i_ms_proc_local_5 = 0;(i_ms_proc_local_5) < (4);i_ms_proc_local_5++) begin
        data_scclang_global_0_zb[i_ms_proc_local_5] <= 0;
        data_scclang_global_0_exp[i_ms_proc_local_5] <= 0;
      end
      rd_idx_scclang_global_4 <= 2'd0;
      wr_idx_scclang_global_6 <= 2'd0;
      full_i_scclang_global_2 <= 0;
      empty_i_scclang_global_1 <= 1;
    end else begin
      if (!(1)) begin
        if (rd_en_i_scclang_global_3) begin
          itf.dout_zb <= data_scclang_global_0_zb[rd_idx_scclang_global_4];
          itf.dout_exp <= data_scclang_global_0_exp[rd_idx_scclang_global_4];
        end
      end
      if (wr_en_i_scclang_global_5) begin
        data_scclang_global_0_zb[wr_idx_scclang_global_6] <= itf.din_zb;
        data_scclang_global_0_exp[wr_idx_scclang_global_6] <= itf.din_exp;
        wr_idx_scclang_global_6 <= wr_inc_ms_proc_local_3;
        if (!(rd_en_i_scclang_global_3)) begin
          if ((wr_inc_ms_proc_local_3) == (rd_idx_scclang_global_4)) begin
            full_i_scclang_global_2 <= 1;
          end
          empty_i_scclang_global_1 <= 0;
        end
      end
      if (rd_en_i_scclang_global_3) begin
        rd_idx_scclang_global_4 <= rd_inc_ms_proc_local_4;
        if (!(wr_en_i_scclang_global_5)) begin
          full_i_scclang_global_2 <= 0;
          if ((rd_inc_ms_proc_local_4) == (wr_idx_scclang_global_6)) begin
            empty_i_scclang_global_1 <= 1;
          end
        end
      end
    end

  end
genvar i_u_fifo_local_1;
  /*generate*/ for (i_u_fifo_local_1 = 0;(i_u_fifo_local_1) < (4);i_u_fifo_local_1++) begin
    always @(data_scclang_global_0_zb[(i_u_fifo_local_1)]) begin: mc_proc
      
      
      if (1) begin
        itf.dout_zb <= data_scclang_global_0_zb[rd_idx_scclang_global_4];
        itf.dout_exp <= data_scclang_global_0_exp[rd_idx_scclang_global_4];
      end
      wr_en_i_scclang_global_5 <= (itf.wr_en) && (!(full_i_scclang_global_2));
      rd_en_i_scclang_global_3 <= (itf.rd_en) && (!(empty_i_scclang_global_1));
      itf.full <= (full_i_scclang_global_2) == (0);
      itf.empty <= (empty_i_scclang_global_1) == (0);
    end
  end
  /*generate*/ for (i_u_fifo_local_1 = 0;(i_u_fifo_local_1) < (4);i_u_fifo_local_1++) begin
    always @(data_scclang_global_0_exp[(i_u_fifo_local_1)]) begin: mc_proc
      
      
      if (1) begin
        itf.dout_zb <= data_scclang_global_0_zb[rd_idx_scclang_global_4];
        itf.dout_exp <= data_scclang_global_0_exp[rd_idx_scclang_global_4];
      end
      wr_en_i_scclang_global_5 <= (itf.wr_en) && (!(full_i_scclang_global_2));
      rd_en_i_scclang_global_3 <= (itf.rd_en) && (!(empty_i_scclang_global_1));
      itf.full <= (full_i_scclang_global_2) == (0);
      itf.empty <= (empty_i_scclang_global_1) == (0);
    end
  end
endmodule
module decode_ints_sc_module_4 (
 decode_ints_sc_module_4_interface.port0 itf
);
  logic [15:0] c_bplane_scclang_global_0[0:63];
  logic [5:0] k_scclang_global_1;
  logic [15:0] kmin_scclang_global_2;
  logic [3:0] s_state_scclang_global_3;
  logic [4:0] n_scclang_global_4;
  logic [3:0] state_scclang_global_5;
  logic [63:0] x_scclang_global_6;
  logic [31:0] stream_window_scclang_global_7;
  logic [31:0] bits_scclang_global_8;
  logic [31:0] bitoff_scclang_global_9;
  logic signed[31:0] planes_scclang_global_10 = 64;
  logic signed[31:0] intprec_scclang_global_11 = 64;
  initial begin

  end
  always @(itf.s_maxprec) begin: mc_proc
    logic [63:0] tmp_mc_proc_local_3;
    logic signed[31:0] j_mc_proc_local_4;
    logic signed[31:0] i_mc_proc_local_5;
    
    for (j_mc_proc_local_4 = 0;(j_mc_proc_local_4) < (16);j_mc_proc_local_4++) begin
      for (i_mc_proc_local_5 = 0;(i_mc_proc_local_5) < (64);i_mc_proc_local_5++) begin
        tmp_mc_proc_local_3[i_mc_proc_local_5] = c_bplane_scclang_global_0[i_mc_proc_local_5][j_mc_proc_local_4];
      end
      itf.m_block[j_mc_proc_local_4] <= tmp_mc_proc_local_3;
    end
    if ((64) > (itf.s_maxprec)) begin
      kmin_scclang_global_2 <= (64) - (itf.s_maxprec);
    end else begin
      kmin_scclang_global_2 <= 0;
    end

  end
  always_ff @(posedge itf.clk) begin: ms_proc
    logic [0:0] m_bc_valid_ms_proc_local_6;
    logic [0:0] s_bp_ready_ms_proc_local_7;
    logic [31:0] m_ms_proc_local_8;
    logic signed[31:0] i_ms_proc_local_9;
    logic [63:0] i_ms_proc_local_10;
    logic [0:0] last_ms_proc_local_11;
    
    if ((itf.reset) == (0)) begin
      state_scclang_global_5 = 0;
      k_scclang_global_1 <= (64) - (1);
      n_scclang_global_4 = 0;
      bits_scclang_global_8 = (itf.s_maxbits) - ((11) + (1));
      bitoff_scclang_global_9 = 0;
      itf.m_blk_start_data <= 1;
      itf.m_blk_start_valid <= 1;
      itf.s_bp_ready <= 0;
      itf.m_bc_valid <= 0;
      itf.m_bc_data <= 0;
    end else begin
      m_bc_valid_ms_proc_local_6 = (itf.m_ready) && ((!(itf.m_bc_ready)) || (itf.s_bp_valid));
      s_bp_ready_ms_proc_local_7 = (itf.m_ready) && ((!(itf.s_bp_valid)) || (itf.m_bc_ready));
      itf.s_bp_ready <= s_bp_ready_ms_proc_local_7;
      itf.m_bc_valid <= m_bc_valid_ms_proc_local_6;
      if ((itf.m_blk_start_ready) == (1)) begin
        itf.m_blk_start_data <= 0;
        itf.m_blk_start_valid <= 0;
      end
      if ((((itf.m_ready) || ((k_scclang_global_1) != (kmin_scclang_global_2))) && (itf.m_bc_ready)) && (itf.s_bp_valid)) begin
        stream_window_scclang_global_7 = itf.s_bp_data;
        if ((n_scclang_global_4) < (bits_scclang_global_8)) begin
          m_ms_proc_local_8 = n_scclang_global_4;
        end else begin
          m_ms_proc_local_8 = bits_scclang_global_8;
        end

        bits_scclang_global_8 = (bits_scclang_global_8) - (m_ms_proc_local_8);
        if ((m_ms_proc_local_8) > (0)) begin
          x_scclang_global_6 = (stream_window_scclang_global_7) & ((~(64'd0)) >>> ((64) - (m_ms_proc_local_8)));
        end else begin
          x_scclang_global_6 = 0;
        end

        bitoff_scclang_global_9 = m_ms_proc_local_8;
        for (i_ms_proc_local_9 = 0;(i_ms_proc_local_9) < ((2) * (16));i_ms_proc_local_9++) begin
          if (!((n_scclang_global_4) < (16))) begin
            break;
          end
          if ((state_scclang_global_5) & (1)) begin
            if ((state_scclang_global_5) & (2)) begin
              state_scclang_global_5 = (0) + (1);
              if ((n_scclang_global_4) < (16)) begin
                bits_scclang_global_8--;
              end
            end
            if (((n_scclang_global_4) < ((16) - (1))) && (!((state_scclang_global_5) & (4)))) begin
              if (!(bits_scclang_global_8)) begin
                x_scclang_global_6[n_scclang_global_4] = 1;
                state_scclang_global_5 = (0) + (4);
              end else begin
                bits_scclang_global_8--;
                if (stream_window_scclang_global_7[bitoff_scclang_global_9++]) begin
                  x_scclang_global_6[n_scclang_global_4] = 1;
                  state_scclang_global_5 = (0) + (4);
                end
              end

              n_scclang_global_4++;
            end
            if (((n_scclang_global_4) == ((16) - (1))) || ((state_scclang_global_5) & (4))) begin
              if (((n_scclang_global_4) >= ((16) - (1))) && (!((state_scclang_global_5) & (4)))) begin
                x_scclang_global_6[n_scclang_global_4] = 1;
                n_scclang_global_4++;
              end
              state_scclang_global_5 = (0) + (4);
            end
          end
          if (!((state_scclang_global_5) & (1))) begin
            if ((!(bits_scclang_global_8)) || ((n_scclang_global_4) >= (16))) begin
              break;
            end
            if (stream_window_scclang_global_7[bitoff_scclang_global_9++]) begin
              state_scclang_global_5 = ((0) + (1)) + (2);
            end else begin
              break;
            end

          end
        end
        if (((n_scclang_global_4) < (16)) && (bits_scclang_global_8)) begin
          bits_scclang_global_8--;
        end
        state_scclang_global_5 = 0;
        if ((k_scclang_global_1) == ((64) - (1))) begin
          for (i_ms_proc_local_10 = 0;(i_ms_proc_local_10) < ((64) - (1));i_ms_proc_local_10++) begin
            c_bplane_scclang_global_0[i_ms_proc_local_10] <= 16'd0;
          end
        end
        c_bplane_scclang_global_0[k_scclang_global_1] <= x_scclang_global_6;
        itf.m_bc_data <= bitoff_scclang_global_9;
        last_ms_proc_local_11 = ((bits_scclang_global_8) == (0)) || ((k_scclang_global_1) <= (kmin_scclang_global_2));
        if (last_ms_proc_local_11) begin
          k_scclang_global_1 <= (64) - (1);
          n_scclang_global_4 = 0;
          bits_scclang_global_8 = (itf.s_maxbits) - ((11) + (1));
        end else begin
          k_scclang_global_1 <= (k_scclang_global_1) - (1);
        end

        itf.m_valid <= last_ms_proc_local_11;
        itf.m_blk_start_valid <= last_ms_proc_local_11;
        itf.m_blk_start_data <= last_ms_proc_local_11;
      end
      if ((itf.m_valid) && (itf.m_ready)) begin
        itf.m_valid <= 0;
      end
    end

    s_state_scclang_global_3 <= state_scclang_global_5;
  end
genvar i_u_decode_ints_local_1;
  /*generate*/ for (i_u_decode_ints_local_1 = 0;(i_u_decode_ints_local_1) < (64);i_u_decode_ints_local_1++) begin
    always @(c_bplane_scclang_global_0[(i_u_decode_ints_local_1)]) begin: mc_proc
      logic [63:0] tmp_mc_proc_local_3;
      logic signed[31:0] j_mc_proc_local_4;
      logic signed[31:0] i_mc_proc_local_5;
      
      /*generate*/ for (j_mc_proc_local_4 = 0;(j_mc_proc_local_4) < (16);j_mc_proc_local_4++) begin
        /*generate*/ for (i_mc_proc_local_5 = 0;(i_mc_proc_local_5) < (64);i_mc_proc_local_5++) begin
          tmp_mc_proc_local_3[i_mc_proc_local_5] = c_bplane_scclang_global_0[i_mc_proc_local_5][j_mc_proc_local_4];
        end
        itf.m_block[j_mc_proc_local_4] <= tmp_mc_proc_local_3;
      end
      if ((64) > (itf.s_maxprec)) begin
        kmin_scclang_global_2 <= (64) - (itf.s_maxprec);
      end else begin
        kmin_scclang_global_2 <= 0;
      end

    end
  end
endmodule
module block_buffer_sc_module_5 (
 block_buffer_sc_module_5_interface.port0 itf
);
  logic [1023:0] c_bi_scclang_global_0;
  logic [1023:0] c_bo_scclang_global_1;
  initial begin

  end
  fifo_cc_sc_module_12_interface u_bbuf_itf();

  fifo_cc_sc_module_12 u_bbuf(
    u_bbuf_itf
  );
    assign u_bbuf_itf.clk = itf.clk;
    assign u_bbuf_itf.reset = itf.reset;
    assign u_bbuf_itf.din = c_bi_scclang_global_0;
    assign u_bbuf_itf.wr_en = itf.s_valid;
    assign itf.s_ready = u_bbuf_itf.full;
    assign c_bo_scclang_global_1 = u_bbuf_itf.dout;
    assign u_bbuf_itf.rd_en = itf.m_ready;
    assign itf.m_valid = u_bbuf_itf.empty;
  always @(*) begin
  end

  always @(c_bo_scclang_global_1) begin: mc_proc
    logic [1023:0] bbufi_mc_proc_local_2;
    logic [1023:0] bbufo_mc_proc_local_3;
    logic signed[31:0] i_mc_proc_local_4;
    logic signed[31:0] i_mc_proc_local_5;
    
    for (i_mc_proc_local_4 = 0;(i_mc_proc_local_4) < (16);i_mc_proc_local_4++) begin
      bbufi_mc_proc_local_2[((i_mc_proc_local_4) * (64)) +: (64)] = itf.s_block[i_mc_proc_local_4];
    end
    c_bi_scclang_global_0 <= bbufi_mc_proc_local_2;
    bbufo_mc_proc_local_3 = c_bo_scclang_global_1;
    for (i_mc_proc_local_5 = 0;(i_mc_proc_local_5) < (16);i_mc_proc_local_5++) begin
      itf.m_block[i_mc_proc_local_5] <= bbufo_mc_proc_local_3[((i_mc_proc_local_5) * (64)) +: (64)];
    end
  end
genvar i_u_block_buffer_local_1;
  /*generate*/ for (i_u_block_buffer_local_1 = 0;(i_u_block_buffer_local_1) < (16);i_u_block_buffer_local_1++) begin
    always @(itf.s_block[(i_u_block_buffer_local_1)]) begin: mc_proc
      logic [1023:0] bbufi_mc_proc_local_2;
      logic [1023:0] bbufo_mc_proc_local_3;
      logic signed[31:0] i_mc_proc_local_4;
      logic signed[31:0] i_mc_proc_local_5;
      
      /*generate*/ for (i_mc_proc_local_4 = 0;(i_mc_proc_local_4) < (16);i_mc_proc_local_4++) begin
        bbufi_mc_proc_local_2[((i_mc_proc_local_4) * (64)) +: (64)] = itf.s_block[i_mc_proc_local_4];
      end
      c_bi_scclang_global_0 <= bbufi_mc_proc_local_2;
      bbufo_mc_proc_local_3 = c_bo_scclang_global_1;
      /*generate*/ for (i_mc_proc_local_5 = 0;(i_mc_proc_local_5) < (16);i_mc_proc_local_5++) begin
        itf.m_block[i_mc_proc_local_5] <= bbufo_mc_proc_local_3[((i_mc_proc_local_5) * (64)) +: (64)];
      end
    end
  end
endmodule
module fifo_cc_sc_module_12 (
 fifo_cc_sc_module_12_interface.port0 itf
);
  logic [1023:0] data_scclang_global_0[0:1];
  logic [0:0] empty_i_scclang_global_1;
  logic [0:0] full_i_scclang_global_2;
  logic [0:0] rd_en_i_scclang_global_3;
  logic [0:0] rd_idx_scclang_global_4;
  logic [0:0] wr_en_i_scclang_global_5;
  logic [0:0] wr_idx_scclang_global_6;
  logic [31:0] depth_scclang_global_7;
  logic signed[31:0] MAX_DEPTH_scclang_global_8 = 2;
  initial begin

  end
  always @(itf.wr_en or itf.rd_en or full_i_scclang_global_2 or empty_i_scclang_global_1 or rd_idx_scclang_global_4) begin: mc_proc
    
    
    if (1) begin
      itf.dout <= data_scclang_global_0[rd_idx_scclang_global_4];
    end
    wr_en_i_scclang_global_5 <= (itf.wr_en) && (!(full_i_scclang_global_2));
    rd_en_i_scclang_global_3 <= (itf.rd_en) && (!(empty_i_scclang_global_1));
    itf.full <= (full_i_scclang_global_2) == (0);
    itf.empty <= (empty_i_scclang_global_1) == (0);
  end
  always_ff @(posedge itf.clk) begin: ms_proc
    logic [0:0] wr_inc_ms_proc_local_3;
    logic [0:0] rd_inc_ms_proc_local_4;
    logic [31:0] i_ms_proc_local_5;
    
    wr_inc_ms_proc_local_3 = ((wr_idx_scclang_global_6) + (1)) % (2);
    rd_inc_ms_proc_local_4 = ((rd_idx_scclang_global_4) + (1)) % (2);
    if ((itf.reset) == (0)) begin
      if (!(1)) begin
        itf.dout <= 0;
      end
      for (i_ms_proc_local_5 = 0;(i_ms_proc_local_5) < (2);i_ms_proc_local_5++) begin
        data_scclang_global_0[i_ms_proc_local_5] <= 0;
      end
      rd_idx_scclang_global_4 <= 1'd0;
      wr_idx_scclang_global_6 <= 1'd0;
      full_i_scclang_global_2 <= 0;
      empty_i_scclang_global_1 <= 1;
    end else begin
      if (!(1)) begin
        if (rd_en_i_scclang_global_3) begin
          itf.dout <= data_scclang_global_0[rd_idx_scclang_global_4];
        end
      end
      if (wr_en_i_scclang_global_5) begin
        data_scclang_global_0[wr_idx_scclang_global_6] <= itf.din;
        wr_idx_scclang_global_6 <= wr_inc_ms_proc_local_3;
        if (!(rd_en_i_scclang_global_3)) begin
          if ((wr_inc_ms_proc_local_3) == (rd_idx_scclang_global_4)) begin
            full_i_scclang_global_2 <= 1;
          end
          empty_i_scclang_global_1 <= 0;
        end
      end
      if (rd_en_i_scclang_global_3) begin
        rd_idx_scclang_global_4 <= rd_inc_ms_proc_local_4;
        if (!(wr_en_i_scclang_global_5)) begin
          full_i_scclang_global_2 <= 0;
          if ((rd_inc_ms_proc_local_4) == (wr_idx_scclang_global_6)) begin
            empty_i_scclang_global_1 <= 1;
          end
        end
      end
    end

  end
genvar i_u_bbuf_local_1;
  /*generate*/ for (i_u_bbuf_local_1 = 0;(i_u_bbuf_local_1) < (2);i_u_bbuf_local_1++) begin
    always @(data_scclang_global_0[(i_u_bbuf_local_1)]) begin: mc_proc
      
      
      if (1) begin
        itf.dout <= data_scclang_global_0[rd_idx_scclang_global_4];
      end
      wr_en_i_scclang_global_5 <= (itf.wr_en) && (!(full_i_scclang_global_2));
      rd_en_i_scclang_global_3 <= (itf.rd_en) && (!(empty_i_scclang_global_1));
      itf.full <= (full_i_scclang_global_2) == (0);
      itf.empty <= (empty_i_scclang_global_1) == (0);
    end
  end
endmodule
module decode_block_sc_module_6 (
 decode_block_sc_module_6_interface.port0 itf
);
  logic signed[63:0] c_xt_data_scclang_global_0[0:3][0:3];
  logic [0:0] c_xt_ready_scclang_global_1[0:3];
  logic [0:0] c_xt_valid_scclang_global_2[0:3];
  logic signed[63:0] c_yt_data_scclang_global_3[0:3][0:3];
  logic [0:0] c_yt_ready_scclang_global_4[0:3];
  logic [0:0] c_yt_valid_scclang_global_5[0:3];
  logic [0:0] xors_valid_scclang_global_6;
  initial begin

  end
  genvar decode_block_sc_module_6_u_xt_0;
  /*generate*/ for (decode_block_sc_module_6_u_xt_0 = 0; decode_block_sc_module_6_u_xt_0 < 4; decode_block_sc_module_6_u_xt_0 = decode_block_sc_module_6_u_xt_0 + 1) begin : u_xt
    inv_lift_sc_module_13_interface itf();
    inv_lift_sc_module_13 mod(
      itf
    );
  end

  genvar decode_block_sc_module_6_u_yt_0;
  /*generate*/ for (decode_block_sc_module_6_u_yt_0 = 0; decode_block_sc_module_6_u_yt_0 < 4; decode_block_sc_module_6_u_yt_0 = decode_block_sc_module_6_u_yt_0 + 1) begin : u_yt
    inv_lift_sc_module_14_interface itf();
    inv_lift_sc_module_14 mod(
      itf
    );
  end

  always @(itf.m_ready or c_xt_valid_scclang_global_2[(0)]) begin: mc_proc
    
    
    itf.s_ready <= itf.m_ready;
    itf.m_valid <= c_xt_valid_scclang_global_2[0];
  end
  always_ff @(posedge itf.clk) begin: ms_rev_order
    
    
    if ((itf.reset) == (0)) begin
      xors_valid_scclang_global_6 <= 0;
    end else begin
      if (itf.m_ready) begin
        c_yt_data_scclang_global_3[0][0] <= ((itf.s_block[0]) ^ (-64'd6148914691236517206)) - (-64'd6148914691236517206);
        c_yt_data_scclang_global_3[0][1] <= ((itf.s_block[1]) ^ (-64'd6148914691236517206)) - (-64'd6148914691236517206);
        c_yt_data_scclang_global_3[1][0] <= ((itf.s_block[2]) ^ (-64'd6148914691236517206)) - (-64'd6148914691236517206);
        c_yt_data_scclang_global_3[1][1] <= ((itf.s_block[3]) ^ (-64'd6148914691236517206)) - (-64'd6148914691236517206);
        c_yt_data_scclang_global_3[0][2] <= ((itf.s_block[4]) ^ (-64'd6148914691236517206)) - (-64'd6148914691236517206);
        c_yt_data_scclang_global_3[2][0] <= ((itf.s_block[5]) ^ (-64'd6148914691236517206)) - (-64'd6148914691236517206);
        c_yt_data_scclang_global_3[1][2] <= ((itf.s_block[6]) ^ (-64'd6148914691236517206)) - (-64'd6148914691236517206);
        c_yt_data_scclang_global_3[2][1] <= ((itf.s_block[7]) ^ (-64'd6148914691236517206)) - (-64'd6148914691236517206);
        c_yt_data_scclang_global_3[0][3] <= ((itf.s_block[8]) ^ (-64'd6148914691236517206)) - (-64'd6148914691236517206);
        c_yt_data_scclang_global_3[3][0] <= ((itf.s_block[9]) ^ (-64'd6148914691236517206)) - (-64'd6148914691236517206);
        c_yt_data_scclang_global_3[2][2] <= ((itf.s_block[10]) ^ (-64'd6148914691236517206)) - (-64'd6148914691236517206);
        c_yt_data_scclang_global_3[1][3] <= ((itf.s_block[11]) ^ (-64'd6148914691236517206)) - (-64'd6148914691236517206);
        c_yt_data_scclang_global_3[3][1] <= ((itf.s_block[12]) ^ (-64'd6148914691236517206)) - (-64'd6148914691236517206);
        c_yt_data_scclang_global_3[2][3] <= ((itf.s_block[13]) ^ (-64'd6148914691236517206)) - (-64'd6148914691236517206);
        c_yt_data_scclang_global_3[3][2] <= ((itf.s_block[14]) ^ (-64'd6148914691236517206)) - (-64'd6148914691236517206);
        c_yt_data_scclang_global_3[3][3] <= ((itf.s_block[15]) ^ (-64'd6148914691236517206)) - (-64'd6148914691236517206);
        xors_valid_scclang_global_6 <= itf.s_valid;
      end
    end

  end
  genvar i_u_decode_block_local_1;
  genvar j_u_decode_block_local_0;
  for (j_u_decode_block_local_0 = 0;(j_u_decode_block_local_0) < (4);j_u_decode_block_local_0++) begin
        assign u_xt[j_u_decode_block_local_0].itf.clk = itf.clk;
        assign u_xt[j_u_decode_block_local_0].itf.reset = itf.reset;
        assign u_xt[j_u_decode_block_local_0].itf.s_valid = c_yt_valid_scclang_global_5[(0)];
    assign c_xt_ready_scclang_global_1[(j_u_decode_block_local_0)] = u_xt[j_u_decode_block_local_0].itf.s_ready;
    assign c_xt_valid_scclang_global_2[(j_u_decode_block_local_0)] = u_xt[j_u_decode_block_local_0].itf.m_valid;
        assign u_xt[j_u_decode_block_local_0].itf.m_ready = itf.m_ready;
        assign u_yt[j_u_decode_block_local_0].itf.clk = itf.clk;
        assign u_yt[j_u_decode_block_local_0].itf.reset = itf.reset;
        assign u_yt[j_u_decode_block_local_0].itf.s_valid = xors_valid_scclang_global_6;
    assign c_yt_ready_scclang_global_4[(j_u_decode_block_local_0)] = u_yt[j_u_decode_block_local_0].itf.s_ready;
    assign c_yt_valid_scclang_global_5[(j_u_decode_block_local_0)] = u_yt[j_u_decode_block_local_0].itf.m_valid;
        assign u_yt[j_u_decode_block_local_0].itf.m_ready = itf.m_ready;
    for (i_u_decode_block_local_1 = 0;(i_u_decode_block_local_1) < (4);i_u_decode_block_local_1++) begin
            assign u_xt[j_u_decode_block_local_0].itf.s_port[i_u_decode_block_local_1] = c_xt_data_scclang_global_0[j_u_decode_block_local_0][i_u_decode_block_local_1];
      assign itf.m_block[((j_u_decode_block_local_0) * (4)) + (i_u_decode_block_local_1)] = u_xt[j_u_decode_block_local_0].itf.m_port[i_u_decode_block_local_1];
            assign u_yt[j_u_decode_block_local_0].itf.s_port[i_u_decode_block_local_1] = c_yt_data_scclang_global_3[i_u_decode_block_local_1][j_u_decode_block_local_0];
      assign c_xt_data_scclang_global_0[i_u_decode_block_local_1][j_u_decode_block_local_0] = u_yt[j_u_decode_block_local_0].itf.m_port[i_u_decode_block_local_1];
    end
  end
endmodule
module inv_lift_sc_module_13 (
 inv_lift_sc_module_13_interface.port0 itf
);
  logic [5:0] v_scclang_global_0;
  logic signed[63:0] w_scclang_global_1[0:5];
  logic signed[63:0] x_scclang_global_2[0:5];
  logic signed[63:0] y_scclang_global_3[0:5];
  logic signed[63:0] z_scclang_global_4[0:5];
  logic signed[31:0] stages_scclang_global_5 = 6;
  always @(itf.m_ready or x_scclang_global_2[((6) - (1))] or y_scclang_global_3[((6) - (1))] or z_scclang_global_4[((6) - (1))] or w_scclang_global_1[((6) - (1))] or v_scclang_global_0) begin: mc_proc
    
    
    itf.s_ready <= itf.m_ready;
    itf.m_port[0] <= x_scclang_global_2[(6) - (1)];
    itf.m_port[1] <= y_scclang_global_3[(6) - (1)];
    itf.m_port[2] <= z_scclang_global_4[(6) - (1)];
    itf.m_port[3] <= w_scclang_global_1[(6) - (1)];
    itf.m_valid <= v_scclang_global_0[(6) - (1)];
  end
  always_ff @(posedge itf.clk) begin: ms_proc
    logic signed[31:0] i_ms_proc_local_2;
    
    if ((itf.reset) == (0)) begin
      for (i_ms_proc_local_2 = 0;(i_ms_proc_local_2) < (6);i_ms_proc_local_2++) begin
        x_scclang_global_2[i_ms_proc_local_2] <= 64'd0;
        y_scclang_global_3[i_ms_proc_local_2] <= 64'd0;
        z_scclang_global_4[i_ms_proc_local_2] <= 64'd0;
        w_scclang_global_1[i_ms_proc_local_2] <= 64'd0;
      end
      v_scclang_global_0 <= 6'd0;
    end else begin
      if (itf.m_ready) begin
        w_scclang_global_1[0] <= itf.s_port[3];
        x_scclang_global_2[0] <= itf.s_port[0];
        y_scclang_global_3[0] <= (itf.s_port[1]) + ((itf.s_port[3]) >>> (1));
        z_scclang_global_4[0] <= itf.s_port[2];
        w_scclang_global_1[1] <= (w_scclang_global_1[0]) - ((y_scclang_global_3[0]) >>> (1));
        x_scclang_global_2[1] <= x_scclang_global_2[0];
        y_scclang_global_3[1] <= y_scclang_global_3[0];
        z_scclang_global_4[1] <= z_scclang_global_4[0];
        w_scclang_global_1[2] <= w_scclang_global_1[1];
        x_scclang_global_2[2] <= x_scclang_global_2[1];
        y_scclang_global_3[2] <= (y_scclang_global_3[1]) + (w_scclang_global_1[1]);
        z_scclang_global_4[2] <= (z_scclang_global_4[1]) + (x_scclang_global_2[1]);
        w_scclang_global_1[3] <= ((w_scclang_global_1[2]) << (1)) - (y_scclang_global_3[2]);
        x_scclang_global_2[3] <= ((x_scclang_global_2[2]) << (1)) - (z_scclang_global_4[2]);
        y_scclang_global_3[3] <= y_scclang_global_3[2];
        z_scclang_global_4[3] <= z_scclang_global_4[2];
        w_scclang_global_1[4] <= (w_scclang_global_1[3]) + (x_scclang_global_2[3]);
        x_scclang_global_2[4] <= x_scclang_global_2[3];
        y_scclang_global_3[4] <= (y_scclang_global_3[3]) + (z_scclang_global_4[3]);
        z_scclang_global_4[4] <= z_scclang_global_4[3];
        w_scclang_global_1[5] <= w_scclang_global_1[4];
        x_scclang_global_2[5] <= ((x_scclang_global_2[4]) << (1)) - (w_scclang_global_1[4]);
        y_scclang_global_3[5] <= y_scclang_global_3[4];
        z_scclang_global_4[5] <= ((z_scclang_global_4[4]) << (1)) - (y_scclang_global_3[4]);
        v_scclang_global_0 <= ((v_scclang_global_0) << (1)) | (itf.s_valid);
      end
    end

  end
endmodule
module inv_lift_sc_module_14 (
 inv_lift_sc_module_14_interface.port0 itf
);
  logic [5:0] v_scclang_global_0;
  logic signed[63:0] w_scclang_global_1[0:5];
  logic signed[63:0] x_scclang_global_2[0:5];
  logic signed[63:0] y_scclang_global_3[0:5];
  logic signed[63:0] z_scclang_global_4[0:5];
  logic signed[31:0] stages_scclang_global_5 = 6;
  always @(itf.m_ready or x_scclang_global_2[((6) - (1))] or y_scclang_global_3[((6) - (1))] or z_scclang_global_4[((6) - (1))] or w_scclang_global_1[((6) - (1))] or v_scclang_global_0) begin: mc_proc
    
    
    itf.s_ready <= itf.m_ready;
    itf.m_port[0] <= x_scclang_global_2[(6) - (1)];
    itf.m_port[1] <= y_scclang_global_3[(6) - (1)];
    itf.m_port[2] <= z_scclang_global_4[(6) - (1)];
    itf.m_port[3] <= w_scclang_global_1[(6) - (1)];
    itf.m_valid <= v_scclang_global_0[(6) - (1)];
  end
  always_ff @(posedge itf.clk) begin: ms_proc
    logic signed[31:0] i_ms_proc_local_2;
    
    if ((itf.reset) == (0)) begin
      for (i_ms_proc_local_2 = 0;(i_ms_proc_local_2) < (6);i_ms_proc_local_2++) begin
        x_scclang_global_2[i_ms_proc_local_2] <= 64'd0;
        y_scclang_global_3[i_ms_proc_local_2] <= 64'd0;
        z_scclang_global_4[i_ms_proc_local_2] <= 64'd0;
        w_scclang_global_1[i_ms_proc_local_2] <= 64'd0;
      end
      v_scclang_global_0 <= 6'd0;
    end else begin
      if (itf.m_ready) begin
        w_scclang_global_1[0] <= itf.s_port[3];
        x_scclang_global_2[0] <= itf.s_port[0];
        y_scclang_global_3[0] <= (itf.s_port[1]) + ((itf.s_port[3]) >>> (1));
        z_scclang_global_4[0] <= itf.s_port[2];
        w_scclang_global_1[1] <= (w_scclang_global_1[0]) - ((y_scclang_global_3[0]) >>> (1));
        x_scclang_global_2[1] <= x_scclang_global_2[0];
        y_scclang_global_3[1] <= y_scclang_global_3[0];
        z_scclang_global_4[1] <= z_scclang_global_4[0];
        w_scclang_global_1[2] <= w_scclang_global_1[1];
        x_scclang_global_2[2] <= x_scclang_global_2[1];
        y_scclang_global_3[2] <= (y_scclang_global_3[1]) + (w_scclang_global_1[1]);
        z_scclang_global_4[2] <= (z_scclang_global_4[1]) + (x_scclang_global_2[1]);
        w_scclang_global_1[3] <= ((w_scclang_global_1[2]) << (1)) - (y_scclang_global_3[2]);
        x_scclang_global_2[3] <= ((x_scclang_global_2[2]) << (1)) - (z_scclang_global_4[2]);
        y_scclang_global_3[3] <= y_scclang_global_3[2];
        z_scclang_global_4[3] <= z_scclang_global_4[2];
        w_scclang_global_1[4] <= (w_scclang_global_1[3]) + (x_scclang_global_2[3]);
        x_scclang_global_2[4] <= x_scclang_global_2[3];
        y_scclang_global_3[4] <= (y_scclang_global_3[3]) + (z_scclang_global_4[3]);
        z_scclang_global_4[4] <= z_scclang_global_4[3];
        w_scclang_global_1[5] <= w_scclang_global_1[4];
        x_scclang_global_2[5] <= ((x_scclang_global_2[4]) << (1)) - (w_scclang_global_1[4]);
        y_scclang_global_3[5] <= y_scclang_global_3[4];
        z_scclang_global_4[5] <= ((z_scclang_global_4[4]) << (1)) - (y_scclang_global_3[4]);
        v_scclang_global_0 <= ((v_scclang_global_0) << (1)) | (itf.s_valid);
      end
    end

  end
endmodule
module block_buffer_sc_module_7 (
 block_buffer_sc_module_7_interface.port0 itf
);
  logic [1023:0] c_bi_scclang_global_0;
  logic [1023:0] c_bo_scclang_global_1;
  initial begin

  end
  fifo_cc_sc_module_15_interface u_bbuf_itf();

  fifo_cc_sc_module_15 u_bbuf(
    u_bbuf_itf
  );
    assign u_bbuf_itf.clk = itf.clk;
    assign u_bbuf_itf.reset = itf.reset;
    assign u_bbuf_itf.din = c_bi_scclang_global_0;
    assign u_bbuf_itf.wr_en = itf.s_valid;
    assign itf.s_ready = u_bbuf_itf.full;
    assign c_bo_scclang_global_1 = u_bbuf_itf.dout;
    assign u_bbuf_itf.rd_en = itf.m_ready;
    assign itf.m_valid = u_bbuf_itf.empty;
  always @(*) begin
  end

  always @(c_bo_scclang_global_1) begin: mc_proc
    logic [1023:0] bbufi_mc_proc_local_2;
    logic [1023:0] bbufo_mc_proc_local_3;
    logic signed[31:0] i_mc_proc_local_4;
    logic signed[31:0] i_mc_proc_local_5;
    
    for (i_mc_proc_local_4 = 0;(i_mc_proc_local_4) < (16);i_mc_proc_local_4++) begin
      bbufi_mc_proc_local_2[((i_mc_proc_local_4) * (64)) +: (64)] = itf.s_block[i_mc_proc_local_4];
    end
    c_bi_scclang_global_0 <= bbufi_mc_proc_local_2;
    bbufo_mc_proc_local_3 = c_bo_scclang_global_1;
    for (i_mc_proc_local_5 = 0;(i_mc_proc_local_5) < (16);i_mc_proc_local_5++) begin
      itf.m_block[i_mc_proc_local_5] <= bbufo_mc_proc_local_3[((i_mc_proc_local_5) * (64)) +: (64)];
    end
  end
genvar i_u_cast_buffer_local_1;
  /*generate*/ for (i_u_cast_buffer_local_1 = 0;(i_u_cast_buffer_local_1) < (16);i_u_cast_buffer_local_1++) begin
    always @(itf.s_block[(i_u_cast_buffer_local_1)]) begin: mc_proc
      logic [1023:0] bbufi_mc_proc_local_2;
      logic [1023:0] bbufo_mc_proc_local_3;
      logic signed[31:0] i_mc_proc_local_4;
      logic signed[31:0] i_mc_proc_local_5;
      
      /*generate*/ for (i_mc_proc_local_4 = 0;(i_mc_proc_local_4) < (16);i_mc_proc_local_4++) begin
        bbufi_mc_proc_local_2[((i_mc_proc_local_4) * (64)) +: (64)] = itf.s_block[i_mc_proc_local_4];
      end
      c_bi_scclang_global_0 <= bbufi_mc_proc_local_2;
      bbufo_mc_proc_local_3 = c_bo_scclang_global_1;
      /*generate*/ for (i_mc_proc_local_5 = 0;(i_mc_proc_local_5) < (16);i_mc_proc_local_5++) begin
        itf.m_block[i_mc_proc_local_5] <= bbufo_mc_proc_local_3[((i_mc_proc_local_5) * (64)) +: (64)];
      end
    end
  end
endmodule
module fifo_cc_sc_module_15 (
 fifo_cc_sc_module_15_interface.port0 itf
);
  logic [1023:0] data_scclang_global_0[0:1];
  logic [0:0] empty_i_scclang_global_1;
  logic [0:0] full_i_scclang_global_2;
  logic [0:0] rd_en_i_scclang_global_3;
  logic [0:0] rd_idx_scclang_global_4;
  logic [0:0] wr_en_i_scclang_global_5;
  logic [0:0] wr_idx_scclang_global_6;
  logic [31:0] depth_scclang_global_7;
  logic signed[31:0] MAX_DEPTH_scclang_global_8 = 2;
  initial begin

  end
  always @(itf.wr_en or itf.rd_en or full_i_scclang_global_2 or empty_i_scclang_global_1 or rd_idx_scclang_global_4) begin: mc_proc
    
    
    if (1) begin
      itf.dout <= data_scclang_global_0[rd_idx_scclang_global_4];
    end
    wr_en_i_scclang_global_5 <= (itf.wr_en) && (!(full_i_scclang_global_2));
    rd_en_i_scclang_global_3 <= (itf.rd_en) && (!(empty_i_scclang_global_1));
    itf.full <= (full_i_scclang_global_2) == (0);
    itf.empty <= (empty_i_scclang_global_1) == (0);
  end
  always_ff @(posedge itf.clk) begin: ms_proc
    logic [0:0] wr_inc_ms_proc_local_3;
    logic [0:0] rd_inc_ms_proc_local_4;
    logic [31:0] i_ms_proc_local_5;
    
    wr_inc_ms_proc_local_3 = ((wr_idx_scclang_global_6) + (1)) % (2);
    rd_inc_ms_proc_local_4 = ((rd_idx_scclang_global_4) + (1)) % (2);
    if ((itf.reset) == (0)) begin
      if (!(1)) begin
        itf.dout <= 0;
      end
      for (i_ms_proc_local_5 = 0;(i_ms_proc_local_5) < (2);i_ms_proc_local_5++) begin
        data_scclang_global_0[i_ms_proc_local_5] <= 0;
      end
      rd_idx_scclang_global_4 <= 1'd0;
      wr_idx_scclang_global_6 <= 1'd0;
      full_i_scclang_global_2 <= 0;
      empty_i_scclang_global_1 <= 1;
    end else begin
      if (!(1)) begin
        if (rd_en_i_scclang_global_3) begin
          itf.dout <= data_scclang_global_0[rd_idx_scclang_global_4];
        end
      end
      if (wr_en_i_scclang_global_5) begin
        data_scclang_global_0[wr_idx_scclang_global_6] <= itf.din;
        wr_idx_scclang_global_6 <= wr_inc_ms_proc_local_3;
        if (!(rd_en_i_scclang_global_3)) begin
          if ((wr_inc_ms_proc_local_3) == (rd_idx_scclang_global_4)) begin
            full_i_scclang_global_2 <= 1;
          end
          empty_i_scclang_global_1 <= 0;
        end
      end
      if (rd_en_i_scclang_global_3) begin
        rd_idx_scclang_global_4 <= rd_inc_ms_proc_local_4;
        if (!(wr_en_i_scclang_global_5)) begin
          full_i_scclang_global_2 <= 0;
          if ((rd_inc_ms_proc_local_4) == (wr_idx_scclang_global_6)) begin
            empty_i_scclang_global_1 <= 1;
          end
        end
      end
    end

  end
genvar i_u_bbuf_local_1;
  /*generate*/ for (i_u_bbuf_local_1 = 0;(i_u_bbuf_local_1) < (2);i_u_bbuf_local_1++) begin
    always @(data_scclang_global_0[(i_u_bbuf_local_1)]) begin: mc_proc
      
      
      if (1) begin
        itf.dout <= data_scclang_global_0[rd_idx_scclang_global_4];
      end
      wr_en_i_scclang_global_5 <= (itf.wr_en) && (!(full_i_scclang_global_2));
      rd_en_i_scclang_global_3 <= (itf.rd_en) && (!(empty_i_scclang_global_1));
      itf.full <= (full_i_scclang_global_2) == (0);
      itf.empty <= (empty_i_scclang_global_1) == (0);
    end
  end
endmodule
module inv_cast_sc_module_8 (
 inv_cast_sc_module_8_interface.port0 itf
);
  logic [0:0] c_sync_scclang_global_0;
  logic [3:0] count_scclang_global_1;
  logic [0:0] got_bhdr_scclang_global_2;
  logic [0:0] got_blk_scclang_global_3;
  logic signed[63:0] r_blk_scclang_global_4[0:15];
  logic [10:0] r_ex_scclang_global_5;
  logic [0:0] zb_scclang_global_6;
  logic [63:0] ui_actual_scclang_global_7;
  always @(itf.s_valid or itf.s_bhdr_valid or got_blk_scclang_global_3 or got_bhdr_scclang_global_2 or itf.m_stream_ready or count_scclang_global_1 or c_sync_scclang_global_0) begin: mc_proc
    logic [51:0] fp_mc_proc_local_2_frac;
    logic [10:0] fp_mc_proc_local_2_expo;
    logic [0:0] fp_mc_proc_local_2_sign;
    logic [0:0] s_mc_proc_local_3;
    logic [10:0] _r_ex_mc_proc_local_4;
    logic [0:0] stall_mc_proc_local_5;
    logic signed[63:0] si_mc_proc_local_6;
    logic [63:0] neg_mask_mc_proc_local_7;
    logic [0:0] zero_output_mc_proc_local_8;
    logic [63:0] rn_mc_proc_local_9;
    logic [10:0] e_mc_proc_local_10;
    logic [63:0] i_mc_proc_local_11;
    logic signed[31:0] shift_mc_proc_local_12;
    logic [51:0] fp_mc_proc_local_2_frac__ref_0;
    logic [0:0] fp_mc_proc_local_2_sign__ref_0;
    logic [10:0] fp_mc_proc_local_2_expo__ref_0;
    
    fp_mc_proc_local_2_expo__ref_0 = fp_mc_proc_local_2_expo;
    fp_mc_proc_local_2_sign__ref_0 = fp_mc_proc_local_2_sign;
    fp_mc_proc_local_2_frac__ref_0 = fp_mc_proc_local_2_frac;
    fp_t11_52__fp_t_func_0(fp_mc_proc_local_2_frac__ref_0,fp_mc_proc_local_2_expo__ref_0,fp_mc_proc_local_2_sign__ref_0,0,fp_mc_proc_local_2_frac__ref_0,fp_mc_proc_local_2_expo__ref_0,fp_mc_proc_local_2_sign__ref_0);
    s_mc_proc_local_3 = 1'd0;
    _r_ex_mc_proc_local_4 = 11'd0;
    stall_mc_proc_local_5 = (((itf.m_stream_ready) == (0)) || (((count_scclang_global_1) == (0)) && (!(got_bhdr_scclang_global_2)))) || (((count_scclang_global_1) == (0)) && ((got_bhdr_scclang_global_2) && (!(got_blk_scclang_global_3))));
    c_sync_scclang_global_0 <= !(stall_mc_proc_local_5);
    itf.m_stream_valid <= !(stall_mc_proc_local_5);
    si_mc_proc_local_6 = r_blk_scclang_global_4[count_scclang_global_1];
    neg_mask_mc_proc_local_7 = (1) << ((64) - (1));
    if ((si_mc_proc_local_6) & (neg_mask_mc_proc_local_7)) begin
      si_mc_proc_local_6 = -(si_mc_proc_local_6);
      s_mc_proc_local_3 = 1;
    end
    zero_output_mc_proc_local_8 = ((si_mc_proc_local_6) == (0)) || (zb_scclang_global_6);
    if (zero_output_mc_proc_local_8) begin
      fp_mc_proc_local_2_frac__ref_0 = 0;
      fp_mc_proc_local_2_expo__ref_0 = 0;
      fp_mc_proc_local_2_sign__ref_0 = 0;
    end else begin
      rn_mc_proc_local_9 = si_mc_proc_local_6;
      e_mc_proc_local_10 = 11'd0;
      for (i_mc_proc_local_11 = 64'd0;(i_mc_proc_local_11) < (64);i_mc_proc_local_11++) begin
        if (!((rn_mc_proc_local_9) >>> ((e_mc_proc_local_10) + (1)))) begin
          break;
        end else begin
          e_mc_proc_local_10++;
        end

      end
      shift_mc_proc_local_12 = (52) - (e_mc_proc_local_10);
      if ((shift_mc_proc_local_12) > (0)) begin
        rn_mc_proc_local_9 = (rn_mc_proc_local_9) << (+(shift_mc_proc_local_12));
      end else begin
        rn_mc_proc_local_9 = (rn_mc_proc_local_9) >>> (-(shift_mc_proc_local_12));
      end

      e_mc_proc_local_10 = (e_mc_proc_local_10) + ((r_ex_scclang_global_5) - ((64) - (2)));
      if ((e_mc_proc_local_10) <= (0)) begin
        rn_mc_proc_local_9 = (rn_mc_proc_local_9) >>> ((1) - (e_mc_proc_local_10));
        e_mc_proc_local_10 = 0;
      end else begin
        rn_mc_proc_local_9 = (rn_mc_proc_local_9) & (((1) << (52)) - (1));
      end

      rn_mc_proc_local_9 = (rn_mc_proc_local_9) + ((((s_mc_proc_local_3) << (11)) + (e_mc_proc_local_10)) << (52));
      { { fp_mc_proc_local_2_sign__ref_0, fp_mc_proc_local_2_expo__ref_0 }, fp_mc_proc_local_2_frac__ref_0 } = rn_mc_proc_local_9;
    end

    itf.m_stream_data_frac <= fp_mc_proc_local_2_frac__ref_0;
    itf.m_stream_data_expo <= fp_mc_proc_local_2_expo__ref_0;
    itf.m_stream_data_sign <= fp_mc_proc_local_2_sign__ref_0;
    fp_mc_proc_local_2_frac = fp_mc_proc_local_2_frac__ref_0;
    fp_mc_proc_local_2_sign = fp_mc_proc_local_2_sign__ref_0;
    fp_mc_proc_local_2_expo = fp_mc_proc_local_2_expo__ref_0;
  end
  always_ff @(posedge itf.clk) begin: ms_proc
    logic [0:0] _zb_ms_proc_local_13;
    logic [0:0] _got_bhdr_ms_proc_local_14;
    logic [31:0] i_ms_proc_local_15;
    logic [31:0] i_ms_proc_local_16;
    
    if ((itf.reset) == (0)) begin
      count_scclang_global_1 <= 4'd0;
      itf.s_bhdr_ready <= 0;
      itf.s_ready <= 0;
      got_bhdr_scclang_global_2 <= 0;
      got_blk_scclang_global_3 <= 0;
    end else begin
      _zb_ms_proc_local_13 = 0;
      _got_bhdr_ms_proc_local_14 = 0;
      if ((((count_scclang_global_1) == (0)) && (itf.s_bhdr_valid)) && ((got_bhdr_scclang_global_2) == (0))) begin
        _got_bhdr_ms_proc_local_14 = 1;
        got_bhdr_scclang_global_2 <= _got_bhdr_ms_proc_local_14;
        _zb_ms_proc_local_13 = itf.s_bhdr_data_zb;
        zb_scclang_global_6 <= _zb_ms_proc_local_13;
        r_ex_scclang_global_5 <= itf.s_bhdr_data_exp;
        if (_zb_ms_proc_local_13) begin
          for (i_ms_proc_local_15 = 0;(i_ms_proc_local_15) < (16);i_ms_proc_local_15++) begin
            r_blk_scclang_global_4[i_ms_proc_local_15] <= 64'd0;
          end
          got_blk_scclang_global_3 <= 1;
        end
        itf.s_bhdr_ready <= 1;
      end
      if ((got_bhdr_scclang_global_2) == (1)) begin
        itf.s_bhdr_ready <= 0;
      end
      if (((count_scclang_global_1) == (0)) && (((_got_bhdr_ms_proc_local_14) && (!(_zb_ms_proc_local_13))) || ((got_bhdr_scclang_global_2) && (!(zb_scclang_global_6))))) begin
        if (itf.s_valid) begin
          got_blk_scclang_global_3 <= 1;
          for (i_ms_proc_local_16 = 0;(i_ms_proc_local_16) < (16);i_ms_proc_local_16++) begin
            r_blk_scclang_global_4[i_ms_proc_local_16] <= itf.s_block[i_ms_proc_local_16];
          end
          itf.s_ready <= 1;
        end
      end
      if (got_blk_scclang_global_3) begin
        itf.s_ready <= 0;
      end
      if ((count_scclang_global_1) != (0)) begin
        got_bhdr_scclang_global_2 <= 0;
        got_blk_scclang_global_3 <= 0;
      end
      if (c_sync_scclang_global_0) begin
        if ((count_scclang_global_1) == ((16) - (1))) begin
          count_scclang_global_1 <= 4'd0;
        end else begin
          count_scclang_global_1 <= (count_scclang_global_1) + (1);
        end

      end
    end

  end
  function automatic void fp_t11_52__fp_t_func_0 (input logic [51:0] hthis_frac, input logic [10:0] hthis_expo, input logic [0:0] hthis_sign, input logic [63:0] ui, inout logic [51:0] hthis_frac__ref_0, inout logic [10:0] hthis_expo__ref_0, inout logic [0:0] hthis_sign__ref_0);
  begin

    ui_actual_scclang_global_7 = ui;
    { { hthis_sign, hthis_expo }, hthis_frac } = ui_actual_scclang_global_7;
  end
  endfunction
endmodule