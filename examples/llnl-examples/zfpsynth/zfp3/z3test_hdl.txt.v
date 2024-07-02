interface encode_sc_module_1_interface;
  logic [0:0] clk;
  logic [0:0] reset;
  logic [15:0] minbits;
  logic [15:0] maxbits;
  logic [15:0] maxprec;
  logic signed[15:0] minexp;
  logic [51:0] s_fp_data_frac;
  logic [10:0] s_fp_data_expo;
  logic [0:0] s_fp_data_sign;
  logic [0:0] s_fp_valid;
  logic [0:0] s_fp_ready;
  logic [63:0] m_bits_data_tdata;
  logic [0:0] m_bits_data_tlast;
  logic [0:0] m_bits_valid;
  logic [0:0] m_bits_ready;
  modport port0(
    input clk,reset,minbits,maxbits,maxprec,minexp,s_fp_data_frac,s_fp_data_expo,s_fp_data_sign,s_fp_valid,m_bits_ready,
    output s_fp_ready,m_bits_data_tdata,m_bits_data_tlast,m_bits_valid
  );
endinterface

interface find_emax_sc_module_2_interface;
  logic [0:0] clk;
  logic [0:0] reset;
  logic [51:0] s_fp_data_frac;
  logic [10:0] s_fp_data_expo;
  logic [0:0] s_fp_data_sign;
  logic [0:0] s_fp_valid;
  logic [0:0] s_fp_ready;
  logic [51:0] m_fp_data_frac;
  logic [10:0] m_fp_data_expo;
  logic [0:0] m_fp_data_sign;
  logic [0:0] m_fp_valid;
  logic [0:0] m_fp_ready;
  logic [10:0] m_ex_data;
  logic [0:0] m_ex_valid;
  logic [0:0] m_ex_ready;
  modport port0(
    input clk,reset,s_fp_data_frac,s_fp_data_expo,s_fp_data_sign,s_fp_valid,m_fp_ready,m_ex_ready,
    output s_fp_ready,m_fp_data_frac,m_fp_data_expo,m_fp_data_sign,m_fp_valid,m_ex_data,m_ex_valid
  );
endinterface

interface rvfifo_cc_sc_module_9_interface;
  logic [0:0] clk;
  logic [0:0] reset;
  logic [51:0] s_port_data_frac;
  logic [10:0] s_port_data_expo;
  logic [0:0] s_port_data_sign;
  logic [0:0] s_port_valid;
  logic [0:0] s_port_ready;
  logic [51:0] m_port_data_frac;
  logic [10:0] m_port_data_expo;
  logic [0:0] m_port_data_sign;
  logic [0:0] m_port_valid;
  logic [0:0] m_port_ready;
  modport port0(
    input clk,reset,s_port_data_frac,s_port_data_expo,s_port_data_sign,s_port_valid,m_port_ready,
    output s_port_ready,m_port_data_frac,m_port_data_expo,m_port_data_sign,m_port_valid
  );
endinterface

interface fifo_cc_sc_module_11_interface;
  logic [0:0] clk;
  logic [0:0] reset;
  logic [51:0] din_frac;
  logic [10:0] din_expo;
  logic [0:0] din_sign;
  logic [0:0] wr_en;
  logic [0:0] rd_en;
  logic [0:0] full;
  logic [51:0] dout_frac;
  logic [10:0] dout_expo;
  logic [0:0] dout_sign;
  logic [0:0] empty;
  modport port0(
    input clk,reset,din_frac,din_expo,din_sign,wr_en,rd_en,
    output full,dout_frac,dout_expo,dout_sign,empty
  );
endinterface

interface sreg_sc_module_10_interface;
  logic [0:0] clk;
  logic [0:0] reset;
  logic [10:0] s_port_data;
  logic [0:0] s_port_valid;
  logic [0:0] s_port_ready;
  logic [10:0] m_port_data;
  logic [0:0] m_port_valid;
  logic [0:0] m_port_ready;
  modport port0(
    input clk,reset,s_port_data,s_port_valid,m_port_ready,
    output s_port_ready,m_port_data,m_port_valid
  );
endinterface

interface ssplit_sc_module_3_interface;
  logic [0:0] clk;
  logic [0:0] reset;
  logic [10:0] s_port_data;
  logic [0:0] s_port_valid;
  logic [0:0] s_port_ready;
  logic [10:0] m_port1_data;
  logic [0:0] m_port1_valid;
  logic [0:0] m_port1_ready;
  logic [10:0] m_port2_data;
  logic [0:0] m_port2_valid;
  logic [0:0] m_port2_ready;
  modport port0(
    input clk,reset,s_port_data,s_port_valid,m_port1_ready,m_port2_ready,
    output s_port_ready,m_port1_data,m_port1_valid,m_port2_data,m_port2_valid
  );
endinterface

interface rvfifo_cc_sc_module_12_interface;
  logic [0:0] clk;
  logic [0:0] reset;
  logic [10:0] s_port_data;
  logic [0:0] s_port_valid;
  logic [0:0] s_port_ready;
  logic [10:0] m_port_data;
  logic [0:0] m_port_valid;
  logic [0:0] m_port_ready;
  modport port0(
    input clk,reset,s_port_data,s_port_valid,m_port_ready,
    output s_port_ready,m_port_data,m_port_valid
  );
endinterface

interface fifo_cc_sc_module_14_interface;
  logic [0:0] clk;
  logic [0:0] reset;
  logic [10:0] din;
  logic [0:0] wr_en;
  logic [0:0] rd_en;
  logic [0:0] full;
  logic [10:0] dout;
  logic [0:0] empty;
  modport port0(
    input clk,reset,din,wr_en,rd_en,
    output full,dout,empty
  );
endinterface

interface rvfifo_cc_sc_module_13_interface;
  logic [0:0] clk;
  logic [0:0] reset;
  logic [10:0] s_port_data;
  logic [0:0] s_port_valid;
  logic [0:0] s_port_ready;
  logic [10:0] m_port_data;
  logic [0:0] m_port_valid;
  logic [0:0] m_port_ready;
  modport port0(
    input clk,reset,s_port_data,s_port_valid,m_port_ready,
    output s_port_ready,m_port_data,m_port_valid
  );
endinterface

interface fifo_cc_sc_module_15_interface;
  logic [0:0] clk;
  logic [0:0] reset;
  logic [10:0] din;
  logic [0:0] wr_en;
  logic [0:0] rd_en;
  logic [0:0] full;
  logic [10:0] dout;
  logic [0:0] empty;
  modport port0(
    input clk,reset,din,wr_en,rd_en,
    output full,dout,empty
  );
endinterface

interface fwd_cast_sc_module_4_interface;
  logic [0:0] clk;
  logic [0:0] reset;
  logic [0:0] m_ready;
  logic [10:0] s_ex_data;
  logic [0:0] s_ex_valid;
  logic [0:0] s_ex_ready;
  logic [51:0] s_fp_data_frac;
  logic [10:0] s_fp_data_expo;
  logic [0:0] s_fp_data_sign;
  logic [0:0] s_fp_valid;
  logic [0:0] s_fp_ready;
  logic signed[63:0] m_block[0:15];
  logic [0:0] m_valid;
  modport port0(
    input clk,reset,m_ready,s_ex_data,s_ex_valid,s_fp_data_frac,s_fp_data_expo,s_fp_data_sign,s_fp_valid,
    output s_ex_ready,s_fp_ready,m_block,m_valid
  );
endinterface

interface encode_block_sc_module_5_interface;
  logic [0:0] clk;
  logic [0:0] reset;
  logic signed[63:0] s_block[0:15];
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

interface fwd_lift_sc_module_16_interface;
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

interface fwd_lift_sc_module_17_interface;
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

interface block_buffer_sc_module_6_interface;
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

interface fifo_cc_sc_module_18_interface;
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

interface encode_ints_sc_module_7_interface;
  logic [0:0] clk;
  logic [0:0] reset;
  logic [63:0] s_block[0:15];
  logic [0:0] s_valid;
  logic [0:0] s_flush;
  logic [0:0] m_ready;
  logic [0:0] s_ready;
  logic [5:0] m_bc;
  logic [32:0] m_bp;
  logic [0:0] m_last;
  logic [0:0] m_valid;
  modport port0(
    input clk,reset,s_block,s_valid,s_flush,m_ready,
    output s_ready,m_bc,m_bp,m_last,m_valid
  );
endinterface

interface encode_stream_sc_module_8_interface;
  logic [0:0] clk;
  logic [0:0] reset;
  logic [15:0] minbits;
  logic [15:0] maxbits;
  logic [15:0] maxprec;
  logic signed[15:0] minexp;
  logic [5:0] s_bc;
  logic [32:0] s_bp;
  logic [0:0] s_last;
  logic [0:0] s_valid;
  logic [10:0] s_ex_data;
  logic [0:0] s_ex_valid;
  logic [0:0] s_ex_ready;
  logic [0:0] s_ready;
  logic [0:0] m_flush;
  logic [63:0] m_bits_data_tdata;
  logic [0:0] m_bits_data_tlast;
  logic [0:0] m_bits_valid;
  logic [0:0] m_bits_ready;
  modport port0(
    input clk,reset,minbits,maxbits,maxprec,minexp,s_bc,s_bp,s_last,s_valid,s_ex_data,s_ex_valid,m_bits_ready,
    output s_ex_ready,s_ready,m_flush,m_bits_data_tdata,m_bits_data_tlast,m_bits_valid
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
  logic [51:0] c_driver_fp_scclang_global_6_data_frac;
  logic [10:0] c_driver_fp_scclang_global_6_data_expo;
  logic [0:0] c_driver_fp_scclang_global_6_data_sign;
  logic [0:0] c_driver_fp_scclang_global_6_valid;
  logic [0:0] c_driver_fp_scclang_global_6_ready;
  logic [63:0] c_dut_enc_scclang_global_7_data_tdata;
  logic [0:0] c_dut_enc_scclang_global_7_data_tlast;
  logic [0:0] c_dut_enc_scclang_global_7_valid;
  logic [0:0] c_dut_enc_scclang_global_7_ready;
  initial begin
    minbits_scclang_global_3 = 1024;
    maxbits_scclang_global_1 = 1024;
    maxprec_scclang_global_2 = 64;
    minexp_scclang_global_4 = ((1) - (1023)) - (52);
  end
  encode_sc_module_1_interface u_dut_itf();

  encode_sc_module_1 u_dut(
    u_dut_itf
  );
    assign u_dut_itf.clk = clk_scclang_global_0;
    assign u_dut_itf.reset = reset_scclang_global_5;
    assign u_dut_itf.minbits = minbits_scclang_global_3;
    assign u_dut_itf.maxbits = maxbits_scclang_global_1;
    assign u_dut_itf.maxprec = maxprec_scclang_global_2;
    assign u_dut_itf.minexp = minexp_scclang_global_4;
    assign u_dut_itf.s_fp_data_frac = c_driver_fp_scclang_global_6_data_frac;
    assign u_dut_itf.s_fp_data_expo = c_driver_fp_scclang_global_6_data_expo;
    assign u_dut_itf.s_fp_data_sign = c_driver_fp_scclang_global_6_data_sign;
    assign u_dut_itf.s_fp_valid = c_driver_fp_scclang_global_6_valid;
    assign c_driver_fp_scclang_global_6_ready = u_dut_itf.s_fp_ready;
    assign c_dut_enc_scclang_global_7_data_tdata = u_dut_itf.m_bits_data_tdata;
    assign c_dut_enc_scclang_global_7_data_tlast = u_dut_itf.m_bits_data_tlast;
    assign c_dut_enc_scclang_global_7_valid = u_dut_itf.m_bits_valid;
    assign u_dut_itf.m_bits_ready = c_dut_enc_scclang_global_7_ready;
  always @(*) begin
  end

endmodule
module encode_sc_module_1 (
 encode_sc_module_1_interface.port0 itf
);
  logic [63:0] c_bb_block_scclang_global_0[0:15];
  logic [0:0] c_bb_ready_scclang_global_1;
  logic [0:0] c_bb_valid_scclang_global_2;
  logic [63:0] c_eb_block_scclang_global_3[0:15];
  logic [0:0] c_eb_ready_scclang_global_4;
  logic [0:0] c_eb_valid_scclang_global_5;
  logic [5:0] c_ei_bc_scclang_global_6;
  logic [32:0] c_ei_bp_scclang_global_7;
  logic [0:0] c_ei_last_scclang_global_8;
  logic [0:0] c_ei_ready_scclang_global_9;
  logic [0:0] c_ei_valid_scclang_global_10;
  logic [0:0] c_es_flush_scclang_global_11;
  logic signed[63:0] c_fc_block_scclang_global_12[0:15];
  logic [0:0] c_fc_ready_scclang_global_13;
  logic [0:0] c_fc_valid_scclang_global_14;
  logic [51:0] c_fe_fp_scclang_global_15_data_frac;
  logic [10:0] c_fe_fp_scclang_global_15_data_expo;
  logic [0:0] c_fe_fp_scclang_global_15_data_sign;
  logic [0:0] c_fe_fp_scclang_global_15_valid;
  logic [0:0] c_fe_fp_scclang_global_15_ready;
  logic [10:0] c_fe_ex_scclang_global_16_data;
  logic [0:0] c_fe_ex_scclang_global_16_valid;
  logic [0:0] c_fe_ex_scclang_global_16_ready;
  logic [10:0] c_sp_ex1_scclang_global_17_data;
  logic [0:0] c_sp_ex1_scclang_global_17_valid;
  logic [0:0] c_sp_ex1_scclang_global_17_ready;
  logic [10:0] c_sp_ex2_scclang_global_18_data;
  logic [0:0] c_sp_ex2_scclang_global_18_valid;
  logic [0:0] c_sp_ex2_scclang_global_18_ready;
  initial begin

  end
  find_emax_sc_module_2_interface u_find_emax_itf();

  find_emax_sc_module_2 u_find_emax(
    u_find_emax_itf
  );
    assign u_find_emax_itf.clk = itf.clk;
    assign u_find_emax_itf.reset = itf.reset;
    assign u_find_emax_itf.s_fp_data_frac = itf.s_fp_data_frac;
    assign u_find_emax_itf.s_fp_data_expo = itf.s_fp_data_expo;
    assign u_find_emax_itf.s_fp_data_sign = itf.s_fp_data_sign;
    assign u_find_emax_itf.s_fp_valid = itf.s_fp_valid;
    assign itf.s_fp_ready = u_find_emax_itf.s_fp_ready;
    assign c_fe_fp_scclang_global_15_data_frac = u_find_emax_itf.m_fp_data_frac;
    assign c_fe_fp_scclang_global_15_data_expo = u_find_emax_itf.m_fp_data_expo;
    assign c_fe_fp_scclang_global_15_data_sign = u_find_emax_itf.m_fp_data_sign;
    assign c_fe_fp_scclang_global_15_valid = u_find_emax_itf.m_fp_valid;
    assign u_find_emax_itf.m_fp_ready = c_fe_fp_scclang_global_15_ready;
    assign c_fe_ex_scclang_global_16_data = u_find_emax_itf.m_ex_data;
    assign c_fe_ex_scclang_global_16_valid = u_find_emax_itf.m_ex_valid;
    assign u_find_emax_itf.m_ex_ready = c_fe_ex_scclang_global_16_ready;
  always @(*) begin
  end

  ssplit_sc_module_3_interface u_ssplit_ex_itf();

  ssplit_sc_module_3 u_ssplit_ex(
    u_ssplit_ex_itf
  );
    assign u_ssplit_ex_itf.clk = itf.clk;
    assign u_ssplit_ex_itf.reset = itf.reset;
    assign u_ssplit_ex_itf.s_port_data = c_fe_ex_scclang_global_16_data;
    assign u_ssplit_ex_itf.s_port_valid = c_fe_ex_scclang_global_16_valid;
    assign c_fe_ex_scclang_global_16_ready = u_ssplit_ex_itf.s_port_ready;
    assign c_sp_ex1_scclang_global_17_data = u_ssplit_ex_itf.m_port1_data;
    assign c_sp_ex1_scclang_global_17_valid = u_ssplit_ex_itf.m_port1_valid;
    assign u_ssplit_ex_itf.m_port1_ready = c_sp_ex1_scclang_global_17_ready;
    assign c_sp_ex2_scclang_global_18_data = u_ssplit_ex_itf.m_port2_data;
    assign c_sp_ex2_scclang_global_18_valid = u_ssplit_ex_itf.m_port2_valid;
    assign u_ssplit_ex_itf.m_port2_ready = c_sp_ex2_scclang_global_18_ready;
  always @(*) begin
  end

  fwd_cast_sc_module_4_interface u_fwd_cast_itf();

  fwd_cast_sc_module_4 u_fwd_cast(
    u_fwd_cast_itf
  );
    assign u_fwd_cast_itf.clk = itf.clk;
    assign u_fwd_cast_itf.reset = itf.reset;
    assign u_fwd_cast_itf.s_fp_data_frac = c_fe_fp_scclang_global_15_data_frac;
    assign u_fwd_cast_itf.s_fp_data_expo = c_fe_fp_scclang_global_15_data_expo;
    assign u_fwd_cast_itf.s_fp_data_sign = c_fe_fp_scclang_global_15_data_sign;
    assign u_fwd_cast_itf.s_fp_valid = c_fe_fp_scclang_global_15_valid;
    assign c_fe_fp_scclang_global_15_ready = u_fwd_cast_itf.s_fp_ready;
    assign u_fwd_cast_itf.s_ex_data = c_sp_ex1_scclang_global_17_data;
    assign u_fwd_cast_itf.s_ex_valid = c_sp_ex1_scclang_global_17_valid;
    assign c_sp_ex1_scclang_global_17_ready = u_fwd_cast_itf.s_ex_ready;
    assign c_fc_valid_scclang_global_14 = u_fwd_cast_itf.m_valid;
    assign u_fwd_cast_itf.m_ready = c_fc_ready_scclang_global_13;
  always @(*) begin
  end

  encode_block_sc_module_5_interface u_encode_block_itf();

  encode_block_sc_module_5 u_encode_block(
    u_encode_block_itf
  );
    assign u_encode_block_itf.clk = itf.clk;
    assign u_encode_block_itf.reset = itf.reset;
    assign u_encode_block_itf.s_valid = c_fc_valid_scclang_global_14;
    assign c_fc_ready_scclang_global_13 = u_encode_block_itf.s_ready;
    assign c_eb_valid_scclang_global_5 = u_encode_block_itf.m_valid;
    assign u_encode_block_itf.m_ready = c_eb_ready_scclang_global_4;
  always @(*) begin
  end

  block_buffer_sc_module_6_interface u_block_buffer_itf();

  block_buffer_sc_module_6 u_block_buffer(
    u_block_buffer_itf
  );
    assign u_block_buffer_itf.clk = itf.clk;
    assign u_block_buffer_itf.reset = itf.reset;
    assign u_block_buffer_itf.s_valid = c_eb_valid_scclang_global_5;
    assign c_eb_ready_scclang_global_4 = u_block_buffer_itf.s_ready;
    assign c_bb_valid_scclang_global_2 = u_block_buffer_itf.m_valid;
    assign u_block_buffer_itf.m_ready = c_bb_ready_scclang_global_1;
  always @(*) begin
  end

  encode_ints_sc_module_7_interface u_encode_ints_itf();

  encode_ints_sc_module_7 u_encode_ints(
    u_encode_ints_itf
  );
    assign u_encode_ints_itf.clk = itf.clk;
    assign u_encode_ints_itf.reset = itf.reset;
    assign u_encode_ints_itf.s_valid = c_bb_valid_scclang_global_2;
    assign c_bb_ready_scclang_global_1 = u_encode_ints_itf.s_ready;
    assign u_encode_ints_itf.s_flush = c_es_flush_scclang_global_11;
    assign c_ei_bc_scclang_global_6 = u_encode_ints_itf.m_bc;
    assign c_ei_bp_scclang_global_7 = u_encode_ints_itf.m_bp;
    assign c_ei_last_scclang_global_8 = u_encode_ints_itf.m_last;
    assign c_ei_valid_scclang_global_10 = u_encode_ints_itf.m_valid;
    assign u_encode_ints_itf.m_ready = c_ei_ready_scclang_global_9;
  always @(*) begin
  end

  encode_stream_sc_module_8_interface u_encode_stream_itf();

  encode_stream_sc_module_8 u_encode_stream(
    u_encode_stream_itf
  );
    assign u_encode_stream_itf.clk = itf.clk;
    assign u_encode_stream_itf.reset = itf.reset;
    assign u_encode_stream_itf.minbits = itf.minbits;
    assign u_encode_stream_itf.maxbits = itf.maxbits;
    assign u_encode_stream_itf.maxprec = itf.maxprec;
    assign u_encode_stream_itf.minexp = itf.minexp;
    assign u_encode_stream_itf.s_ex_data = c_sp_ex2_scclang_global_18_data;
    assign u_encode_stream_itf.s_ex_valid = c_sp_ex2_scclang_global_18_valid;
    assign c_sp_ex2_scclang_global_18_ready = u_encode_stream_itf.s_ex_ready;
    assign u_encode_stream_itf.s_bc = c_ei_bc_scclang_global_6;
    assign u_encode_stream_itf.s_bp = c_ei_bp_scclang_global_7;
    assign u_encode_stream_itf.s_last = c_ei_last_scclang_global_8;
    assign u_encode_stream_itf.s_valid = c_ei_valid_scclang_global_10;
    assign c_ei_ready_scclang_global_9 = u_encode_stream_itf.s_ready;
    assign c_es_flush_scclang_global_11 = u_encode_stream_itf.m_flush;
    assign itf.m_bits_data_tdata = u_encode_stream_itf.m_bits_data_tdata;
    assign itf.m_bits_data_tlast = u_encode_stream_itf.m_bits_data_tlast;
    assign itf.m_bits_valid = u_encode_stream_itf.m_bits_valid;
    assign u_encode_stream_itf.m_bits_ready = itf.m_bits_ready;
  always @(*) begin
  end

  genvar i_u_dut_local_4;
  genvar i_u_dut_local_5;
  genvar i_u_dut_local_1;
  genvar i_u_dut_local_3;
  genvar i_u_dut_local_2;
  genvar i_u_dut_local_0;
  for (i_u_dut_local_0 = 0;(i_u_dut_local_0) < (16);i_u_dut_local_0++) begin
    assign c_fc_block_scclang_global_12[i_u_dut_local_0] = u_fwd_cast_itf.m_block[i_u_dut_local_0];
  end
  for (i_u_dut_local_1 = 0;(i_u_dut_local_1) < (16);i_u_dut_local_1++) begin
        assign u_encode_block_itf.s_block[i_u_dut_local_1] = c_fc_block_scclang_global_12[i_u_dut_local_1];
  end
  for (i_u_dut_local_2 = 0;(i_u_dut_local_2) < (16);i_u_dut_local_2++) begin
    assign c_eb_block_scclang_global_3[i_u_dut_local_2] = u_encode_block_itf.m_block[i_u_dut_local_2];
  end
  for (i_u_dut_local_3 = 0;(i_u_dut_local_3) < (16);i_u_dut_local_3++) begin
        assign u_block_buffer_itf.s_block[i_u_dut_local_3] = c_eb_block_scclang_global_3[i_u_dut_local_3];
  end
  for (i_u_dut_local_4 = 0;(i_u_dut_local_4) < (16);i_u_dut_local_4++) begin
    assign c_bb_block_scclang_global_0[i_u_dut_local_4] = u_block_buffer_itf.m_block[i_u_dut_local_4];
  end
  for (i_u_dut_local_5 = 0;(i_u_dut_local_5) < (16);i_u_dut_local_5++) begin
        assign u_encode_ints_itf.s_block[i_u_dut_local_5] = c_bb_block_scclang_global_0[i_u_dut_local_5];
  end
endmodule
module find_emax_sc_module_2 (
 find_emax_sc_module_2_interface.port0 itf
);
  logic [0:0] c_sync_scclang_global_0;
  logic [3:0] count_scclang_global_1;
  logic [10:0] emax_scclang_global_2;
  logic [0:0] emax_v_scclang_global_3;
  logic [51:0] c_fp_scclang_global_4_data_frac;
  logic [10:0] c_fp_scclang_global_4_data_expo;
  logic [0:0] c_fp_scclang_global_4_data_sign;
  logic [0:0] c_fp_scclang_global_4_valid;
  logic [0:0] c_fp_scclang_global_4_ready;
  logic [10:0] c_ex_scclang_global_5_data;
  logic [0:0] c_ex_scclang_global_5_valid;
  logic [0:0] c_ex_scclang_global_5_ready;
  initial begin

  end
  rvfifo_cc_sc_module_9_interface u_que_fp_itf();

  rvfifo_cc_sc_module_9 u_que_fp(
    u_que_fp_itf
  );
    assign u_que_fp_itf.clk = itf.clk;
    assign u_que_fp_itf.reset = itf.reset;
    assign u_que_fp_itf.s_port_data_frac = c_fp_scclang_global_4_data_frac;
    assign u_que_fp_itf.s_port_data_expo = c_fp_scclang_global_4_data_expo;
    assign u_que_fp_itf.s_port_data_sign = c_fp_scclang_global_4_data_sign;
    assign u_que_fp_itf.s_port_valid = c_fp_scclang_global_4_valid;
    assign c_fp_scclang_global_4_ready = u_que_fp_itf.s_port_ready;
    assign itf.m_fp_data_frac = u_que_fp_itf.m_port_data_frac;
    assign itf.m_fp_data_expo = u_que_fp_itf.m_port_data_expo;
    assign itf.m_fp_data_sign = u_que_fp_itf.m_port_data_sign;
    assign itf.m_fp_valid = u_que_fp_itf.m_port_valid;
    assign u_que_fp_itf.m_port_ready = itf.m_fp_ready;
  always @(*) begin
  end

  sreg_sc_module_10_interface u_reg_ex_itf();

  sreg_sc_module_10 u_reg_ex(
    u_reg_ex_itf
  );
    assign u_reg_ex_itf.clk = itf.clk;
    assign u_reg_ex_itf.reset = itf.reset;
    assign u_reg_ex_itf.s_port_data = c_ex_scclang_global_5_data;
    assign u_reg_ex_itf.s_port_valid = c_ex_scclang_global_5_valid;
    assign c_ex_scclang_global_5_ready = u_reg_ex_itf.s_port_ready;
    assign itf.m_ex_data = u_reg_ex_itf.m_port_data;
    assign itf.m_ex_valid = u_reg_ex_itf.m_port_valid;
    assign u_reg_ex_itf.m_port_ready = itf.m_ex_ready;
  always @(*) begin
  end

  always @(itf.s_fp_valid or itf.s_fp_data_frac or itf.s_fp_data_expo or itf.s_fp_data_sign or c_fp_scclang_global_4_ready or c_ex_scclang_global_5_ready or emax_scclang_global_2 or emax_v_scclang_global_3 or c_sync_scclang_global_0) begin: mc_proc
    
    
    itf.s_fp_ready <= c_sync_scclang_global_0;
    c_fp_scclang_global_4_data_frac <= itf.s_fp_data_frac;
    c_fp_scclang_global_4_data_expo <= itf.s_fp_data_expo;
    c_fp_scclang_global_4_data_sign <= itf.s_fp_data_sign;
    c_fp_scclang_global_4_valid <= c_sync_scclang_global_0;
    c_ex_scclang_global_5_data <= emax_scclang_global_2;
    c_ex_scclang_global_5_valid <= emax_v_scclang_global_3;
    c_sync_scclang_global_0 <= ((itf.s_fp_valid) && (c_fp_scclang_global_4_ready)) && ((!(emax_v_scclang_global_3)) || (c_ex_scclang_global_5_ready));
  end
  always_ff @(posedge itf.clk) begin: ms_proc
    logic [0:0] last_ms_proc_local_2;
    logic [51:0] fp_ms_proc_local_3_frac;
    logic [10:0] fp_ms_proc_local_3_expo;
    logic [0:0] fp_ms_proc_local_3_sign;
    logic [10:0] expo_ms_proc_local_4;
    
    if ((itf.reset) == (0)) begin
      count_scclang_global_1 <= (16) - (1);
      emax_scclang_global_2 <= 11'd0;
      emax_v_scclang_global_3 <= 0;
    end else begin
      last_ms_proc_local_2 = (count_scclang_global_1) == (0);
      fp_ms_proc_local_3_frac = itf.s_fp_data_frac;
      fp_ms_proc_local_3_expo = itf.s_fp_data_expo;
      fp_ms_proc_local_3_sign = itf.s_fp_data_sign;
      if (((fp_ms_proc_local_3_expo) == (0)) && ((fp_ms_proc_local_3_frac) == (0))) begin
        expo_ms_proc_local_4 = fp_ms_proc_local_3_expo;
      end else begin
        expo_ms_proc_local_4 = (fp_ms_proc_local_3_expo) + (11'd1);
      end

      if (c_sync_scclang_global_0) begin
        if (last_ms_proc_local_2) begin
          count_scclang_global_1 <= (16) - (1);
        end else begin
          count_scclang_global_1 <= (count_scclang_global_1) - (1);
        end

      end
      if ((emax_v_scclang_global_3) && (c_ex_scclang_global_5_ready)) begin
        if (itf.s_fp_valid) begin
          emax_scclang_global_2 <= expo_ms_proc_local_4;
        end else begin
          emax_scclang_global_2 <= 11'd0;
        end

      end else begin
        if ((itf.s_fp_valid) && ((expo_ms_proc_local_4) > (emax_scclang_global_2))) begin
          emax_scclang_global_2 <= expo_ms_proc_local_4;
        end
      end

      if ((emax_v_scclang_global_3) && (c_ex_scclang_global_5_ready)) begin
        emax_v_scclang_global_3 <= 0;
      end else begin
        if ((c_sync_scclang_global_0) && (last_ms_proc_local_2)) begin
          emax_v_scclang_global_3 <= 1;
        end
      end

    end

  end
endmodule
module rvfifo_cc_sc_module_9 (
 rvfifo_cc_sc_module_9_interface.port0 itf
);
  logic signed[31:0] MAX_DEPTH_scclang_global_0 = 32;
  initial begin

  end
  fifo_cc_sc_module_11_interface u_fifo_itf();

  fifo_cc_sc_module_11 u_fifo(
    u_fifo_itf
  );
    assign u_fifo_itf.clk = itf.clk;
    assign u_fifo_itf.reset = itf.reset;
    assign u_fifo_itf.din_frac = itf.s_port_data_frac;
    assign u_fifo_itf.din_expo = itf.s_port_data_expo;
    assign u_fifo_itf.din_sign = itf.s_port_data_sign;
    assign u_fifo_itf.wr_en = itf.s_port_valid;
    assign itf.s_port_ready = u_fifo_itf.full;
    assign itf.m_port_data_frac = u_fifo_itf.dout_frac;
    assign itf.m_port_data_expo = u_fifo_itf.dout_expo;
    assign itf.m_port_data_sign = u_fifo_itf.dout_sign;
    assign u_fifo_itf.rd_en = itf.m_port_ready;
    assign itf.m_port_valid = u_fifo_itf.empty;
  always @(*) begin
  end

endmodule
module fifo_cc_sc_module_11 (
 fifo_cc_sc_module_11_interface.port0 itf
);
  logic [51:0] data_scclang_global_0_frac[0:31];
  logic [10:0] data_scclang_global_0_expo[0:31];
  logic [0:0] data_scclang_global_0_sign[0:31];
  logic [0:0] empty_i_scclang_global_1;
  logic [0:0] full_i_scclang_global_2;
  logic [0:0] rd_en_i_scclang_global_3;
  logic [4:0] rd_idx_scclang_global_4;
  logic [0:0] wr_en_i_scclang_global_5;
  logic [4:0] wr_idx_scclang_global_6;
  logic [31:0] depth_scclang_global_7;
  logic signed[31:0] MAX_DEPTH_scclang_global_8 = 32;
  initial begin

  end
  always @(itf.wr_en or itf.rd_en or full_i_scclang_global_2 or empty_i_scclang_global_1 or rd_idx_scclang_global_4) begin: mc_proc
    
    
    if (1) begin
      itf.dout_frac <= data_scclang_global_0_frac[rd_idx_scclang_global_4];
      itf.dout_expo <= data_scclang_global_0_expo[rd_idx_scclang_global_4];
      itf.dout_sign <= data_scclang_global_0_sign[rd_idx_scclang_global_4];
    end
    wr_en_i_scclang_global_5 <= (itf.wr_en) && (!(full_i_scclang_global_2));
    rd_en_i_scclang_global_3 <= (itf.rd_en) && (!(empty_i_scclang_global_1));
    itf.full <= (full_i_scclang_global_2) == (0);
    itf.empty <= (empty_i_scclang_global_1) == (0);
  end
  always_ff @(posedge itf.clk) begin: ms_proc
    logic [4:0] wr_inc_ms_proc_local_3;
    logic [4:0] rd_inc_ms_proc_local_4;
    logic [31:0] i_ms_proc_local_5;
    
    wr_inc_ms_proc_local_3 = ((wr_idx_scclang_global_6) + (1)) % (32);
    rd_inc_ms_proc_local_4 = ((rd_idx_scclang_global_4) + (1)) % (32);
    if ((itf.reset) == (0)) begin
      if (!(1)) begin
        itf.dout_frac <= 0;
        itf.dout_expo <= 0;
        itf.dout_sign <= 0;
      end
      for (i_ms_proc_local_5 = 0;(i_ms_proc_local_5) < (32);i_ms_proc_local_5++) begin
        data_scclang_global_0_frac[i_ms_proc_local_5] <= 0;
        data_scclang_global_0_expo[i_ms_proc_local_5] <= 0;
        data_scclang_global_0_sign[i_ms_proc_local_5] <= 0;
      end
      rd_idx_scclang_global_4 <= 5'd0;
      wr_idx_scclang_global_6 <= 5'd0;
      full_i_scclang_global_2 <= 0;
      empty_i_scclang_global_1 <= 1;
    end else begin
      if (!(1)) begin
        if (rd_en_i_scclang_global_3) begin
          itf.dout_frac <= data_scclang_global_0_frac[rd_idx_scclang_global_4];
          itf.dout_expo <= data_scclang_global_0_expo[rd_idx_scclang_global_4];
          itf.dout_sign <= data_scclang_global_0_sign[rd_idx_scclang_global_4];
        end
      end
      if (wr_en_i_scclang_global_5) begin
        data_scclang_global_0_frac[wr_idx_scclang_global_6] <= itf.din_frac;
        data_scclang_global_0_expo[wr_idx_scclang_global_6] <= itf.din_expo;
        data_scclang_global_0_sign[wr_idx_scclang_global_6] <= itf.din_sign;
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
  /*generate*/ for (i_u_fifo_local_1 = 0;(i_u_fifo_local_1) < (32);i_u_fifo_local_1++) begin
    always @(data_scclang_global_0_frac[(i_u_fifo_local_1)]) begin: mc_proc
      
      
      if (1) begin
        itf.dout_frac <= data_scclang_global_0_frac[rd_idx_scclang_global_4];
        itf.dout_expo <= data_scclang_global_0_expo[rd_idx_scclang_global_4];
        itf.dout_sign <= data_scclang_global_0_sign[rd_idx_scclang_global_4];
      end
      wr_en_i_scclang_global_5 <= (itf.wr_en) && (!(full_i_scclang_global_2));
      rd_en_i_scclang_global_3 <= (itf.rd_en) && (!(empty_i_scclang_global_1));
      itf.full <= (full_i_scclang_global_2) == (0);
      itf.empty <= (empty_i_scclang_global_1) == (0);
    end
  end
  /*generate*/ for (i_u_fifo_local_1 = 0;(i_u_fifo_local_1) < (32);i_u_fifo_local_1++) begin
    always @(data_scclang_global_0_expo[(i_u_fifo_local_1)]) begin: mc_proc
      
      
      if (1) begin
        itf.dout_frac <= data_scclang_global_0_frac[rd_idx_scclang_global_4];
        itf.dout_expo <= data_scclang_global_0_expo[rd_idx_scclang_global_4];
        itf.dout_sign <= data_scclang_global_0_sign[rd_idx_scclang_global_4];
      end
      wr_en_i_scclang_global_5 <= (itf.wr_en) && (!(full_i_scclang_global_2));
      rd_en_i_scclang_global_3 <= (itf.rd_en) && (!(empty_i_scclang_global_1));
      itf.full <= (full_i_scclang_global_2) == (0);
      itf.empty <= (empty_i_scclang_global_1) == (0);
    end
  end
  /*generate*/ for (i_u_fifo_local_1 = 0;(i_u_fifo_local_1) < (32);i_u_fifo_local_1++) begin
    always @(data_scclang_global_0_sign[(i_u_fifo_local_1)]) begin: mc_proc
      
      
      if (1) begin
        itf.dout_frac <= data_scclang_global_0_frac[rd_idx_scclang_global_4];
        itf.dout_expo <= data_scclang_global_0_expo[rd_idx_scclang_global_4];
        itf.dout_sign <= data_scclang_global_0_sign[rd_idx_scclang_global_4];
      end
      wr_en_i_scclang_global_5 <= (itf.wr_en) && (!(full_i_scclang_global_2));
      rd_en_i_scclang_global_3 <= (itf.rd_en) && (!(empty_i_scclang_global_1));
      itf.full <= (full_i_scclang_global_2) == (0);
      itf.empty <= (empty_i_scclang_global_1) == (0);
    end
  end
endmodule
module sreg_sc_module_10 (
 sreg_sc_module_10_interface.port0 itf
);
  logic [10:0] data_scclang_global_0[0:1];
  logic [0:0] empty_i_scclang_global_1;
  logic [0:0] full_i_scclang_global_2;
  logic [0:0] rd_en_i_scclang_global_3;
  logic [0:0] rd_idx_scclang_global_4;
  logic [0:0] wr_en_i_scclang_global_5;
  logic [0:0] wr_idx_scclang_global_6;
  logic signed[31:0] IW_scclang_global_7 = 1;
  logic [31:0] depth_scclang_global_8 = 2;
  initial begin

  end
  always @(itf.s_port_valid or itf.m_port_ready or full_i_scclang_global_2 or empty_i_scclang_global_1 or rd_idx_scclang_global_4) begin: mc_proc
    
    
    itf.m_port_data <= data_scclang_global_0[rd_idx_scclang_global_4];
    wr_en_i_scclang_global_5 <= (itf.s_port_valid) && (!(full_i_scclang_global_2));
    rd_en_i_scclang_global_3 <= (itf.m_port_ready) && (!(empty_i_scclang_global_1));
    itf.s_port_ready <= !(full_i_scclang_global_2);
    itf.m_port_valid <= !(empty_i_scclang_global_1);
  end
  always_ff @(posedge itf.clk) begin: ms_proc
    logic [0:0] wr_inc_ms_proc_local_3;
    logic [0:0] rd_inc_ms_proc_local_4;
    logic [31:0] i_ms_proc_local_5;
    
    wr_inc_ms_proc_local_3 = ((wr_idx_scclang_global_6) + (1)) % (2);
    rd_inc_ms_proc_local_4 = ((rd_idx_scclang_global_4) + (1)) % (2);
    if ((itf.reset) == (0)) begin
      for (i_ms_proc_local_5 = 0;(i_ms_proc_local_5) < (2);i_ms_proc_local_5++) begin
        data_scclang_global_0[i_ms_proc_local_5] <= 0;
      end
      rd_idx_scclang_global_4 <= 1'd0;
      wr_idx_scclang_global_6 <= 1'd0;
      full_i_scclang_global_2 <= 0;
      empty_i_scclang_global_1 <= 1;
    end else begin
      if (wr_en_i_scclang_global_5) begin
        data_scclang_global_0[wr_idx_scclang_global_6] <= itf.s_port_data;
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
genvar i_u_reg_ex_local_1;
  /*generate*/ for (i_u_reg_ex_local_1 = 0;(i_u_reg_ex_local_1) < (2);i_u_reg_ex_local_1++) begin
    always @(data_scclang_global_0[(i_u_reg_ex_local_1)]) begin: mc_proc
      
      
      itf.m_port_data <= data_scclang_global_0[rd_idx_scclang_global_4];
      wr_en_i_scclang_global_5 <= (itf.s_port_valid) && (!(full_i_scclang_global_2));
      rd_en_i_scclang_global_3 <= (itf.m_port_ready) && (!(empty_i_scclang_global_1));
      itf.s_port_ready <= !(full_i_scclang_global_2);
      itf.m_port_valid <= !(empty_i_scclang_global_1);
    end
  end
endmodule
module ssplit_sc_module_3 (
 ssplit_sc_module_3_interface.port0 itf
);
  logic [10:0] c_port1_scclang_global_0_data;
  logic [0:0] c_port1_scclang_global_0_valid;
  logic [0:0] c_port1_scclang_global_0_ready;
  logic [10:0] c_port2_scclang_global_1_data;
  logic [0:0] c_port2_scclang_global_1_valid;
  logic [0:0] c_port2_scclang_global_1_ready;
  initial begin

  end
  rvfifo_cc_sc_module_12_interface u_que1_itf();

  rvfifo_cc_sc_module_12 u_que1(
    u_que1_itf
  );
    assign u_que1_itf.clk = itf.clk;
    assign u_que1_itf.reset = itf.reset;
    assign u_que1_itf.s_port_data = c_port1_scclang_global_0_data;
    assign u_que1_itf.s_port_valid = c_port1_scclang_global_0_valid;
    assign c_port1_scclang_global_0_ready = u_que1_itf.s_port_ready;
    assign itf.m_port1_data = u_que1_itf.m_port_data;
    assign itf.m_port1_valid = u_que1_itf.m_port_valid;
    assign u_que1_itf.m_port_ready = itf.m_port1_ready;
  always @(*) begin
  end

  rvfifo_cc_sc_module_13_interface u_que2_itf();

  rvfifo_cc_sc_module_13 u_que2(
    u_que2_itf
  );
    assign u_que2_itf.clk = itf.clk;
    assign u_que2_itf.reset = itf.reset;
    assign u_que2_itf.s_port_data = c_port2_scclang_global_1_data;
    assign u_que2_itf.s_port_valid = c_port2_scclang_global_1_valid;
    assign c_port2_scclang_global_1_ready = u_que2_itf.s_port_ready;
    assign itf.m_port2_data = u_que2_itf.m_port_data;
    assign itf.m_port2_valid = u_que2_itf.m_port_valid;
    assign u_que2_itf.m_port_ready = itf.m_port2_ready;
  always @(*) begin
  end

  always @(itf.s_port_data or itf.s_port_valid or c_port1_scclang_global_0_ready or c_port2_scclang_global_1_ready) begin: mc_proc
    
    
    c_port1_scclang_global_0_data <= itf.s_port_data;
    c_port2_scclang_global_1_data <= itf.s_port_data;
    c_port1_scclang_global_0_valid <= (itf.s_port_valid) && (c_port2_scclang_global_1_ready);
    c_port2_scclang_global_1_valid <= (itf.s_port_valid) && (c_port1_scclang_global_0_ready);
    itf.s_port_ready <= (c_port1_scclang_global_0_ready) && (c_port2_scclang_global_1_ready);
  end
endmodule
module rvfifo_cc_sc_module_12 (
 rvfifo_cc_sc_module_12_interface.port0 itf
);
  logic signed[31:0] MAX_DEPTH_scclang_global_0 = 2;
  initial begin

  end
  fifo_cc_sc_module_14_interface u_fifo_itf();

  fifo_cc_sc_module_14 u_fifo(
    u_fifo_itf
  );
    assign u_fifo_itf.clk = itf.clk;
    assign u_fifo_itf.reset = itf.reset;
    assign u_fifo_itf.din = itf.s_port_data;
    assign u_fifo_itf.wr_en = itf.s_port_valid;
    assign itf.s_port_ready = u_fifo_itf.full;
    assign itf.m_port_data = u_fifo_itf.dout;
    assign u_fifo_itf.rd_en = itf.m_port_ready;
    assign itf.m_port_valid = u_fifo_itf.empty;
  always @(*) begin
  end

endmodule
module fifo_cc_sc_module_14 (
 fifo_cc_sc_module_14_interface.port0 itf
);
  logic [10:0] data_scclang_global_0[0:1];
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
genvar i_u_fifo_local_1;
  /*generate*/ for (i_u_fifo_local_1 = 0;(i_u_fifo_local_1) < (2);i_u_fifo_local_1++) begin
    always @(data_scclang_global_0[(i_u_fifo_local_1)]) begin: mc_proc
      
      
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
module rvfifo_cc_sc_module_13 (
 rvfifo_cc_sc_module_13_interface.port0 itf
);
  logic signed[31:0] MAX_DEPTH_scclang_global_0 = 16;
  initial begin

  end
  fifo_cc_sc_module_15_interface u_fifo_itf();

  fifo_cc_sc_module_15 u_fifo(
    u_fifo_itf
  );
    assign u_fifo_itf.clk = itf.clk;
    assign u_fifo_itf.reset = itf.reset;
    assign u_fifo_itf.din = itf.s_port_data;
    assign u_fifo_itf.wr_en = itf.s_port_valid;
    assign itf.s_port_ready = u_fifo_itf.full;
    assign itf.m_port_data = u_fifo_itf.dout;
    assign u_fifo_itf.rd_en = itf.m_port_ready;
    assign itf.m_port_valid = u_fifo_itf.empty;
  always @(*) begin
  end

endmodule
module fifo_cc_sc_module_15 (
 fifo_cc_sc_module_15_interface.port0 itf
);
  logic [10:0] data_scclang_global_0[0:15];
  logic [0:0] empty_i_scclang_global_1;
  logic [0:0] full_i_scclang_global_2;
  logic [0:0] rd_en_i_scclang_global_3;
  logic [3:0] rd_idx_scclang_global_4;
  logic [0:0] wr_en_i_scclang_global_5;
  logic [3:0] wr_idx_scclang_global_6;
  logic [31:0] depth_scclang_global_7;
  logic signed[31:0] MAX_DEPTH_scclang_global_8 = 16;
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
    logic [3:0] wr_inc_ms_proc_local_3;
    logic [3:0] rd_inc_ms_proc_local_4;
    logic [31:0] i_ms_proc_local_5;
    
    wr_inc_ms_proc_local_3 = ((wr_idx_scclang_global_6) + (1)) % (16);
    rd_inc_ms_proc_local_4 = ((rd_idx_scclang_global_4) + (1)) % (16);
    if ((itf.reset) == (0)) begin
      if (!(1)) begin
        itf.dout <= 0;
      end
      for (i_ms_proc_local_5 = 0;(i_ms_proc_local_5) < (16);i_ms_proc_local_5++) begin
        data_scclang_global_0[i_ms_proc_local_5] <= 0;
      end
      rd_idx_scclang_global_4 <= 4'd0;
      wr_idx_scclang_global_6 <= 4'd0;
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
genvar i_u_fifo_local_1;
  /*generate*/ for (i_u_fifo_local_1 = 0;(i_u_fifo_local_1) < (16);i_u_fifo_local_1++) begin
    always @(data_scclang_global_0[(i_u_fifo_local_1)]) begin: mc_proc
      
      
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
module fwd_cast_sc_module_4 (
 fwd_cast_sc_module_4_interface.port0 itf
);
  logic signed[63:0] c_int_scclang_global_0;
  logic [0:0] c_sync_scclang_global_1;
  logic [3:0] count_scclang_global_2;
  always @(itf.s_ex_valid or itf.s_ex_data or itf.s_fp_valid or itf.s_fp_data_frac or itf.s_fp_data_expo or itf.s_fp_data_sign or itf.m_ready or count_scclang_global_2 or c_sync_scclang_global_1 or c_int_scclang_global_0) begin: mc_proc
    logic [10:0] emax_mc_proc_local_2;
    logic [51:0] fp_mc_proc_local_3_frac;
    logic [10:0] fp_mc_proc_local_3_expo;
    logic [0:0] fp_mc_proc_local_3_sign;
    logic [2:0] hid_mc_proc_local_4;
    logic [63:0] ui_mc_proc_local_5;
    logic signed[63:0] si_mc_proc_local_6;
    
    c_sync_scclang_global_1 <= ((itf.s_ex_valid) && (itf.s_fp_valid)) && (itf.m_ready);
    itf.s_ex_ready <= (c_sync_scclang_global_1) && ((count_scclang_global_2) == ((16) - (1)));
    itf.s_fp_ready <= c_sync_scclang_global_1;
    emax_mc_proc_local_2 = itf.s_ex_data;
    if ((emax_mc_proc_local_2) != (0)) begin
      emax_mc_proc_local_2 = (emax_mc_proc_local_2) - (11'd1);
    end
    fp_mc_proc_local_3_frac = itf.s_fp_data_frac;
    fp_mc_proc_local_3_expo = itf.s_fp_data_expo;
    fp_mc_proc_local_3_sign = itf.s_fp_data_sign;
    hid_mc_proc_local_4 = (fp_mc_proc_local_3_expo) != (0);
    ui_mc_proc_local_5 = ({ ({ (hid_mc_proc_local_4) , (fp_mc_proc_local_3_frac) }) , (9'd0) }) >>> ((emax_mc_proc_local_2) - (fp_mc_proc_local_3_expo));
    if (fp_mc_proc_local_3_sign) begin
      si_mc_proc_local_6 = -(ui_mc_proc_local_5);
    end else begin
      si_mc_proc_local_6 = ui_mc_proc_local_5;
    end

    c_int_scclang_global_0 <= si_mc_proc_local_6;
  end
  always_ff @(posedge itf.clk) begin: ms_proc
    
    
    if ((itf.reset) == (0)) begin
      count_scclang_global_2 <= 4'd0;
      itf.m_valid <= 0;
    end else begin
      if (c_sync_scclang_global_1) begin
        if ((count_scclang_global_2) == ((16) - (1))) begin
          count_scclang_global_2 <= 4'd0;
          itf.m_valid <= 1;
        end else begin
          count_scclang_global_2 <= (count_scclang_global_2) + (1);
          itf.m_valid <= 0;
        end

        itf.m_block[count_scclang_global_2] <= c_int_scclang_global_0;
      end else begin
        if (itf.m_ready) begin
          itf.m_valid <= 0;
        end
      end

    end

  end
endmodule
module encode_block_sc_module_5 (
 encode_block_sc_module_5_interface.port0 itf
);
  logic signed[63:0] c_xt_data_scclang_global_0[0:3][0:3];
  logic [0:0] c_xt_ready_scclang_global_1[0:3];
  logic [0:0] c_xt_valid_scclang_global_2[0:3];
  logic signed[63:0] c_yt_data_scclang_global_3[0:3][0:3];
  logic [0:0] c_yt_ready_scclang_global_4[0:3];
  logic [0:0] c_yt_valid_scclang_global_5[0:3];
  initial begin

  end
  genvar encode_block_sc_module_5_u_xt_0;
  /*generate*/ for (encode_block_sc_module_5_u_xt_0 = 0; encode_block_sc_module_5_u_xt_0 < 4; encode_block_sc_module_5_u_xt_0 = encode_block_sc_module_5_u_xt_0 + 1) begin : u_xt
    fwd_lift_sc_module_16_interface itf();
    fwd_lift_sc_module_16 mod(
      itf
    );
  end

  genvar encode_block_sc_module_5_u_yt_0;
  /*generate*/ for (encode_block_sc_module_5_u_yt_0 = 0; encode_block_sc_module_5_u_yt_0 < 4; encode_block_sc_module_5_u_yt_0 = encode_block_sc_module_5_u_yt_0 + 1) begin : u_yt
    fwd_lift_sc_module_17_interface itf();
    fwd_lift_sc_module_17 mod(
      itf
    );
  end

  always @(itf.m_ready) begin: mc_proc
    
    
    itf.s_ready <= itf.m_ready;
  end
  always_ff @(posedge itf.clk) begin: ms_fwd_order
    
    
    if (itf.m_ready) begin
      itf.m_block[0] <= ((c_yt_data_scclang_global_3[0][0]) + (-64'd6148914691236517206)) ^ (-64'd6148914691236517206);
      itf.m_block[1] <= ((c_yt_data_scclang_global_3[0][1]) + (-64'd6148914691236517206)) ^ (-64'd6148914691236517206);
      itf.m_block[2] <= ((c_yt_data_scclang_global_3[1][0]) + (-64'd6148914691236517206)) ^ (-64'd6148914691236517206);
      itf.m_block[3] <= ((c_yt_data_scclang_global_3[1][1]) + (-64'd6148914691236517206)) ^ (-64'd6148914691236517206);
      itf.m_block[4] <= ((c_yt_data_scclang_global_3[0][2]) + (-64'd6148914691236517206)) ^ (-64'd6148914691236517206);
      itf.m_block[5] <= ((c_yt_data_scclang_global_3[2][0]) + (-64'd6148914691236517206)) ^ (-64'd6148914691236517206);
      itf.m_block[6] <= ((c_yt_data_scclang_global_3[1][2]) + (-64'd6148914691236517206)) ^ (-64'd6148914691236517206);
      itf.m_block[7] <= ((c_yt_data_scclang_global_3[2][1]) + (-64'd6148914691236517206)) ^ (-64'd6148914691236517206);
      itf.m_block[8] <= ((c_yt_data_scclang_global_3[0][3]) + (-64'd6148914691236517206)) ^ (-64'd6148914691236517206);
      itf.m_block[9] <= ((c_yt_data_scclang_global_3[3][0]) + (-64'd6148914691236517206)) ^ (-64'd6148914691236517206);
      itf.m_block[10] <= ((c_yt_data_scclang_global_3[2][2]) + (-64'd6148914691236517206)) ^ (-64'd6148914691236517206);
      itf.m_block[11] <= ((c_yt_data_scclang_global_3[1][3]) + (-64'd6148914691236517206)) ^ (-64'd6148914691236517206);
      itf.m_block[12] <= ((c_yt_data_scclang_global_3[3][1]) + (-64'd6148914691236517206)) ^ (-64'd6148914691236517206);
      itf.m_block[13] <= ((c_yt_data_scclang_global_3[2][3]) + (-64'd6148914691236517206)) ^ (-64'd6148914691236517206);
      itf.m_block[14] <= ((c_yt_data_scclang_global_3[3][2]) + (-64'd6148914691236517206)) ^ (-64'd6148914691236517206);
      itf.m_block[15] <= ((c_yt_data_scclang_global_3[3][3]) + (-64'd6148914691236517206)) ^ (-64'd6148914691236517206);
      itf.m_valid <= c_yt_valid_scclang_global_5[0];
    end
  end
  genvar i_u_encode_block_local_1;
  genvar j_u_encode_block_local_0;
  for (j_u_encode_block_local_0 = 0;(j_u_encode_block_local_0) < (4);j_u_encode_block_local_0++) begin
        assign u_xt[j_u_encode_block_local_0].itf.clk = itf.clk;
        assign u_xt[j_u_encode_block_local_0].itf.reset = itf.reset;
        assign u_xt[j_u_encode_block_local_0].itf.s_valid = itf.s_valid;
    assign c_xt_ready_scclang_global_1[(j_u_encode_block_local_0)] = u_xt[j_u_encode_block_local_0].itf.s_ready;
    assign c_xt_valid_scclang_global_2[(j_u_encode_block_local_0)] = u_xt[j_u_encode_block_local_0].itf.m_valid;
        assign u_xt[j_u_encode_block_local_0].itf.m_ready = itf.m_ready;
        assign u_yt[j_u_encode_block_local_0].itf.clk = itf.clk;
        assign u_yt[j_u_encode_block_local_0].itf.reset = itf.reset;
        assign u_yt[j_u_encode_block_local_0].itf.s_valid = c_xt_valid_scclang_global_2[(0)];
    assign c_yt_ready_scclang_global_4[(j_u_encode_block_local_0)] = u_yt[j_u_encode_block_local_0].itf.s_ready;
    assign c_yt_valid_scclang_global_5[(j_u_encode_block_local_0)] = u_yt[j_u_encode_block_local_0].itf.m_valid;
        assign u_yt[j_u_encode_block_local_0].itf.m_ready = itf.m_ready;
    for (i_u_encode_block_local_1 = 0;(i_u_encode_block_local_1) < (4);i_u_encode_block_local_1++) begin
            assign u_xt[j_u_encode_block_local_0].itf.s_port[i_u_encode_block_local_1] = itf.s_block[((j_u_encode_block_local_0) * (4)) + (i_u_encode_block_local_1)];
      assign c_xt_data_scclang_global_0[j_u_encode_block_local_0][i_u_encode_block_local_1] = u_xt[j_u_encode_block_local_0].itf.m_port[i_u_encode_block_local_1];
            assign u_yt[j_u_encode_block_local_0].itf.s_port[i_u_encode_block_local_1] = c_xt_data_scclang_global_0[i_u_encode_block_local_1][j_u_encode_block_local_0];
      assign c_yt_data_scclang_global_3[i_u_encode_block_local_1][j_u_encode_block_local_0] = u_yt[j_u_encode_block_local_0].itf.m_port[i_u_encode_block_local_1];
    end
  end
endmodule
module fwd_lift_sc_module_16 (
 fwd_lift_sc_module_16_interface.port0 itf
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
        x_scclang_global_2[0] <= ((itf.s_port[0]) + (itf.s_port[3])) >>> (1);
        y_scclang_global_3[0] <= itf.s_port[1];
        z_scclang_global_4[0] <= ((itf.s_port[2]) + (itf.s_port[1])) >>> (1);
        w_scclang_global_1[0] <= itf.s_port[3];
        x_scclang_global_2[1] <= ((x_scclang_global_2[0]) + (z_scclang_global_4[0])) >>> (1);
        y_scclang_global_3[1] <= (y_scclang_global_3[0]) - (z_scclang_global_4[0]);
        z_scclang_global_4[1] <= z_scclang_global_4[0];
        w_scclang_global_1[1] <= (w_scclang_global_1[0]) - (x_scclang_global_2[0]);
        x_scclang_global_2[2] <= x_scclang_global_2[1];
        y_scclang_global_3[2] <= y_scclang_global_3[1];
        z_scclang_global_4[2] <= (z_scclang_global_4[1]) - (x_scclang_global_2[1]);
        w_scclang_global_1[2] <= ((w_scclang_global_1[1]) + (y_scclang_global_3[1])) >>> (1);
        x_scclang_global_2[3] <= x_scclang_global_2[2];
        y_scclang_global_3[3] <= (y_scclang_global_3[2]) - (w_scclang_global_1[2]);
        z_scclang_global_4[3] <= z_scclang_global_4[2];
        w_scclang_global_1[3] <= w_scclang_global_1[2];
        x_scclang_global_2[4] <= x_scclang_global_2[3];
        y_scclang_global_3[4] <= y_scclang_global_3[3];
        z_scclang_global_4[4] <= z_scclang_global_4[3];
        w_scclang_global_1[4] <= (w_scclang_global_1[3]) + ((y_scclang_global_3[3]) >>> (1));
        x_scclang_global_2[5] <= x_scclang_global_2[4];
        y_scclang_global_3[5] <= (y_scclang_global_3[4]) - ((w_scclang_global_1[4]) >>> (1));
        z_scclang_global_4[5] <= z_scclang_global_4[4];
        w_scclang_global_1[5] <= w_scclang_global_1[4];
        v_scclang_global_0 <= ((v_scclang_global_0) << (1)) | (itf.s_valid);
      end
    end

  end
endmodule
module fwd_lift_sc_module_17 (
 fwd_lift_sc_module_17_interface.port0 itf
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
        x_scclang_global_2[0] <= ((itf.s_port[0]) + (itf.s_port[3])) >>> (1);
        y_scclang_global_3[0] <= itf.s_port[1];
        z_scclang_global_4[0] <= ((itf.s_port[2]) + (itf.s_port[1])) >>> (1);
        w_scclang_global_1[0] <= itf.s_port[3];
        x_scclang_global_2[1] <= ((x_scclang_global_2[0]) + (z_scclang_global_4[0])) >>> (1);
        y_scclang_global_3[1] <= (y_scclang_global_3[0]) - (z_scclang_global_4[0]);
        z_scclang_global_4[1] <= z_scclang_global_4[0];
        w_scclang_global_1[1] <= (w_scclang_global_1[0]) - (x_scclang_global_2[0]);
        x_scclang_global_2[2] <= x_scclang_global_2[1];
        y_scclang_global_3[2] <= y_scclang_global_3[1];
        z_scclang_global_4[2] <= (z_scclang_global_4[1]) - (x_scclang_global_2[1]);
        w_scclang_global_1[2] <= ((w_scclang_global_1[1]) + (y_scclang_global_3[1])) >>> (1);
        x_scclang_global_2[3] <= x_scclang_global_2[2];
        y_scclang_global_3[3] <= (y_scclang_global_3[2]) - (w_scclang_global_1[2]);
        z_scclang_global_4[3] <= z_scclang_global_4[2];
        w_scclang_global_1[3] <= w_scclang_global_1[2];
        x_scclang_global_2[4] <= x_scclang_global_2[3];
        y_scclang_global_3[4] <= y_scclang_global_3[3];
        z_scclang_global_4[4] <= z_scclang_global_4[3];
        w_scclang_global_1[4] <= (w_scclang_global_1[3]) + ((y_scclang_global_3[3]) >>> (1));
        x_scclang_global_2[5] <= x_scclang_global_2[4];
        y_scclang_global_3[5] <= (y_scclang_global_3[4]) - ((w_scclang_global_1[4]) >>> (1));
        z_scclang_global_4[5] <= z_scclang_global_4[4];
        w_scclang_global_1[5] <= w_scclang_global_1[4];
        v_scclang_global_0 <= ((v_scclang_global_0) << (1)) | (itf.s_valid);
      end
    end

  end
endmodule
module block_buffer_sc_module_6 (
 block_buffer_sc_module_6_interface.port0 itf
);
  logic [1023:0] c_bi_scclang_global_0;
  logic [1023:0] c_bo_scclang_global_1;
  initial begin

  end
  fifo_cc_sc_module_18_interface u_bbuf_itf();

  fifo_cc_sc_module_18 u_bbuf(
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
module fifo_cc_sc_module_18 (
 fifo_cc_sc_module_18_interface.port0 itf
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
module encode_ints_sc_module_7 (
 encode_ints_sc_module_7_interface.port0 itf
);
  logic [4:0] bc1_scclang_global_0;
  logic [15:0] bp1_scclang_global_1;
  logic [15:0] c_bplane_scclang_global_2[0:63];
  logic [0:0] flush1_scclang_global_3;
  logic [5:0] k0_scclang_global_4;
  logic [0:0] last1_scclang_global_5;
  logic [4:0] n0_scclang_global_6;
  logic [4:0] n1_scclang_global_7;
  logic [0:0] valid1_scclang_global_8;
  logic signed[31:0] planes_scclang_global_9 = 64;
  initial begin

  end
  always @(k0_scclang_global_4 or flush1_scclang_global_3 or itf.m_ready or itf.s_valid or itf.s_flush) begin: mc_proc
    logic [0:0] last_mc_proc_local_3;
    logic [15:0] tmp_mc_proc_local_4;
    logic signed[31:0] j_mc_proc_local_5;
    logic signed[31:0] i_mc_proc_local_6;
    
    last_mc_proc_local_3 = ((k0_scclang_global_4) == ((64) - (1))) || ((itf.s_flush) && ((itf.s_flush) != (flush1_scclang_global_3)));
    for (j_mc_proc_local_5 = 0;(j_mc_proc_local_5) < (64);j_mc_proc_local_5++) begin
      for (i_mc_proc_local_6 = 0;(i_mc_proc_local_6) < (16);i_mc_proc_local_6++) begin
        tmp_mc_proc_local_4[i_mc_proc_local_6] = itf.s_block[i_mc_proc_local_6][j_mc_proc_local_5];
      end
      c_bplane_scclang_global_2[((64) - (1)) - (j_mc_proc_local_5)] <= tmp_mc_proc_local_4;
    end
    itf.s_ready <= ((last_mc_proc_local_3) && (itf.m_ready)) || (!(itf.s_valid));
  end
  always_ff @(posedge itf.clk) begin: ms_proc
    logic [0:0] last_ms_proc_local_7;
    logic [31:0] b_ms_proc_local_8;
    logic [0:0] frst_ms_proc_local_9;
    logic [31:0] i_ms_proc_local_10;
    logic [32:0] tmp_ms_proc_local_11;
    logic [31:0] n_ms_proc_local_12;
    logic [31:0] shiftamt_ms_proc_local_13;
    logic [31:0] n1_shiftamt_ms_proc_local_14;
    logic [31:0] first_after_bc_ms_proc_local_15;
    logic [31:0] last_n_ms_proc_local_16;
    logic [0:0] found_ms_proc_local_17;
    logic [32:0] shiftmask_ms_proc_local_18;
    logic [31:0] delta_ms_proc_local_19;
    logic [32:0] shiftmask_2_ms_proc_local_20;
    logic [31:0] j_ms_proc_local_21;
    
    if ((itf.reset) == (0)) begin
      k0_scclang_global_4 <= 6'd0;
      n0_scclang_global_6 <= 5'd0;
      n1_scclang_global_7 <= 5'd0;
      bc1_scclang_global_0 <= 5'd0;
      itf.m_bc <= 6'd0;
      bp1_scclang_global_1 <= 16'd0;
      itf.m_bp <= 33'd0;
      last1_scclang_global_5 <= 0;
      itf.m_last <= 0;
      valid1_scclang_global_8 <= 0;
      itf.m_valid <= 0;
    end else begin
      if (itf.m_ready) begin
        last_ms_proc_local_7 = ((k0_scclang_global_4) == ((64) - (1))) || ((itf.s_flush) && ((itf.s_flush) != (flush1_scclang_global_3)));
        b_ms_proc_local_8 = 0;
        frst_ms_proc_local_9 = 1;
        for (i_ms_proc_local_10 = 16;(i_ms_proc_local_10) > (0);i_ms_proc_local_10--) begin
          if (c_bplane_scclang_global_2[k0_scclang_global_4][(i_ms_proc_local_10) - (1)]) begin
            if (frst_ms_proc_local_9) begin
              frst_ms_proc_local_9 = 0;
              b_ms_proc_local_8 = i_ms_proc_local_10;
            end
          end
        end
        n1_scclang_global_7 <= n0_scclang_global_6;
        bc1_scclang_global_0 <= b_ms_proc_local_8;
        bp1_scclang_global_1 <= c_bplane_scclang_global_2[k0_scclang_global_4];
        last1_scclang_global_5 <= last_ms_proc_local_7;
        valid1_scclang_global_8 <= itf.s_valid;
        flush1_scclang_global_3 <= itf.s_flush;
        if (itf.s_valid) begin
          if ((b_ms_proc_local_8) > (n0_scclang_global_6)) begin
            n0_scclang_global_6 <= b_ms_proc_local_8;
          end
          if (last_ms_proc_local_7) begin
            k0_scclang_global_4 <= 6'd0;
            n0_scclang_global_6 <= 5'd0;
          end else begin
            k0_scclang_global_4 <= (k0_scclang_global_4) + (1);
          end

        end
        tmp_ms_proc_local_11 = 33'd0;
        shiftamt_ms_proc_local_13 = 0;
        n1_shiftamt_ms_proc_local_14 = 0;
        first_after_bc_ms_proc_local_15 = n1_scclang_global_7;
        last_n_ms_proc_local_16 = n1_scclang_global_7;
        found_ms_proc_local_17 = 0;
        shiftamt_ms_proc_local_13 = 0;
        for (n_ms_proc_local_12 = 0;(n_ms_proc_local_12) < (16);n_ms_proc_local_12++) begin
          if ((n_ms_proc_local_12) == (n1_scclang_global_7)) begin
            n1_shiftamt_ms_proc_local_14 = shiftamt_ms_proc_local_13;
          end
          if ((n_ms_proc_local_12) == (0)) begin
            shiftamt_ms_proc_local_13 = (shiftamt_ms_proc_local_13) + (2);
            if (bp1_scclang_global_1[0]) begin
              tmp_ms_proc_local_11[0] = 1;
              tmp_ms_proc_local_11[1] = 1;
            end else begin
              tmp_ms_proc_local_11[0] = 1;
              tmp_ms_proc_local_11[1] = 0;
            end

          end else begin
            if ((bp1_scclang_global_1[n_ms_proc_local_12]) && (bp1_scclang_global_1[(n_ms_proc_local_12) - (1)])) begin
              tmp_ms_proc_local_11[shiftamt_ms_proc_local_13] = 1;
              tmp_ms_proc_local_11[(shiftamt_ms_proc_local_13) + (1)] = 1;
              shiftamt_ms_proc_local_13 = (shiftamt_ms_proc_local_13) + (2);
            end else begin
              if ((bp1_scclang_global_1[n_ms_proc_local_12]) && (!(bp1_scclang_global_1[(n_ms_proc_local_12) - (1)]))) begin
                if ((n_ms_proc_local_12) != ((16) - (1))) begin
                  tmp_ms_proc_local_11[shiftamt_ms_proc_local_13] = 1;
                  shiftamt_ms_proc_local_13 = (shiftamt_ms_proc_local_13) + (1);
                end
              end else begin
                if ((!(bp1_scclang_global_1[n_ms_proc_local_12])) && (!(bp1_scclang_global_1[(n_ms_proc_local_12) - (1)]))) begin
                  tmp_ms_proc_local_11[shiftamt_ms_proc_local_13] = 0;
                  shiftamt_ms_proc_local_13 = (shiftamt_ms_proc_local_13) + (1);
                end else begin
                  if ((!(bp1_scclang_global_1[n_ms_proc_local_12])) && (bp1_scclang_global_1[(n_ms_proc_local_12) - (1)])) begin
                    tmp_ms_proc_local_11[shiftamt_ms_proc_local_13] = 1;
                    tmp_ms_proc_local_11[(shiftamt_ms_proc_local_13) + (1)] = 0;
                    shiftamt_ms_proc_local_13 = (shiftamt_ms_proc_local_13) + (2);
                  end
                end

              end

            end

          end

          if ((((!(found_ms_proc_local_17)) && (bp1_scclang_global_1[n_ms_proc_local_12])) && ((n_ms_proc_local_12) >= ((bc1_scclang_global_0) - (1)))) && ((n1_scclang_global_7) < (bc1_scclang_global_0))) begin
            found_ms_proc_local_17 = 1;
            if ((bp1_scclang_global_1[n_ms_proc_local_12]) && (!(bp1_scclang_global_1[(n_ms_proc_local_12) - (1)]))) begin
              if ((n_ms_proc_local_12) != ((16) - (1))) begin
                first_after_bc_ms_proc_local_15 = shiftamt_ms_proc_local_13;
              end else begin
                if ((bc1_scclang_global_0) == (16)) begin
                  first_after_bc_ms_proc_local_15 = shiftamt_ms_proc_local_13;
                end else begin
                  first_after_bc_ms_proc_local_15 = (shiftamt_ms_proc_local_13) - (1);
                end

              end

            end else begin
              if ((n_ms_proc_local_12) != ((16) - (1))) begin
                first_after_bc_ms_proc_local_15 = shiftamt_ms_proc_local_13;
              end else begin
                first_after_bc_ms_proc_local_15 = (shiftamt_ms_proc_local_13) - (1);
              end

            end

            last_n_ms_proc_local_16 = (n_ms_proc_local_12) + (1);
          end
        end
        shiftmask_ms_proc_local_18 = -(1);
        if ((first_after_bc_ms_proc_local_15) < (33)) begin
          tmp_ms_proc_local_11 = (tmp_ms_proc_local_11) & (~((shiftmask_ms_proc_local_18) << (first_after_bc_ms_proc_local_15)));
        end
        delta_ms_proc_local_19 = 0;
        if ((last_n_ms_proc_local_16) != (16)) begin
          tmp_ms_proc_local_11[first_after_bc_ms_proc_local_15] = 0;
        end
        if ((n1_scclang_global_7) >= (bc1_scclang_global_0)) begin
          tmp_ms_proc_local_11 = 0;
        end else begin
          if ((n1_scclang_global_7) == (0)) begin

          end else begin
            if (((!(bp1_scclang_global_1[(n1_scclang_global_7)])) && (!(bp1_scclang_global_1[((n1_scclang_global_7) - (1))]))) || ((bp1_scclang_global_1[n1_scclang_global_7]) && (!(bp1_scclang_global_1[((n1_scclang_global_7) - (1))])))) begin
              tmp_ms_proc_local_11 = ((((tmp_ms_proc_local_11) >>> (n1_shiftamt_ms_proc_local_14)) << (1)) | (1)) << (n1_scclang_global_7);
              delta_ms_proc_local_19 = 1;
            end else begin
              tmp_ms_proc_local_11 = ((tmp_ms_proc_local_11) >>> (n1_shiftamt_ms_proc_local_14)) << (n1_scclang_global_7);
            end

          end

        end

        shiftmask_2_ms_proc_local_20 = -(1);
        tmp_ms_proc_local_11 = ((tmp_ms_proc_local_11) & ((shiftmask_2_ms_proc_local_20) << (n1_scclang_global_7))) | ((bp1_scclang_global_1) & (~((shiftmask_2_ms_proc_local_20) << (n1_scclang_global_7))));
        j_ms_proc_local_21 = 0;
        if ((n1_scclang_global_7) >= (bc1_scclang_global_0)) begin
          j_ms_proc_local_21 = n1_scclang_global_7;
          if ((n1_scclang_global_7) != (16)) begin
            j_ms_proc_local_21 = (j_ms_proc_local_21) + (1);
          end
        end else begin
          j_ms_proc_local_21 = (((first_after_bc_ms_proc_local_15) + (delta_ms_proc_local_19)) - (n1_shiftamt_ms_proc_local_14)) + (n1_scclang_global_7);
          if ((last_n_ms_proc_local_16) != (16)) begin
            j_ms_proc_local_21 = (j_ms_proc_local_21) + (1);
          end
        end

        itf.m_bc <= j_ms_proc_local_21;
        itf.m_bp <= tmp_ms_proc_local_11;
        itf.m_last <= last1_scclang_global_5;
        itf.m_valid <= valid1_scclang_global_8;
      end else begin
        if (!(itf.s_flush)) begin
          flush1_scclang_global_3 <= 0;
        end
      end

    end

  end
genvar i_u_encode_ints_local_1;
  /*generate*/ for (i_u_encode_ints_local_1 = 0;(i_u_encode_ints_local_1) < (16);i_u_encode_ints_local_1++) begin
    always @(itf.s_block[(i_u_encode_ints_local_1)]) begin: mc_proc
      logic [0:0] last_mc_proc_local_3;
      logic [15:0] tmp_mc_proc_local_4;
      logic signed[31:0] j_mc_proc_local_5;
      logic signed[31:0] i_mc_proc_local_6;
      
      last_mc_proc_local_3 = ((k0_scclang_global_4) == ((64) - (1))) || ((itf.s_flush) && ((itf.s_flush) != (flush1_scclang_global_3)));
      /*generate*/ for (j_mc_proc_local_5 = 0;(j_mc_proc_local_5) < (64);j_mc_proc_local_5++) begin
        /*generate*/ for (i_mc_proc_local_6 = 0;(i_mc_proc_local_6) < (16);i_mc_proc_local_6++) begin
          tmp_mc_proc_local_4[i_mc_proc_local_6] = itf.s_block[i_mc_proc_local_6][j_mc_proc_local_5];
        end
        c_bplane_scclang_global_2[((64) - (1)) - (j_mc_proc_local_5)] <= tmp_mc_proc_local_4;
      end
      itf.s_ready <= ((last_mc_proc_local_3) && (itf.m_ready)) || (!(itf.s_valid));
    end
  end
endmodule
module encode_stream_sc_module_8 (
 encode_stream_sc_module_8_interface.port0 itf
);
  logic [2:0] cs_scclang_global_0_s;
  logic [6:0] cs_scclang_global_0_prec;
  logic [6:0] cs_scclang_global_0_planes;
  logic [127:0] cs_scclang_global_0_buf;
  logic [11:0] cs_scclang_global_0_bits;
  logic [7:0] cs_scclang_global_0_posr;
  logic [7:0] cs_scclang_global_0_posw;
  logic [2:0] ns_scclang_global_1_s;
  logic [6:0] ns_scclang_global_1_prec;
  logic [6:0] ns_scclang_global_1_planes;
  logic [127:0] ns_scclang_global_1_buf;
  logic [11:0] ns_scclang_global_1_bits;
  logic [7:0] ns_scclang_global_1_posr;
  logic [7:0] ns_scclang_global_1_posw;
  logic signed[31:0] pre_w_scclang_global_2 = 7;
  logic signed[31:0] tbc_w_scclang_global_3 = 12;
  logic signed[31:0] buf_w_scclang_global_4 = 128;
  logic signed[31:0] pos_w_scclang_global_5 = 8;
  logic [11:0] bc_actual_scclang_global_6;
  logic [127:0] bp_actual_scclang_global_7;
  logic [0:0] done_actual_scclang_global_8;
  always @(itf.s_ex_valid or itf.s_ex_data or itf.m_bits_ready or itf.minbits or itf.maxbits or itf.maxprec or itf.minexp or itf.s_bc or itf.s_bp or itf.s_last or itf.s_valid or cs_scclang_global_0_s or cs_scclang_global_0_prec or cs_scclang_global_0_planes or cs_scclang_global_0_buf or cs_scclang_global_0_bits or cs_scclang_global_0_posr or cs_scclang_global_0_posw) begin: mc_proc
    logic [2:0] ts_mc_proc_local_2_s;
    logic [6:0] ts_mc_proc_local_2_prec;
    logic [6:0] ts_mc_proc_local_2_planes;
    logic [127:0] ts_mc_proc_local_2_buf;
    logic [11:0] ts_mc_proc_local_2_bits;
    logic [7:0] ts_mc_proc_local_2_posr;
    logic [7:0] ts_mc_proc_local_2_posw;
    logic [0:0] dis_done_mc_proc_local_3;
    logic [0:0] add_done_mc_proc_local_4;
    logic [0:0] pad_done_mc_proc_local_5;
    logic [0:0] fls_done_mc_proc_local_6;
    logic [11:0] pbits_mc_proc_local_7;
    logic [11:0] abits_mc_proc_local_8;
    logic [11:0] n_mc_proc_local_9;
    logic [7:0] ts_mc_proc_local_2_posw__ref_0;
    logic [6:0] ts_mc_proc_local_2_prec__ref_0;
    logic [127:0] ts_mc_proc_local_2_buf__ref_0;
    logic [2:0] ts_mc_proc_local_2_s__ref_0;
    logic [6:0] ts_mc_proc_local_2_planes__ref_0;
    logic [7:0] ts_mc_proc_local_2_posr__ref_0;
    logic [11:0] ts_mc_proc_local_2_bits__ref_0;
    
    ts_mc_proc_local_2_bits__ref_0 = ts_mc_proc_local_2_bits;
    ts_mc_proc_local_2_posr__ref_0 = ts_mc_proc_local_2_posr;
    ts_mc_proc_local_2_planes__ref_0 = ts_mc_proc_local_2_planes;
    ts_mc_proc_local_2_s__ref_0 = ts_mc_proc_local_2_s;
    ts_mc_proc_local_2_buf__ref_0 = ts_mc_proc_local_2_buf;
    ts_mc_proc_local_2_prec__ref_0 = ts_mc_proc_local_2_prec;
    ts_mc_proc_local_2_posw__ref_0 = ts_mc_proc_local_2_posw;
    itf.s_ex_ready <= 0;
    itf.s_ready <= 0;
    itf.m_flush <= 0;
    itf.m_bits_valid <= 0;
    ts_mc_proc_local_2_s__ref_0 = cs_scclang_global_0_s;
    ts_mc_proc_local_2_prec__ref_0 = cs_scclang_global_0_prec;
    ts_mc_proc_local_2_planes__ref_0 = cs_scclang_global_0_planes;
    ts_mc_proc_local_2_buf__ref_0 = cs_scclang_global_0_buf;
    ts_mc_proc_local_2_bits__ref_0 = cs_scclang_global_0_bits;
    ts_mc_proc_local_2_posr__ref_0 = cs_scclang_global_0_posr;
    ts_mc_proc_local_2_posw__ref_0 = cs_scclang_global_0_posw;
    case(ts_mc_proc_local_2_s__ref_0)
      0: begin
        ts_mc_proc_local_2_prec__ref_0 = ((itf.maxprec) < (((0) < (((itf.s_ex_data) - (itf.minexp)) + ((2) * ((2) + (1)))) ? (((itf.s_ex_data) - (itf.minexp)) + ((2) * ((2) + (1)))) : (0))) ? (itf.maxprec) : (((0) < (((itf.s_ex_data) - (itf.minexp)) + ((2) * ((2) + (1)))) ? (((itf.s_ex_data) - (itf.minexp)) + ((2) * ((2) + (1)))) : (0))));
        ts_mc_proc_local_2_bits__ref_0 = 0;
        ts_mc_proc_local_2_planes__ref_0 = 0;
        ts_mc_proc_local_2_buf__ref_0 = 0;
        ts_mc_proc_local_2_posr__ref_0 = 0;
        ts_mc_proc_local_2_posw__ref_0 = 0;
        if (itf.s_ex_valid) begin
          if (((itf.s_ex_data) != (0)) && ((ts_mc_proc_local_2_prec__ref_0) != (0))) begin
            ts_mc_proc_local_2_s__ref_0 = 2;
          end else begin
            ts_mc_proc_local_2_s__ref_0 = 1;
          end

        end

      end
      1: begin
        ts_mc_proc_local_2_buf__ref_0[0] = 0;
        ts_mc_proc_local_2_bits__ref_0 = 1;
        ts_mc_proc_local_2_posw__ref_0 = 1;
        itf.s_ex_ready <= 1;
        ts_mc_proc_local_2_s__ref_0 = 4;

      end
      2: begin
        ts_mc_proc_local_2_buf__ref_0[0] = 1;
        ts_mc_proc_local_2_buf__ref_0[11:1] = itf.s_ex_data;
        ts_mc_proc_local_2_bits__ref_0 = (11) + (1);
        ts_mc_proc_local_2_posw__ref_0 = (11) + (1);
        itf.s_ex_ready <= 1;
        if ((ts_mc_proc_local_2_planes__ref_0) < (ts_mc_proc_local_2_prec__ref_0)) begin
          ts_mc_proc_local_2_s__ref_0 = 3;
        end else begin
          ts_mc_proc_local_2_s__ref_0 = 4;
        end


      end
      3: begin
        if (itf.s_valid) begin
          if (zhw__encode_streamfp_t11_52_2_bits_t64__pack_bits_func_0(ts_mc_proc_local_2_s__ref_0,ts_mc_proc_local_2_prec__ref_0,ts_mc_proc_local_2_planes__ref_0,ts_mc_proc_local_2_buf__ref_0,ts_mc_proc_local_2_bits__ref_0,ts_mc_proc_local_2_posr__ref_0,ts_mc_proc_local_2_posw__ref_0,itf.s_bc,itf.s_bp,ts_mc_proc_local_2_s__ref_0,ts_mc_proc_local_2_prec__ref_0,ts_mc_proc_local_2_planes__ref_0,ts_mc_proc_local_2_buf__ref_0,ts_mc_proc_local_2_bits__ref_0,ts_mc_proc_local_2_posr__ref_0,ts_mc_proc_local_2_posw__ref_0)) begin
            itf.s_ready <= 1;
            if (itf.s_last) begin
              ts_mc_proc_local_2_planes__ref_0 = 64;
            end else begin
              ts_mc_proc_local_2_planes__ref_0 = (ts_mc_proc_local_2_planes__ref_0) + (1);
            end

          end
        end else begin
          itf.s_ready <= 1;
        end

        dis_done_mc_proc_local_3 = (ts_mc_proc_local_2_planes__ref_0) == (64);
        add_done_mc_proc_local_4 = ((ts_mc_proc_local_2_planes__ref_0) >= (ts_mc_proc_local_2_prec__ref_0)) || ((ts_mc_proc_local_2_bits__ref_0) >= (itf.maxbits));
        pad_done_mc_proc_local_5 = (ts_mc_proc_local_2_bits__ref_0) >= (itf.minbits);
        fls_done_mc_proc_local_6 = zhw__encode_streamfp_t11_52_2_bits_t64__out_bits_func_1(ts_mc_proc_local_2_s__ref_0,ts_mc_proc_local_2_prec__ref_0,ts_mc_proc_local_2_planes__ref_0,ts_mc_proc_local_2_buf__ref_0,ts_mc_proc_local_2_bits__ref_0,ts_mc_proc_local_2_posr__ref_0,ts_mc_proc_local_2_posw__ref_0,(add_done_mc_proc_local_4) && (pad_done_mc_proc_local_5),ts_mc_proc_local_2_s__ref_0,ts_mc_proc_local_2_prec__ref_0,ts_mc_proc_local_2_planes__ref_0,ts_mc_proc_local_2_buf__ref_0,ts_mc_proc_local_2_bits__ref_0,ts_mc_proc_local_2_posr__ref_0,ts_mc_proc_local_2_posw__ref_0);
        if (add_done_mc_proc_local_4) begin
          if (((dis_done_mc_proc_local_3) && (pad_done_mc_proc_local_5)) && (fls_done_mc_proc_local_6)) begin
            ts_mc_proc_local_2_s__ref_0 = 0;
          end else begin
            ts_mc_proc_local_2_s__ref_0 = 4;
          end

        end

      end
      4: begin
        if (itf.s_valid) begin
          if ((ts_mc_proc_local_2_planes__ref_0) < (64)) begin
            if ((ts_mc_proc_local_2_planes__ref_0) < ((64) - (2))) begin
              itf.m_flush <= 1;
            end
            itf.s_ready <= 1;
            if (itf.s_last) begin
              ts_mc_proc_local_2_planes__ref_0 = 64;
            end else begin
              ts_mc_proc_local_2_planes__ref_0 = (ts_mc_proc_local_2_planes__ref_0) + (1);
            end

          end
        end else begin
          itf.s_ready <= 1;
        end

        if ((ts_mc_proc_local_2_bits__ref_0) < (itf.minbits)) begin
          pbits_mc_proc_local_7 = (itf.minbits) - (ts_mc_proc_local_2_bits__ref_0);
          abits_mc_proc_local_8 = (64) - ((ts_mc_proc_local_2_bits__ref_0) & ((64) - (1)));
          n_mc_proc_local_9 = ((pbits_mc_proc_local_7) < (abits_mc_proc_local_8) ? (pbits_mc_proc_local_7) : (abits_mc_proc_local_8));
          zhw__encode_streamfp_t11_52_2_bits_t64__pack_bits_func_0(ts_mc_proc_local_2_s__ref_0,ts_mc_proc_local_2_prec__ref_0,ts_mc_proc_local_2_planes__ref_0,ts_mc_proc_local_2_buf__ref_0,ts_mc_proc_local_2_bits__ref_0,ts_mc_proc_local_2_posr__ref_0,ts_mc_proc_local_2_posw__ref_0,n_mc_proc_local_9,64'd0,ts_mc_proc_local_2_s__ref_0,ts_mc_proc_local_2_prec__ref_0,ts_mc_proc_local_2_planes__ref_0,ts_mc_proc_local_2_buf__ref_0,ts_mc_proc_local_2_bits__ref_0,ts_mc_proc_local_2_posr__ref_0,ts_mc_proc_local_2_posw__ref_0);
        end
        dis_done_mc_proc_local_3 = (ts_mc_proc_local_2_planes__ref_0) == (64);
        pad_done_mc_proc_local_5 = (ts_mc_proc_local_2_bits__ref_0) >= (itf.minbits);
        fls_done_mc_proc_local_6 = zhw__encode_streamfp_t11_52_2_bits_t64__out_bits_func_1(ts_mc_proc_local_2_s__ref_0,ts_mc_proc_local_2_prec__ref_0,ts_mc_proc_local_2_planes__ref_0,ts_mc_proc_local_2_buf__ref_0,ts_mc_proc_local_2_bits__ref_0,ts_mc_proc_local_2_posr__ref_0,ts_mc_proc_local_2_posw__ref_0,pad_done_mc_proc_local_5,ts_mc_proc_local_2_s__ref_0,ts_mc_proc_local_2_prec__ref_0,ts_mc_proc_local_2_planes__ref_0,ts_mc_proc_local_2_buf__ref_0,ts_mc_proc_local_2_bits__ref_0,ts_mc_proc_local_2_posr__ref_0,ts_mc_proc_local_2_posw__ref_0);
        if (((dis_done_mc_proc_local_3) && (pad_done_mc_proc_local_5)) && (fls_done_mc_proc_local_6)) begin
          ts_mc_proc_local_2_s__ref_0 = 0;
        end

      end
    endcase
    ns_scclang_global_1_s <= ts_mc_proc_local_2_s__ref_0;
    ns_scclang_global_1_prec <= ts_mc_proc_local_2_prec__ref_0;
    ns_scclang_global_1_planes <= ts_mc_proc_local_2_planes__ref_0;
    ns_scclang_global_1_buf <= ts_mc_proc_local_2_buf__ref_0;
    ns_scclang_global_1_bits <= ts_mc_proc_local_2_bits__ref_0;
    ns_scclang_global_1_posr <= ts_mc_proc_local_2_posr__ref_0;
    ns_scclang_global_1_posw <= ts_mc_proc_local_2_posw__ref_0;
    ts_mc_proc_local_2_posw = ts_mc_proc_local_2_posw__ref_0;
    ts_mc_proc_local_2_prec = ts_mc_proc_local_2_prec__ref_0;
    ts_mc_proc_local_2_buf = ts_mc_proc_local_2_buf__ref_0;
    ts_mc_proc_local_2_s = ts_mc_proc_local_2_s__ref_0;
    ts_mc_proc_local_2_planes = ts_mc_proc_local_2_planes__ref_0;
    ts_mc_proc_local_2_posr = ts_mc_proc_local_2_posr__ref_0;
    ts_mc_proc_local_2_bits = ts_mc_proc_local_2_bits__ref_0;
  end
  always_ff @(posedge itf.clk) begin: ms_proc
    
    
    if ((itf.reset) == (0)) begin
      cs_scclang_global_0_s <= 0;
      cs_scclang_global_0_prec <= 0;
      cs_scclang_global_0_planes <= 0;
      cs_scclang_global_0_buf <= 0;
      cs_scclang_global_0_bits <= 0;
      cs_scclang_global_0_posr <= 0;
      cs_scclang_global_0_posw <= 0;
    end else begin
      cs_scclang_global_0_s <= ns_scclang_global_1_s;
      cs_scclang_global_0_prec <= ns_scclang_global_1_prec;
      cs_scclang_global_0_planes <= ns_scclang_global_1_planes;
      cs_scclang_global_0_buf <= ns_scclang_global_1_buf;
      cs_scclang_global_0_bits <= ns_scclang_global_1_bits;
      cs_scclang_global_0_posr <= ns_scclang_global_1_posr;
      cs_scclang_global_0_posw <= ns_scclang_global_1_posw;
    end

  end
  function automatic logic [0:0] zhw__encode_streamfp_t11_52_2_bits_t64__pack_bits_func_0 (input logic [2:0] ts_s, input logic [6:0] ts_prec, input logic [6:0] ts_planes, input logic [127:0] ts_buf, input logic [11:0] ts_bits, input logic [7:0] ts_posr, input logic [7:0] ts_posw, input logic [11:0] bc, input logic [127:0] bp, inout logic [2:0] ts_s__ref_0, inout logic [6:0] ts_prec__ref_0, inout logic [6:0] ts_planes__ref_0, inout logic [127:0] ts_buf__ref_0, inout logic [11:0] ts_bits__ref_0, inout logic [7:0] ts_posr__ref_0, inout logic [7:0] ts_posw__ref_0);
  begin
    logic [11:0] n__local_10;
    bc_actual_scclang_global_6 = bc;
    bp_actual_scclang_global_7 = bp;
    n__local_10 = ((bc_actual_scclang_global_6) < ((itf.maxbits) - (ts_bits)) ? (bc_actual_scclang_global_6) : ((itf.maxbits) - (ts_bits)));
    if (((128) - (ts_posw)) >= (n__local_10)) begin
      ts_buf__ref_0 = ((((bp_actual_scclang_global_7) >> (0)) & ~(~($bits(bp_actual_scclang_global_7)'('b0)) << (((n__local_10) - (1)) - (0) + 1)) & ~(~($bits(bp_actual_scclang_global_7)'('b0)) << ((((ts_posw) + (n__local_10)) - (1))-(ts_posw)+1))) << (ts_posw)) | ((ts_buf__ref_0) & ((~($bits(ts_buf__ref_0)'('b0)) ) << ((((ts_posw) + (n__local_10)) - (1)) + 1) | ~(( ~($bits(ts_buf__ref_0)'('b0)) ) << (ts_posw))));
      ts_bits__ref_0 = (ts_bits) + (n__local_10);
      ts_posw__ref_0 = (ts_posw) + (n__local_10);
      return 1;
    end
    return 0;
  end
  endfunction
  function automatic logic [0:0] zhw__encode_streamfp_t11_52_2_bits_t64__out_bits_func_1 (input logic [2:0] ts_s, input logic [6:0] ts_prec, input logic [6:0] ts_planes, input logic [127:0] ts_buf, input logic [11:0] ts_bits, input logic [7:0] ts_posr, input logic [7:0] ts_posw, input logic [0:0] done, inout logic [2:0] ts_s__ref_0, inout logic [6:0] ts_prec__ref_0, inout logic [6:0] ts_planes__ref_0, inout logic [127:0] ts_buf__ref_0, inout logic [11:0] ts_bits__ref_0, inout logic [7:0] ts_posr__ref_0, inout logic [7:0] ts_posw__ref_0);
  begin
    logic [7:0] bbits__local_11;
    logic [63:0] flit__local_12_tdata;
    logic [0:0] flit__local_12_tlast;
    done_actual_scclang_global_8 = done;
    bbits__local_11 = (ts_posw) - (ts_posr);
    if (((bbits__local_11) >= (64)) && (itf.m_bits_ready)) begin
      flit__local_12_tdata = ((ts_buf__ref_0) >> (ts_posr)) & ~(~($bits(ts_buf__ref_0)'('b0)) << ((((ts_posr) + (64)) - (1)) - (ts_posr) + 1));
      flit__local_12_tlast = (done_actual_scclang_global_8) && ((bbits__local_11) == (64));
      itf.m_bits_data_tdata <= flit__local_12_tdata;
      itf.m_bits_data_tlast <= flit__local_12_tlast;
      itf.m_bits_valid <= 1;
      ts_buf__ref_0 = ((((ts_buf__ref_0) >> ((ts_posr) + (64))) & ~(~($bits(ts_buf__ref_0)'('b0)) << (((ts_posw) - (1)) - ((ts_posr) + (64)) + 1)) & ~(~($bits(ts_buf__ref_0)'('b0)) << (((ts_posw) - (64))-(0)+1))) << (0)) | ((ts_buf__ref_0) & ((~($bits(ts_buf__ref_0)'('b0)) ) << (((ts_posw) - (64)) + 1) | ~(( ~($bits(ts_buf__ref_0)'('b0)) ) << (0))));
      ts_posr__ref_0 = 0;
      ts_posw__ref_0 = (bbits__local_11) - (64);
      return flit__local_12_tlast;
    end
    return (done_actual_scclang_global_8) && ((bbits__local_11) == (0));
  end
  endfunction
endmodule