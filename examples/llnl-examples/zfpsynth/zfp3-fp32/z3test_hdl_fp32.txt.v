module mymodule_sc_module_0 (
);
  logic [0:0] clk_scclang_global_0;
  logic [15:0] maxbits_scclang_global_1;
  logic [15:0] maxprec_scclang_global_2;
  logic [15:0] minbits_scclang_global_3;
  logic signed[15:0] minexp_scclang_global_4;
  logic [0:0] reset_scclang_global_5;
  logic [22:0] c_driver_fp_scclang_global_6_data_frac;
  logic [7:0] c_driver_fp_scclang_global_6_data_expo;
  logic [0:0] c_driver_fp_scclang_global_6_data_sign;
  logic [0:0] c_driver_fp_scclang_global_6_valid;
  logic [0:0] c_driver_fp_scclang_global_6_ready;
  logic [31:0] c_dut_enc_scclang_global_7_data_tdata;
  logic [0:0] c_dut_enc_scclang_global_7_data_tlast;
  logic [0:0] c_dut_enc_scclang_global_7_valid;
  logic [0:0] c_dut_enc_scclang_global_7_ready;
  initial begin
    minbits_scclang_global_3 = 1024;
    maxbits_scclang_global_1 = 1024;
    maxprec_scclang_global_2 = 32;
    minexp_scclang_global_4 = ((1) - (127)) - (23);
  end
  encode_sc_module_1 u_dut(
    .clk(clk_scclang_global_0),
    .reset(reset_scclang_global_5),
    .minbits(minbits_scclang_global_3),
    .maxbits(maxbits_scclang_global_1),
    .maxprec(maxprec_scclang_global_2),
    .minexp(minexp_scclang_global_4),
    .s_fp_data_frac(c_driver_fp_scclang_global_6_data_frac),
    .s_fp_data_expo(c_driver_fp_scclang_global_6_data_expo),
    .s_fp_data_sign(c_driver_fp_scclang_global_6_data_sign),
    .s_fp_valid(c_driver_fp_scclang_global_6_valid),
    .s_fp_ready(c_driver_fp_scclang_global_6_ready),
    .m_bits_data_tdata(c_dut_enc_scclang_global_7_data_tdata),
    .m_bits_data_tlast(c_dut_enc_scclang_global_7_data_tlast),
    .m_bits_valid(c_dut_enc_scclang_global_7_valid),
    .m_bits_ready(c_dut_enc_scclang_global_7_ready)
  );
  always @(*) begin
  end

endmodule
module encode_sc_module_1 (
  input logic [0:0] clk,
  input logic [0:0] reset,
  input logic [15:0] minbits,
  input logic [15:0] maxbits,
  input logic [15:0] maxprec,
  input logic signed[15:0] minexp,
  input logic [22:0] s_fp_data_frac,
  input logic [7:0] s_fp_data_expo,
  input logic [0:0] s_fp_data_sign,
  input logic [0:0] s_fp_valid,
  output logic [0:0] s_fp_ready,
  output logic [31:0] m_bits_data_tdata,
  output logic [0:0] m_bits_data_tlast,
  output logic [0:0] m_bits_valid,
  input logic [0:0] m_bits_ready
);
  logic [31:0] c_bb_block_scclang_global_0[0:15];
  logic [0:0] c_bb_ready_scclang_global_1;
  logic [0:0] c_bb_valid_scclang_global_2;
  logic [31:0] c_eb_block_scclang_global_3[0:15];
  logic [0:0] c_eb_ready_scclang_global_4;
  logic [0:0] c_eb_valid_scclang_global_5;
  logic [5:0] c_ei_bc_scclang_global_6;
  logic [32:0] c_ei_bp_scclang_global_7;
  logic [0:0] c_ei_last_scclang_global_8;
  logic [0:0] c_ei_ready_scclang_global_9;
  logic [0:0] c_ei_valid_scclang_global_10;
  logic [0:0] c_es_flush_scclang_global_11;
  logic signed[31:0] c_fc_block_scclang_global_12[0:15];
  logic [0:0] c_fc_ready_scclang_global_13;
  logic [0:0] c_fc_valid_scclang_global_14;
  logic [22:0] c_fe_fp_scclang_global_15_data_frac;
  logic [7:0] c_fe_fp_scclang_global_15_data_expo;
  logic [0:0] c_fe_fp_scclang_global_15_data_sign;
  logic [0:0] c_fe_fp_scclang_global_15_valid;
  logic [0:0] c_fe_fp_scclang_global_15_ready;
  logic [7:0] c_fe_ex_scclang_global_16_data;
  logic [0:0] c_fe_ex_scclang_global_16_valid;
  logic [0:0] c_fe_ex_scclang_global_16_ready;
  logic [7:0] c_sp_ex1_scclang_global_17_data;
  logic [0:0] c_sp_ex1_scclang_global_17_valid;
  logic [0:0] c_sp_ex1_scclang_global_17_ready;
  logic [7:0] c_sp_ex2_scclang_global_18_data;
  logic [0:0] c_sp_ex2_scclang_global_18_valid;
  logic [0:0] c_sp_ex2_scclang_global_18_ready;
  find_emax_sc_module_2 u_find_emax(
    .clk(clk),
    .reset(reset),
    .s_fp_data_frac(s_fp_data_frac),
    .s_fp_data_expo(s_fp_data_expo),
    .s_fp_data_sign(s_fp_data_sign),
    .s_fp_valid(s_fp_valid),
    .s_fp_ready(s_fp_ready),
    .m_fp_data_frac(c_fe_fp_scclang_global_15_data_frac),
    .m_fp_data_expo(c_fe_fp_scclang_global_15_data_expo),
    .m_fp_data_sign(c_fe_fp_scclang_global_15_data_sign),
    .m_fp_valid(c_fe_fp_scclang_global_15_valid),
    .m_fp_ready(c_fe_fp_scclang_global_15_ready),
    .m_ex_data(c_fe_ex_scclang_global_16_data),
    .m_ex_valid(c_fe_ex_scclang_global_16_valid),
    .m_ex_ready(c_fe_ex_scclang_global_16_ready)
  );
  always @(*) begin
  end

  ssplit_sc_module_3 u_ssplit_ex(
    .clk(clk),
    .reset(reset),
    .s_port_data(c_fe_ex_scclang_global_16_data),
    .s_port_valid(c_fe_ex_scclang_global_16_valid),
    .s_port_ready(c_fe_ex_scclang_global_16_ready),
    .m_port1_data(c_sp_ex1_scclang_global_17_data),
    .m_port1_valid(c_sp_ex1_scclang_global_17_valid),
    .m_port1_ready(c_sp_ex1_scclang_global_17_ready),
    .m_port2_data(c_sp_ex2_scclang_global_18_data),
    .m_port2_valid(c_sp_ex2_scclang_global_18_valid),
    .m_port2_ready(c_sp_ex2_scclang_global_18_ready)
  );
  always @(*) begin
  end

  fwd_cast_sc_module_4 u_fwd_cast(
    .clk(clk),
    .reset(reset),
    .s_fp_data_frac(c_fe_fp_scclang_global_15_data_frac),
    .s_fp_data_expo(c_fe_fp_scclang_global_15_data_expo),
    .s_fp_data_sign(c_fe_fp_scclang_global_15_data_sign),
    .s_fp_valid(c_fe_fp_scclang_global_15_valid),
    .s_fp_ready(c_fe_fp_scclang_global_15_ready),
    .s_ex_data(c_sp_ex1_scclang_global_17_data),
    .s_ex_valid(c_sp_ex1_scclang_global_17_valid),
    .s_ex_ready(c_sp_ex1_scclang_global_17_ready),
    .m_valid(c_fc_valid_scclang_global_14),
    .m_ready(c_fc_ready_scclang_global_13),
    .m_block('{ c_fc_block_scclang_global_12[0],c_fc_block_scclang_global_12[1],c_fc_block_scclang_global_12[2],c_fc_block_scclang_global_12[3],c_fc_block_scclang_global_12[4],c_fc_block_scclang_global_12[5],c_fc_block_scclang_global_12[6],c_fc_block_scclang_global_12[7],c_fc_block_scclang_global_12[8],c_fc_block_scclang_global_12[9],c_fc_block_scclang_global_12[10],c_fc_block_scclang_global_12[11],c_fc_block_scclang_global_12[12],c_fc_block_scclang_global_12[13],c_fc_block_scclang_global_12[14],c_fc_block_scclang_global_12[15] })
  );
  always @(*) begin
  end

  encode_block_sc_module_5 u_encode_block(
    .clk(clk),
    .reset(reset),
    .s_valid(c_fc_valid_scclang_global_14),
    .s_ready(c_fc_ready_scclang_global_13),
    .m_valid(c_eb_valid_scclang_global_5),
    .m_ready(c_eb_ready_scclang_global_4),
    .s_block('{ c_fc_block_scclang_global_12[0],c_fc_block_scclang_global_12[1],c_fc_block_scclang_global_12[2],c_fc_block_scclang_global_12[3],c_fc_block_scclang_global_12[4],c_fc_block_scclang_global_12[5],c_fc_block_scclang_global_12[6],c_fc_block_scclang_global_12[7],c_fc_block_scclang_global_12[8],c_fc_block_scclang_global_12[9],c_fc_block_scclang_global_12[10],c_fc_block_scclang_global_12[11],c_fc_block_scclang_global_12[12],c_fc_block_scclang_global_12[13],c_fc_block_scclang_global_12[14],c_fc_block_scclang_global_12[15] }),
    .m_block('{ c_eb_block_scclang_global_3[0],c_eb_block_scclang_global_3[1],c_eb_block_scclang_global_3[2],c_eb_block_scclang_global_3[3],c_eb_block_scclang_global_3[4],c_eb_block_scclang_global_3[5],c_eb_block_scclang_global_3[6],c_eb_block_scclang_global_3[7],c_eb_block_scclang_global_3[8],c_eb_block_scclang_global_3[9],c_eb_block_scclang_global_3[10],c_eb_block_scclang_global_3[11],c_eb_block_scclang_global_3[12],c_eb_block_scclang_global_3[13],c_eb_block_scclang_global_3[14],c_eb_block_scclang_global_3[15] })
  );
  always @(*) begin
  end

  block_buffer_sc_module_6 u_block_buffer(
    .clk(clk),
    .reset(reset),
    .s_valid(c_eb_valid_scclang_global_5),
    .s_ready(c_eb_ready_scclang_global_4),
    .m_valid(c_bb_valid_scclang_global_2),
    .m_ready(c_bb_ready_scclang_global_1),
    .s_block('{ c_eb_block_scclang_global_3[0],c_eb_block_scclang_global_3[1],c_eb_block_scclang_global_3[2],c_eb_block_scclang_global_3[3],c_eb_block_scclang_global_3[4],c_eb_block_scclang_global_3[5],c_eb_block_scclang_global_3[6],c_eb_block_scclang_global_3[7],c_eb_block_scclang_global_3[8],c_eb_block_scclang_global_3[9],c_eb_block_scclang_global_3[10],c_eb_block_scclang_global_3[11],c_eb_block_scclang_global_3[12],c_eb_block_scclang_global_3[13],c_eb_block_scclang_global_3[14],c_eb_block_scclang_global_3[15] }),
    .m_block('{ c_bb_block_scclang_global_0[0],c_bb_block_scclang_global_0[1],c_bb_block_scclang_global_0[2],c_bb_block_scclang_global_0[3],c_bb_block_scclang_global_0[4],c_bb_block_scclang_global_0[5],c_bb_block_scclang_global_0[6],c_bb_block_scclang_global_0[7],c_bb_block_scclang_global_0[8],c_bb_block_scclang_global_0[9],c_bb_block_scclang_global_0[10],c_bb_block_scclang_global_0[11],c_bb_block_scclang_global_0[12],c_bb_block_scclang_global_0[13],c_bb_block_scclang_global_0[14],c_bb_block_scclang_global_0[15] })
  );
  always @(*) begin
  end

  encode_ints_sc_module_7 u_encode_ints(
    .clk(clk),
    .reset(reset),
    .s_valid(c_bb_valid_scclang_global_2),
    .s_ready(c_bb_ready_scclang_global_1),
    .s_flush(c_es_flush_scclang_global_11),
    .m_bc(c_ei_bc_scclang_global_6),
    .m_bp(c_ei_bp_scclang_global_7),
    .m_last(c_ei_last_scclang_global_8),
    .m_valid(c_ei_valid_scclang_global_10),
    .m_ready(c_ei_ready_scclang_global_9),
    .s_block('{ c_bb_block_scclang_global_0[0],c_bb_block_scclang_global_0[1],c_bb_block_scclang_global_0[2],c_bb_block_scclang_global_0[3],c_bb_block_scclang_global_0[4],c_bb_block_scclang_global_0[5],c_bb_block_scclang_global_0[6],c_bb_block_scclang_global_0[7],c_bb_block_scclang_global_0[8],c_bb_block_scclang_global_0[9],c_bb_block_scclang_global_0[10],c_bb_block_scclang_global_0[11],c_bb_block_scclang_global_0[12],c_bb_block_scclang_global_0[13],c_bb_block_scclang_global_0[14],c_bb_block_scclang_global_0[15] })
  );
  always @(*) begin
  end

  encode_stream_sc_module_8 u_encode_stream(
    .clk(clk),
    .reset(reset),
    .minbits(minbits),
    .maxbits(maxbits),
    .maxprec(maxprec),
    .minexp(minexp),
    .s_ex_data(c_sp_ex2_scclang_global_18_data),
    .s_ex_valid(c_sp_ex2_scclang_global_18_valid),
    .s_ex_ready(c_sp_ex2_scclang_global_18_ready),
    .s_bc(c_ei_bc_scclang_global_6),
    .s_bp(c_ei_bp_scclang_global_7),
    .s_last(c_ei_last_scclang_global_8),
    .s_valid(c_ei_valid_scclang_global_10),
    .s_ready(c_ei_ready_scclang_global_9),
    .m_flush(c_es_flush_scclang_global_11),
    .m_bits_data_tdata(m_bits_data_tdata),
    .m_bits_data_tlast(m_bits_data_tlast),
    .m_bits_valid(m_bits_valid),
    .m_bits_ready(m_bits_ready)
  );
  always @(*) begin
  end

endmodule
module find_emax_sc_module_2 (
  input logic [0:0] clk,
  input logic [0:0] reset,
  input logic [22:0] s_fp_data_frac,
  input logic [7:0] s_fp_data_expo,
  input logic [0:0] s_fp_data_sign,
  input logic [0:0] s_fp_valid,
  output logic [0:0] s_fp_ready,
  output logic [22:0] m_fp_data_frac,
  output logic [7:0] m_fp_data_expo,
  output logic [0:0] m_fp_data_sign,
  output logic [0:0] m_fp_valid,
  input logic [0:0] m_fp_ready,
  output logic [7:0] m_ex_data,
  output logic [0:0] m_ex_valid,
  input logic [0:0] m_ex_ready
);
  logic [0:0] c_sync_scclang_global_0;
  logic [3:0] count_scclang_global_1;
  logic [7:0] emax_scclang_global_2;
  logic [0:0] emax_v_scclang_global_3;
  logic [22:0] c_fp_scclang_global_4_data_frac;
  logic [7:0] c_fp_scclang_global_4_data_expo;
  logic [0:0] c_fp_scclang_global_4_data_sign;
  logic [0:0] c_fp_scclang_global_4_valid;
  logic [0:0] c_fp_scclang_global_4_ready;
  logic [7:0] c_ex_scclang_global_5_data;
  logic [0:0] c_ex_scclang_global_5_valid;
  logic [0:0] c_ex_scclang_global_5_ready;
  rvfifo_cc_sc_module_9 u_que_fp(
    .clk(clk),
    .reset(reset),
    .s_port_data_frac(c_fp_scclang_global_4_data_frac),
    .s_port_data_expo(c_fp_scclang_global_4_data_expo),
    .s_port_data_sign(c_fp_scclang_global_4_data_sign),
    .s_port_valid(c_fp_scclang_global_4_valid),
    .s_port_ready(c_fp_scclang_global_4_ready),
    .m_port_data_frac(m_fp_data_frac),
    .m_port_data_expo(m_fp_data_expo),
    .m_port_data_sign(m_fp_data_sign),
    .m_port_valid(m_fp_valid),
    .m_port_ready(m_fp_ready)
  );
  always @(*) begin
  end

  sreg_sc_module_10 u_reg_ex(
    .clk(clk),
    .reset(reset),
    .s_port_data(c_ex_scclang_global_5_data),
    .s_port_valid(c_ex_scclang_global_5_valid),
    .s_port_ready(c_ex_scclang_global_5_ready),
    .m_port_data(m_ex_data),
    .m_port_valid(m_ex_valid),
    .m_port_ready(m_ex_ready)
  );
  always @(*) begin
  end

  always @(s_fp_valid or s_fp_data_frac or s_fp_data_expo or s_fp_data_sign or c_fp_scclang_global_4_ready or c_ex_scclang_global_5_ready or emax_scclang_global_2 or emax_v_scclang_global_3 or c_sync_scclang_global_0) begin: mc_proc
    
    
    s_fp_ready <= c_sync_scclang_global_0;
    c_fp_scclang_global_4_data_frac <= s_fp_data_frac;
    c_fp_scclang_global_4_data_expo <= s_fp_data_expo;
    c_fp_scclang_global_4_data_sign <= s_fp_data_sign;
    c_fp_scclang_global_4_valid <= c_sync_scclang_global_0;
    c_ex_scclang_global_5_data <= emax_scclang_global_2;
    c_ex_scclang_global_5_valid <= emax_v_scclang_global_3;
    c_sync_scclang_global_0 <= ((s_fp_valid) && (c_fp_scclang_global_4_ready)) && ((!(emax_v_scclang_global_3)) || (c_ex_scclang_global_5_ready));
  end
  always_ff @(posedge clk) begin: ms_proc
    logic [0:0] last_ms_proc_local_2;
    logic [22:0] fp_ms_proc_local_3_frac;
    logic [7:0] fp_ms_proc_local_3_expo;
    logic [0:0] fp_ms_proc_local_3_sign;
    logic [7:0] expo_ms_proc_local_4;
    logic [0:0] fp_ms_proc_local_3_sign__ref_0;
    logic [22:0] fp_ms_proc_local_3_frac__ref_0;
    logic [7:0] __phantom_var_1__ref_0;
    logic [7:0] fp_ms_proc_local_3_expo__ref_0;
    logic [0:0] __phantom_var_2__ref_0;
    logic [22:0] __phantom_var_0__ref_0;
    logic [22:0] __phantom_var_0;
    logic [7:0] __phantom_var_1;
    logic [0:0] __phantom_var_2;
    
    __phantom_var_0__ref_0 = __phantom_var_0;
    __phantom_var_2__ref_0 = __phantom_var_2;
    fp_ms_proc_local_3_expo__ref_0 = fp_ms_proc_local_3_expo;
    __phantom_var_1__ref_0 = __phantom_var_1;
    fp_ms_proc_local_3_frac__ref_0 = fp_ms_proc_local_3_frac;
    fp_ms_proc_local_3_sign__ref_0 = fp_ms_proc_local_3_sign;
    if ((reset) == (0)) begin
      count_scclang_global_1 <= (16) - (1);
      emax_scclang_global_2 <= 8'd0;
      emax_v_scclang_global_3 <= 0;
    end else begin
      last_ms_proc_local_2 = (count_scclang_global_1) == (0);
      fp_t8_23__fp_t_func_0(fp_ms_proc_local_3_frac__ref_0,fp_ms_proc_local_3_expo__ref_0,fp_ms_proc_local_3_sign__ref_0,s_fp_data_frac,s_fp_data_expo,s_fp_data_sign,fp_ms_proc_local_3_frac__ref_0,fp_ms_proc_local_3_expo__ref_0,fp_ms_proc_local_3_sign__ref_0,__phantom_var_0__ref_0,__phantom_var_1__ref_0,__phantom_var_2__ref_0);
      if (((fp_ms_proc_local_3_expo__ref_0) == (0)) && ((fp_ms_proc_local_3_frac__ref_0) == (0))) begin
        expo_ms_proc_local_4 = fp_ms_proc_local_3_expo__ref_0;
      end else begin
        expo_ms_proc_local_4 = (fp_ms_proc_local_3_expo__ref_0) + (8'd1);
      end

      if (c_sync_scclang_global_0) begin
        if (last_ms_proc_local_2) begin
          count_scclang_global_1 <= (16) - (1);
        end else begin
          count_scclang_global_1 <= (count_scclang_global_1) - (1);
        end

      end
      if ((emax_v_scclang_global_3) && (c_ex_scclang_global_5_ready)) begin
        if (s_fp_valid) begin
          emax_scclang_global_2 <= expo_ms_proc_local_4;
        end else begin
          emax_scclang_global_2 <= 8'd0;
        end

      end else begin
        if ((s_fp_valid) && ((expo_ms_proc_local_4) > (emax_scclang_global_2))) begin
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

    fp_ms_proc_local_3_sign = fp_ms_proc_local_3_sign__ref_0;
    fp_ms_proc_local_3_frac = fp_ms_proc_local_3_frac__ref_0;
    __phantom_var_1 = __phantom_var_1__ref_0;
    fp_ms_proc_local_3_expo = fp_ms_proc_local_3_expo__ref_0;
    __phantom_var_2 = __phantom_var_2__ref_0;
    __phantom_var_0 = __phantom_var_0__ref_0;
  end
  function automatic void fp_t8_23__fp_t_func_0 (input logic [22:0] hthis_frac, input logic [7:0] hthis_expo, input logic [0:0] hthis_sign, input logic [22:0] NONAME_frac, input logic [7:0] NONAME_expo, input logic [0:0] NONAME_sign, inout logic [22:0] hthis_frac__ref_0, inout logic [7:0] hthis_expo__ref_0, inout logic [0:0] hthis_sign__ref_0, inout logic [22:0] NONAME_frac__ref_0, inout logic [7:0] NONAME_expo__ref_0, inout logic [0:0] NONAME_sign__ref_0);
  begin


  end
  endfunction
endmodule
module rvfifo_cc_sc_module_9 (
  input logic [0:0] clk,
  input logic [0:0] reset,
  input logic [22:0] s_port_data_frac,
  input logic [7:0] s_port_data_expo,
  input logic [0:0] s_port_data_sign,
  input logic [0:0] s_port_valid,
  output logic [0:0] s_port_ready,
  output logic [22:0] m_port_data_frac,
  output logic [7:0] m_port_data_expo,
  output logic [0:0] m_port_data_sign,
  output logic [0:0] m_port_valid,
  input logic [0:0] m_port_ready
);
  logic signed[31:0] MAX_DEPTH_scclang_global_0 = 32;
  fifo_cc_sc_module_11 u_fifo(
    .clk(clk),
    .reset(reset),
    .din_frac(s_port_data_frac),
    .din_expo(s_port_data_expo),
    .din_sign(s_port_data_sign),
    .wr_en(s_port_valid),
    .full(s_port_ready),
    .dout_frac(m_port_data_frac),
    .dout_expo(m_port_data_expo),
    .dout_sign(m_port_data_sign),
    .rd_en(m_port_ready),
    .empty(m_port_valid)
  );
  always @(*) begin
  end

endmodule
module fifo_cc_sc_module_11 (
  input logic [0:0] clk,
  input logic [0:0] reset,
  input logic [22:0] din_frac,
  input logic [7:0] din_expo,
  input logic [0:0] din_sign,
  input logic [0:0] wr_en,
  input logic [0:0] rd_en,
  output logic [0:0] full,
  output logic [22:0] dout_frac,
  output logic [7:0] dout_expo,
  output logic [0:0] dout_sign,
  output logic [0:0] empty
);
  logic [22:0] data_scclang_global_0_frac[0:31];
  logic [7:0] data_scclang_global_0_expo[0:31];
  logic [0:0] data_scclang_global_0_sign[0:31];
  logic [0:0] empty_i_scclang_global_1;
  logic [0:0] full_i_scclang_global_2;
  logic [0:0] rd_en_i_scclang_global_3;
  logic [4:0] rd_idx_scclang_global_4;
  logic [0:0] wr_en_i_scclang_global_5;
  logic [4:0] wr_idx_scclang_global_6;
  logic signed[31:0] MAX_DEPTH_scclang_global_7 = 32;
  initial begin
    if ((32) <= (0)) begin
    end
  end
  always @(wr_en or rd_en or full_i_scclang_global_2 or empty_i_scclang_global_1 or rd_idx_scclang_global_4 or data_scclang_global_0_frac[(0)] or data_scclang_global_0_expo[(0)] or data_scclang_global_0_sign[(0)] or data_scclang_global_0_frac[(1)] or data_scclang_global_0_expo[(1)] or data_scclang_global_0_sign[(1)] or data_scclang_global_0_frac[(2)] or data_scclang_global_0_expo[(2)] or data_scclang_global_0_sign[(2)] or data_scclang_global_0_frac[(3)] or data_scclang_global_0_expo[(3)] or data_scclang_global_0_sign[(3)] or data_scclang_global_0_frac[(4)] or data_scclang_global_0_expo[(4)] or data_scclang_global_0_sign[(4)] or data_scclang_global_0_frac[(5)] or data_scclang_global_0_expo[(5)] or data_scclang_global_0_sign[(5)] or data_scclang_global_0_frac[(6)] or data_scclang_global_0_expo[(6)] or data_scclang_global_0_sign[(6)] or data_scclang_global_0_frac[(7)] or data_scclang_global_0_expo[(7)] or data_scclang_global_0_sign[(7)] or data_scclang_global_0_frac[(8)] or data_scclang_global_0_expo[(8)] or data_scclang_global_0_sign[(8)] or data_scclang_global_0_frac[(9)] or data_scclang_global_0_expo[(9)] or data_scclang_global_0_sign[(9)] or data_scclang_global_0_frac[(10)] or data_scclang_global_0_expo[(10)] or data_scclang_global_0_sign[(10)] or data_scclang_global_0_frac[(11)] or data_scclang_global_0_expo[(11)] or data_scclang_global_0_sign[(11)] or data_scclang_global_0_frac[(12)] or data_scclang_global_0_expo[(12)] or data_scclang_global_0_sign[(12)] or data_scclang_global_0_frac[(13)] or data_scclang_global_0_expo[(13)] or data_scclang_global_0_sign[(13)] or data_scclang_global_0_frac[(14)] or data_scclang_global_0_expo[(14)] or data_scclang_global_0_sign[(14)] or data_scclang_global_0_frac[(15)] or data_scclang_global_0_expo[(15)] or data_scclang_global_0_sign[(15)] or data_scclang_global_0_frac[(16)] or data_scclang_global_0_expo[(16)] or data_scclang_global_0_sign[(16)] or data_scclang_global_0_frac[(17)] or data_scclang_global_0_expo[(17)] or data_scclang_global_0_sign[(17)] or data_scclang_global_0_frac[(18)] or data_scclang_global_0_expo[(18)] or data_scclang_global_0_sign[(18)] or data_scclang_global_0_frac[(19)] or data_scclang_global_0_expo[(19)] or data_scclang_global_0_sign[(19)] or data_scclang_global_0_frac[(20)] or data_scclang_global_0_expo[(20)] or data_scclang_global_0_sign[(20)] or data_scclang_global_0_frac[(21)] or data_scclang_global_0_expo[(21)] or data_scclang_global_0_sign[(21)] or data_scclang_global_0_frac[(22)] or data_scclang_global_0_expo[(22)] or data_scclang_global_0_sign[(22)] or data_scclang_global_0_frac[(23)] or data_scclang_global_0_expo[(23)] or data_scclang_global_0_sign[(23)] or data_scclang_global_0_frac[(24)] or data_scclang_global_0_expo[(24)] or data_scclang_global_0_sign[(24)] or data_scclang_global_0_frac[(25)] or data_scclang_global_0_expo[(25)] or data_scclang_global_0_sign[(25)] or data_scclang_global_0_frac[(26)] or data_scclang_global_0_expo[(26)] or data_scclang_global_0_sign[(26)] or data_scclang_global_0_frac[(27)] or data_scclang_global_0_expo[(27)] or data_scclang_global_0_sign[(27)] or data_scclang_global_0_frac[(28)] or data_scclang_global_0_expo[(28)] or data_scclang_global_0_sign[(28)] or data_scclang_global_0_frac[(29)] or data_scclang_global_0_expo[(29)] or data_scclang_global_0_sign[(29)] or data_scclang_global_0_frac[(30)] or data_scclang_global_0_expo[(30)] or data_scclang_global_0_sign[(30)] or data_scclang_global_0_frac[(31)] or data_scclang_global_0_expo[(31)] or data_scclang_global_0_sign[(31)]) begin: mc_proc
    
    
    if (1) begin
      dout_frac <= data_scclang_global_0_frac[rd_idx_scclang_global_4];
      dout_expo <= data_scclang_global_0_expo[rd_idx_scclang_global_4];
      dout_sign <= data_scclang_global_0_sign[rd_idx_scclang_global_4];
    end
    wr_en_i_scclang_global_5 <= (wr_en) && (!(full_i_scclang_global_2));
    rd_en_i_scclang_global_3 <= (rd_en) && (!(empty_i_scclang_global_1));
    full <= (full_i_scclang_global_2) == (0);
    empty <= (empty_i_scclang_global_1) == (0);
  end
  always_ff @(posedge clk) begin: ms_proc
    logic [4:0] wr_inc_ms_proc_local_3;
    logic [4:0] rd_inc_ms_proc_local_4;
    logic [31:0] i_ms_proc_local_5;
    
    wr_inc_ms_proc_local_3 = ((wr_idx_scclang_global_6) + (1)) % (32);
    rd_inc_ms_proc_local_4 = ((rd_idx_scclang_global_4) + (1)) % (32);
    if ((reset) == (0)) begin
      if (!(1)) begin
        dout_frac <= 0;
        dout_expo <= 0;
        dout_sign <= 0;
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
          dout_frac <= data_scclang_global_0_frac[rd_idx_scclang_global_4];
          dout_expo <= data_scclang_global_0_expo[rd_idx_scclang_global_4];
          dout_sign <= data_scclang_global_0_sign[rd_idx_scclang_global_4];
        end
      end
      if (wr_en_i_scclang_global_5) begin
        data_scclang_global_0_frac[wr_idx_scclang_global_6] <= din_frac;
        data_scclang_global_0_expo[wr_idx_scclang_global_6] <= din_expo;
        data_scclang_global_0_sign[wr_idx_scclang_global_6] <= din_sign;
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
endmodule
module sreg_sc_module_10 (
  input logic [0:0] clk,
  input logic [0:0] reset,
  input logic [7:0] s_port_data,
  input logic [0:0] s_port_valid,
  output logic [0:0] s_port_ready,
  output logic [7:0] m_port_data,
  output logic [0:0] m_port_valid,
  input logic [0:0] m_port_ready
);
  logic [7:0] data_scclang_global_0[0:1];
  logic [0:0] empty_i_scclang_global_1;
  logic [0:0] full_i_scclang_global_2;
  logic [0:0] rd_en_i_scclang_global_3;
  logic [0:0] rd_idx_scclang_global_4;
  logic [0:0] wr_en_i_scclang_global_5;
  logic [0:0] wr_idx_scclang_global_6;
  logic signed[31:0] IW_scclang_global_7 = 1;
  logic [31:0] depth_scclang_global_8 = 2;
  always @(s_port_valid or m_port_ready or full_i_scclang_global_2 or empty_i_scclang_global_1 or rd_idx_scclang_global_4 or data_scclang_global_0[(0)] or data_scclang_global_0[(1)]) begin: mc_proc
    
    
    m_port_data <= data_scclang_global_0[rd_idx_scclang_global_4];
    wr_en_i_scclang_global_5 <= (s_port_valid) && (!(full_i_scclang_global_2));
    rd_en_i_scclang_global_3 <= (m_port_ready) && (!(empty_i_scclang_global_1));
    s_port_ready <= !(full_i_scclang_global_2);
    m_port_valid <= !(empty_i_scclang_global_1);
  end
  always_ff @(posedge clk) begin: ms_proc
    logic [0:0] wr_inc_ms_proc_local_3;
    logic [0:0] rd_inc_ms_proc_local_4;
    logic [31:0] i_ms_proc_local_5;
    
    wr_inc_ms_proc_local_3 = ((wr_idx_scclang_global_6) + (1)) % (2);
    rd_inc_ms_proc_local_4 = ((rd_idx_scclang_global_4) + (1)) % (2);
    if ((reset) == (0)) begin
      for (i_ms_proc_local_5 = 0;(i_ms_proc_local_5) < (2);i_ms_proc_local_5++) begin
        data_scclang_global_0[i_ms_proc_local_5] <= 0;
      end
      rd_idx_scclang_global_4 <= 1'd0;
      wr_idx_scclang_global_6 <= 1'd0;
      full_i_scclang_global_2 <= 0;
      empty_i_scclang_global_1 <= 1;
    end else begin
      if (wr_en_i_scclang_global_5) begin
        data_scclang_global_0[wr_idx_scclang_global_6] <= s_port_data;
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
endmodule
module ssplit_sc_module_3 (
  input logic [0:0] clk,
  input logic [0:0] reset,
  input logic [7:0] s_port_data,
  input logic [0:0] s_port_valid,
  output logic [0:0] s_port_ready,
  output logic [7:0] m_port1_data,
  output logic [0:0] m_port1_valid,
  input logic [0:0] m_port1_ready,
  output logic [7:0] m_port2_data,
  output logic [0:0] m_port2_valid,
  input logic [0:0] m_port2_ready
);
  logic [7:0] c_port1_scclang_global_0_data;
  logic [0:0] c_port1_scclang_global_0_valid;
  logic [0:0] c_port1_scclang_global_0_ready;
  logic [7:0] c_port2_scclang_global_1_data;
  logic [0:0] c_port2_scclang_global_1_valid;
  logic [0:0] c_port2_scclang_global_1_ready;
  rvfifo_cc_sc_module_12 u_que1(
    .clk(clk),
    .reset(reset),
    .s_port_data(c_port1_scclang_global_0_data),
    .s_port_valid(c_port1_scclang_global_0_valid),
    .s_port_ready(c_port1_scclang_global_0_ready),
    .m_port_data(m_port1_data),
    .m_port_valid(m_port1_valid),
    .m_port_ready(m_port1_ready)
  );
  always @(*) begin
  end

  rvfifo_cc_sc_module_13 u_que2(
    .clk(clk),
    .reset(reset),
    .s_port_data(c_port2_scclang_global_1_data),
    .s_port_valid(c_port2_scclang_global_1_valid),
    .s_port_ready(c_port2_scclang_global_1_ready),
    .m_port_data(m_port2_data),
    .m_port_valid(m_port2_valid),
    .m_port_ready(m_port2_ready)
  );
  always @(*) begin
  end

  always @(s_port_data or s_port_valid or c_port1_scclang_global_0_ready or c_port2_scclang_global_1_ready) begin: mc_proc
    
    
    c_port1_scclang_global_0_data <= s_port_data;
    c_port2_scclang_global_1_data <= s_port_data;
    c_port1_scclang_global_0_valid <= (s_port_valid) && (c_port2_scclang_global_1_ready);
    c_port2_scclang_global_1_valid <= (s_port_valid) && (c_port1_scclang_global_0_ready);
    s_port_ready <= (c_port1_scclang_global_0_ready) && (c_port2_scclang_global_1_ready);
  end
endmodule
module rvfifo_cc_sc_module_12 (
  input logic [0:0] clk,
  input logic [0:0] reset,
  input logic [7:0] s_port_data,
  input logic [0:0] s_port_valid,
  output logic [0:0] s_port_ready,
  output logic [7:0] m_port_data,
  output logic [0:0] m_port_valid,
  input logic [0:0] m_port_ready
);
  logic signed[31:0] MAX_DEPTH_scclang_global_0 = 2;
  fifo_cc_sc_module_14 u_fifo(
    .clk(clk),
    .reset(reset),
    .din(s_port_data),
    .wr_en(s_port_valid),
    .full(s_port_ready),
    .dout(m_port_data),
    .rd_en(m_port_ready),
    .empty(m_port_valid)
  );
  always @(*) begin
  end

endmodule
module fifo_cc_sc_module_14 (
  input logic [0:0] clk,
  input logic [0:0] reset,
  input logic [7:0] din,
  input logic [0:0] wr_en,
  input logic [0:0] rd_en,
  output logic [0:0] full,
  output logic [7:0] dout,
  output logic [0:0] empty
);
  logic [7:0] data_scclang_global_0[0:1];
  logic [0:0] empty_i_scclang_global_1;
  logic [0:0] full_i_scclang_global_2;
  logic [0:0] rd_en_i_scclang_global_3;
  logic [0:0] rd_idx_scclang_global_4;
  logic [0:0] wr_en_i_scclang_global_5;
  logic [0:0] wr_idx_scclang_global_6;
  logic signed[31:0] MAX_DEPTH_scclang_global_7 = 2;
  initial begin
    if ((2) <= (0)) begin
    end
  end
  always @(wr_en or rd_en or full_i_scclang_global_2 or empty_i_scclang_global_1 or rd_idx_scclang_global_4 or data_scclang_global_0[(0)] or data_scclang_global_0[(1)]) begin: mc_proc
    
    
    if (1) begin
      dout <= data_scclang_global_0[rd_idx_scclang_global_4];
    end
    wr_en_i_scclang_global_5 <= (wr_en) && (!(full_i_scclang_global_2));
    rd_en_i_scclang_global_3 <= (rd_en) && (!(empty_i_scclang_global_1));
    full <= (full_i_scclang_global_2) == (0);
    empty <= (empty_i_scclang_global_1) == (0);
  end
  always_ff @(posedge clk) begin: ms_proc
    logic [0:0] wr_inc_ms_proc_local_3;
    logic [0:0] rd_inc_ms_proc_local_4;
    logic [31:0] i_ms_proc_local_5;
    
    wr_inc_ms_proc_local_3 = ((wr_idx_scclang_global_6) + (1)) % (2);
    rd_inc_ms_proc_local_4 = ((rd_idx_scclang_global_4) + (1)) % (2);
    if ((reset) == (0)) begin
      if (!(1)) begin
        dout <= 0;
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
          dout <= data_scclang_global_0[rd_idx_scclang_global_4];
        end
      end
      if (wr_en_i_scclang_global_5) begin
        data_scclang_global_0[wr_idx_scclang_global_6] <= din;
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
endmodule
module rvfifo_cc_sc_module_13 (
  input logic [0:0] clk,
  input logic [0:0] reset,
  input logic [7:0] s_port_data,
  input logic [0:0] s_port_valid,
  output logic [0:0] s_port_ready,
  output logic [7:0] m_port_data,
  output logic [0:0] m_port_valid,
  input logic [0:0] m_port_ready
);
  logic signed[31:0] MAX_DEPTH_scclang_global_0 = 16;
  fifo_cc_sc_module_15 u_fifo(
    .clk(clk),
    .reset(reset),
    .din(s_port_data),
    .wr_en(s_port_valid),
    .full(s_port_ready),
    .dout(m_port_data),
    .rd_en(m_port_ready),
    .empty(m_port_valid)
  );
  always @(*) begin
  end

endmodule
module fifo_cc_sc_module_15 (
  input logic [0:0] clk,
  input logic [0:0] reset,
  input logic [7:0] din,
  input logic [0:0] wr_en,
  input logic [0:0] rd_en,
  output logic [0:0] full,
  output logic [7:0] dout,
  output logic [0:0] empty
);
  logic [7:0] data_scclang_global_0[0:15];
  logic [0:0] empty_i_scclang_global_1;
  logic [0:0] full_i_scclang_global_2;
  logic [0:0] rd_en_i_scclang_global_3;
  logic [3:0] rd_idx_scclang_global_4;
  logic [0:0] wr_en_i_scclang_global_5;
  logic [3:0] wr_idx_scclang_global_6;
  logic signed[31:0] MAX_DEPTH_scclang_global_7 = 16;
  initial begin
    if ((16) <= (0)) begin
    end
  end
  always @(wr_en or rd_en or full_i_scclang_global_2 or empty_i_scclang_global_1 or rd_idx_scclang_global_4 or data_scclang_global_0[(0)] or data_scclang_global_0[(1)] or data_scclang_global_0[(2)] or data_scclang_global_0[(3)] or data_scclang_global_0[(4)] or data_scclang_global_0[(5)] or data_scclang_global_0[(6)] or data_scclang_global_0[(7)] or data_scclang_global_0[(8)] or data_scclang_global_0[(9)] or data_scclang_global_0[(10)] or data_scclang_global_0[(11)] or data_scclang_global_0[(12)] or data_scclang_global_0[(13)] or data_scclang_global_0[(14)] or data_scclang_global_0[(15)]) begin: mc_proc
    
    
    if (1) begin
      dout <= data_scclang_global_0[rd_idx_scclang_global_4];
    end
    wr_en_i_scclang_global_5 <= (wr_en) && (!(full_i_scclang_global_2));
    rd_en_i_scclang_global_3 <= (rd_en) && (!(empty_i_scclang_global_1));
    full <= (full_i_scclang_global_2) == (0);
    empty <= (empty_i_scclang_global_1) == (0);
  end
  always_ff @(posedge clk) begin: ms_proc
    logic [3:0] wr_inc_ms_proc_local_3;
    logic [3:0] rd_inc_ms_proc_local_4;
    logic [31:0] i_ms_proc_local_5;
    
    wr_inc_ms_proc_local_3 = ((wr_idx_scclang_global_6) + (1)) % (16);
    rd_inc_ms_proc_local_4 = ((rd_idx_scclang_global_4) + (1)) % (16);
    if ((reset) == (0)) begin
      if (!(1)) begin
        dout <= 0;
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
          dout <= data_scclang_global_0[rd_idx_scclang_global_4];
        end
      end
      if (wr_en_i_scclang_global_5) begin
        data_scclang_global_0[wr_idx_scclang_global_6] <= din;
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
endmodule
module fwd_cast_sc_module_4 (
  input logic [0:0] clk,
  input logic [0:0] reset,
  input logic [0:0] m_ready,
  input logic [7:0] s_ex_data,
  input logic [0:0] s_ex_valid,
  output logic [0:0] s_ex_ready,
  input logic [22:0] s_fp_data_frac,
  input logic [7:0] s_fp_data_expo,
  input logic [0:0] s_fp_data_sign,
  input logic [0:0] s_fp_valid,
  output logic [0:0] s_fp_ready,
  output logic signed[31:0] m_block[0:15],
  output logic [0:0] m_valid
);
  logic signed[31:0] c_int_scclang_global_0;
  logic [0:0] c_sync_scclang_global_1;
  logic [3:0] count_scclang_global_2;
  always @(s_ex_valid or s_ex_data or s_fp_valid or s_fp_data_frac or s_fp_data_expo or s_fp_data_sign or m_ready or count_scclang_global_2 or c_sync_scclang_global_1 or c_int_scclang_global_0) begin: mc_proc
    logic [7:0] emax_mc_proc_local_2;
    logic [22:0] fp_mc_proc_local_3_frac;
    logic [7:0] fp_mc_proc_local_3_expo;
    logic [0:0] fp_mc_proc_local_3_sign;
    logic [2:0] hid_mc_proc_local_4;
    logic [31:0] ui_mc_proc_local_5;
    logic signed[31:0] si_mc_proc_local_6;
    logic [0:0] fp_mc_proc_local_3_sign__ref_0;
    logic [22:0] fp_mc_proc_local_3_frac__ref_0;
    logic [0:0] __phantom_var_4__ref_0;
    logic [7:0] fp_mc_proc_local_3_expo__ref_0;
    logic [7:0] __phantom_var_3__ref_0;
    logic [22:0] __phantom_var_2__ref_0;
    logic [22:0] __phantom_var_2;
    logic [7:0] __phantom_var_3;
    logic [0:0] __phantom_var_4;
    
    __phantom_var_2__ref_0 = __phantom_var_2;
    __phantom_var_3__ref_0 = __phantom_var_3;
    fp_mc_proc_local_3_expo__ref_0 = fp_mc_proc_local_3_expo;
    __phantom_var_4__ref_0 = __phantom_var_4;
    fp_mc_proc_local_3_frac__ref_0 = fp_mc_proc_local_3_frac;
    fp_mc_proc_local_3_sign__ref_0 = fp_mc_proc_local_3_sign;
    c_sync_scclang_global_1 <= ((s_ex_valid) && (s_fp_valid)) && (m_ready);
    s_ex_ready <= (c_sync_scclang_global_1) && ((count_scclang_global_2) == ((16) - (1)));
    s_fp_ready <= c_sync_scclang_global_1;
    emax_mc_proc_local_2 = s_ex_data;
    if ((emax_mc_proc_local_2) != (0)) begin
      emax_mc_proc_local_2 = (emax_mc_proc_local_2) - (8'd1);
    end
    fp_t8_23__fp_t_func_0(fp_mc_proc_local_3_frac__ref_0,fp_mc_proc_local_3_expo__ref_0,fp_mc_proc_local_3_sign__ref_0,s_fp_data_frac,s_fp_data_expo,s_fp_data_sign,fp_mc_proc_local_3_frac__ref_0,fp_mc_proc_local_3_expo__ref_0,fp_mc_proc_local_3_sign__ref_0,__phantom_var_2__ref_0,__phantom_var_3__ref_0,__phantom_var_4__ref_0);
    hid_mc_proc_local_4 = (fp_mc_proc_local_3_expo__ref_0) != (0);
    ui_mc_proc_local_5 = ({ ({ (hid_mc_proc_local_4) , (fp_mc_proc_local_3_frac__ref_0) }) , (6'd0) }) >>> ((emax_mc_proc_local_2) - (fp_mc_proc_local_3_expo__ref_0));
    if (fp_mc_proc_local_3_sign__ref_0) begin
      si_mc_proc_local_6 = -(ui_mc_proc_local_5);
    end else begin
      si_mc_proc_local_6 = ui_mc_proc_local_5;
    end

    c_int_scclang_global_0 <= si_mc_proc_local_6;
    fp_mc_proc_local_3_sign = fp_mc_proc_local_3_sign__ref_0;
    fp_mc_proc_local_3_frac = fp_mc_proc_local_3_frac__ref_0;
    __phantom_var_4 = __phantom_var_4__ref_0;
    fp_mc_proc_local_3_expo = fp_mc_proc_local_3_expo__ref_0;
    __phantom_var_3 = __phantom_var_3__ref_0;
    __phantom_var_2 = __phantom_var_2__ref_0;
  end
  always_ff @(posedge clk) begin: ms_proc
    
    
    if ((reset) == (0)) begin
      count_scclang_global_2 <= 4'd0;
      m_valid <= 0;
    end else begin
      if (c_sync_scclang_global_1) begin
        if ((count_scclang_global_2) == ((16) - (1))) begin
          count_scclang_global_2 <= 4'd0;
          m_valid <= 1;
        end else begin
          count_scclang_global_2 <= (count_scclang_global_2) + (1);
          m_valid <= 0;
        end

        m_block[count_scclang_global_2] = c_int_scclang_global_0;
      end else begin
        if (m_ready) begin
          m_valid <= 0;
        end
      end

    end

  end
  function automatic void fp_t8_23__fp_t_func_0 (input logic [22:0] hthis_frac, input logic [7:0] hthis_expo, input logic [0:0] hthis_sign, input logic [22:0] NONAME_frac, input logic [7:0] NONAME_expo, input logic [0:0] NONAME_sign, inout logic [22:0] hthis_frac__ref_0, inout logic [7:0] hthis_expo__ref_0, inout logic [0:0] hthis_sign__ref_0, inout logic [22:0] NONAME_frac__ref_0, inout logic [7:0] NONAME_expo__ref_0, inout logic [0:0] NONAME_sign__ref_0);
  begin


  end
  endfunction
endmodule
module encode_block_sc_module_5 (
  input logic [0:0] clk,
  input logic [0:0] reset,
  input logic signed[31:0] s_block[0:15],
  input logic [0:0] s_valid,
  input logic [0:0] m_ready,
  output logic [0:0] s_ready,
  output logic [31:0] m_block[0:15],
  output logic [0:0] m_valid
);
  logic signed[31:0] c_xt_data_scclang_global_0[0:3][0:3];
  logic [0:0] c_xt_ready_scclang_global_1[0:3];
  logic [0:0] c_xt_valid_scclang_global_2[0:3];
  logic signed[31:0] c_yt_data_scclang_global_3[0:3][0:3];
  logic [0:0] c_yt_ready_scclang_global_4[0:3];
  logic [0:0] c_yt_valid_scclang_global_5[0:3];
  fwd_lift_sc_module_16 u_xt_0(
    .clk(clk),
    .reset(reset),
    .s_valid(s_valid),
    .s_ready(c_xt_ready_scclang_global_1[0]),
    .m_valid(c_xt_valid_scclang_global_2[0]),
    .m_ready(m_ready),
    .s_port('{ s_block[((0) * (4)) + (0)],s_block[((0) * (4)) + (1)],s_block[((0) * (4)) + (2)],s_block[((0) * (4)) + (3)] }),
    .m_port('{ c_xt_data_scclang_global_0[0][0],c_xt_data_scclang_global_0[0][1],c_xt_data_scclang_global_0[0][2],c_xt_data_scclang_global_0[0][3] })
  );
  always @(*) begin
  end

  fwd_lift_sc_module_16 u_xt_1(
    .clk(clk),
    .reset(reset),
    .s_valid(s_valid),
    .s_ready(c_xt_ready_scclang_global_1[1]),
    .m_valid(c_xt_valid_scclang_global_2[1]),
    .m_ready(m_ready),
    .s_port('{ s_block[((1) * (4)) + (0)],s_block[((1) * (4)) + (1)],s_block[((1) * (4)) + (2)],s_block[((1) * (4)) + (3)] }),
    .m_port('{ c_xt_data_scclang_global_0[1][0],c_xt_data_scclang_global_0[1][1],c_xt_data_scclang_global_0[1][2],c_xt_data_scclang_global_0[1][3] })
  );
  always @(*) begin
  end

  fwd_lift_sc_module_16 u_xt_2(
    .clk(clk),
    .reset(reset),
    .s_valid(s_valid),
    .s_ready(c_xt_ready_scclang_global_1[2]),
    .m_valid(c_xt_valid_scclang_global_2[2]),
    .m_ready(m_ready),
    .s_port('{ s_block[((2) * (4)) + (0)],s_block[((2) * (4)) + (1)],s_block[((2) * (4)) + (2)],s_block[((2) * (4)) + (3)] }),
    .m_port('{ c_xt_data_scclang_global_0[2][0],c_xt_data_scclang_global_0[2][1],c_xt_data_scclang_global_0[2][2],c_xt_data_scclang_global_0[2][3] })
  );
  always @(*) begin
  end

  fwd_lift_sc_module_16 u_xt_3(
    .clk(clk),
    .reset(reset),
    .s_valid(s_valid),
    .s_ready(c_xt_ready_scclang_global_1[3]),
    .m_valid(c_xt_valid_scclang_global_2[3]),
    .m_ready(m_ready),
    .s_port('{ s_block[((3) * (4)) + (0)],s_block[((3) * (4)) + (1)],s_block[((3) * (4)) + (2)],s_block[((3) * (4)) + (3)] }),
    .m_port('{ c_xt_data_scclang_global_0[3][0],c_xt_data_scclang_global_0[3][1],c_xt_data_scclang_global_0[3][2],c_xt_data_scclang_global_0[3][3] })
  );
  always @(*) begin
  end

  fwd_lift_sc_module_17 u_yt_0(
    .clk(clk),
    .reset(reset),
    .s_valid(c_xt_valid_scclang_global_2[0]),
    .s_ready(c_yt_ready_scclang_global_4[0]),
    .m_valid(c_yt_valid_scclang_global_5[0]),
    .m_ready(m_ready),
    .s_port('{ c_xt_data_scclang_global_0[0][0],c_xt_data_scclang_global_0[1][0],c_xt_data_scclang_global_0[2][0],c_xt_data_scclang_global_0[3][0] }),
    .m_port('{ c_yt_data_scclang_global_3[0][0],c_yt_data_scclang_global_3[1][0],c_yt_data_scclang_global_3[2][0],c_yt_data_scclang_global_3[3][0] })
  );
  always @(*) begin
  end

  fwd_lift_sc_module_17 u_yt_1(
    .clk(clk),
    .reset(reset),
    .s_valid(c_xt_valid_scclang_global_2[0]),
    .s_ready(c_yt_ready_scclang_global_4[1]),
    .m_valid(c_yt_valid_scclang_global_5[1]),
    .m_ready(m_ready),
    .s_port('{ c_xt_data_scclang_global_0[0][1],c_xt_data_scclang_global_0[1][1],c_xt_data_scclang_global_0[2][1],c_xt_data_scclang_global_0[3][1] }),
    .m_port('{ c_yt_data_scclang_global_3[0][1],c_yt_data_scclang_global_3[1][1],c_yt_data_scclang_global_3[2][1],c_yt_data_scclang_global_3[3][1] })
  );
  always @(*) begin
  end

  fwd_lift_sc_module_17 u_yt_2(
    .clk(clk),
    .reset(reset),
    .s_valid(c_xt_valid_scclang_global_2[0]),
    .s_ready(c_yt_ready_scclang_global_4[2]),
    .m_valid(c_yt_valid_scclang_global_5[2]),
    .m_ready(m_ready),
    .s_port('{ c_xt_data_scclang_global_0[0][2],c_xt_data_scclang_global_0[1][2],c_xt_data_scclang_global_0[2][2],c_xt_data_scclang_global_0[3][2] }),
    .m_port('{ c_yt_data_scclang_global_3[0][2],c_yt_data_scclang_global_3[1][2],c_yt_data_scclang_global_3[2][2],c_yt_data_scclang_global_3[3][2] })
  );
  always @(*) begin
  end

  fwd_lift_sc_module_17 u_yt_3(
    .clk(clk),
    .reset(reset),
    .s_valid(c_xt_valid_scclang_global_2[0]),
    .s_ready(c_yt_ready_scclang_global_4[3]),
    .m_valid(c_yt_valid_scclang_global_5[3]),
    .m_ready(m_ready),
    .s_port('{ c_xt_data_scclang_global_0[0][3],c_xt_data_scclang_global_0[1][3],c_xt_data_scclang_global_0[2][3],c_xt_data_scclang_global_0[3][3] }),
    .m_port('{ c_yt_data_scclang_global_3[0][3],c_yt_data_scclang_global_3[1][3],c_yt_data_scclang_global_3[2][3],c_yt_data_scclang_global_3[3][3] })
  );
  always @(*) begin
  end

  always @(m_ready) begin: mc_proc
    
    
    s_ready <= m_ready;
  end
  always_ff @(posedge clk) begin: ms_fwd_order
    
    
    if (m_ready) begin
      m_block[0] = ((c_yt_data_scclang_global_3[0][0]) + (-32'd6148914691236517206)) ^ (-32'd6148914691236517206);
      m_block[1] = ((c_yt_data_scclang_global_3[0][1]) + (-32'd6148914691236517206)) ^ (-32'd6148914691236517206);
      m_block[2] = ((c_yt_data_scclang_global_3[1][0]) + (-32'd6148914691236517206)) ^ (-32'd6148914691236517206);
      m_block[3] = ((c_yt_data_scclang_global_3[1][1]) + (-32'd6148914691236517206)) ^ (-32'd6148914691236517206);
      m_block[4] = ((c_yt_data_scclang_global_3[0][2]) + (-32'd6148914691236517206)) ^ (-32'd6148914691236517206);
      m_block[5] = ((c_yt_data_scclang_global_3[2][0]) + (-32'd6148914691236517206)) ^ (-32'd6148914691236517206);
      m_block[6] = ((c_yt_data_scclang_global_3[1][2]) + (-32'd6148914691236517206)) ^ (-32'd6148914691236517206);
      m_block[7] = ((c_yt_data_scclang_global_3[2][1]) + (-32'd6148914691236517206)) ^ (-32'd6148914691236517206);
      m_block[8] = ((c_yt_data_scclang_global_3[0][3]) + (-32'd6148914691236517206)) ^ (-32'd6148914691236517206);
      m_block[9] = ((c_yt_data_scclang_global_3[3][0]) + (-32'd6148914691236517206)) ^ (-32'd6148914691236517206);
      m_block[10] = ((c_yt_data_scclang_global_3[2][2]) + (-32'd6148914691236517206)) ^ (-32'd6148914691236517206);
      m_block[11] = ((c_yt_data_scclang_global_3[1][3]) + (-32'd6148914691236517206)) ^ (-32'd6148914691236517206);
      m_block[12] = ((c_yt_data_scclang_global_3[3][1]) + (-32'd6148914691236517206)) ^ (-32'd6148914691236517206);
      m_block[13] = ((c_yt_data_scclang_global_3[2][3]) + (-32'd6148914691236517206)) ^ (-32'd6148914691236517206);
      m_block[14] = ((c_yt_data_scclang_global_3[3][2]) + (-32'd6148914691236517206)) ^ (-32'd6148914691236517206);
      m_block[15] = ((c_yt_data_scclang_global_3[3][3]) + (-32'd6148914691236517206)) ^ (-32'd6148914691236517206);
      m_valid <= c_yt_valid_scclang_global_5[0];
    end
  end
endmodule
module fwd_lift_sc_module_16 (
  input logic [0:0] clk,
  input logic [0:0] reset,
  input logic signed[31:0] s_port[0:3],
  input logic [0:0] s_valid,
  input logic [0:0] m_ready,
  output logic [0:0] s_ready,
  output logic signed[31:0] m_port[0:3],
  output logic [0:0] m_valid
);
  logic [5:0] v_scclang_global_0;
  logic signed[31:0] w_scclang_global_1[0:5];
  logic signed[31:0] x_scclang_global_2[0:5];
  logic signed[31:0] y_scclang_global_3[0:5];
  logic signed[31:0] z_scclang_global_4[0:5];
  logic signed[31:0] stages_scclang_global_5 = 6;
  always @(m_ready or x_scclang_global_2[((6) - (1))] or y_scclang_global_3[((6) - (1))] or z_scclang_global_4[((6) - (1))] or w_scclang_global_1[((6) - (1))] or v_scclang_global_0) begin: mc_proc
    
    
    s_ready <= m_ready;
    m_port[0] = x_scclang_global_2[(6) - (1)];
    m_port[1] = y_scclang_global_3[(6) - (1)];
    m_port[2] = z_scclang_global_4[(6) - (1)];
    m_port[3] = w_scclang_global_1[(6) - (1)];
    m_valid <= v_scclang_global_0[(6) - (1)];
  end
  always_ff @(posedge clk) begin: ms_proc
    logic signed[31:0] i_ms_proc_local_2;
    
    if ((reset) == (0)) begin
      for (i_ms_proc_local_2 = 0;(i_ms_proc_local_2) < (6);i_ms_proc_local_2++) begin
        x_scclang_global_2[i_ms_proc_local_2] <= 32'd0;
        y_scclang_global_3[i_ms_proc_local_2] <= 32'd0;
        z_scclang_global_4[i_ms_proc_local_2] <= 32'd0;
        w_scclang_global_1[i_ms_proc_local_2] <= 32'd0;
      end
      v_scclang_global_0 <= 6'd0;
    end else begin
      if (m_ready) begin
        x_scclang_global_2[0] <= ((s_port[0]) + (s_port[3])) >>> (1);
        y_scclang_global_3[0] <= s_port[1];
        z_scclang_global_4[0] <= ((s_port[2]) + (s_port[1])) >>> (1);
        w_scclang_global_1[0] <= s_port[3];
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
        v_scclang_global_0 <= ((v_scclang_global_0) << (1)) | (s_valid);
      end
    end

  end
endmodule
module fwd_lift_sc_module_17 (
  input logic [0:0] clk,
  input logic [0:0] reset,
  input logic signed[31:0] s_port[0:3],
  input logic [0:0] s_valid,
  input logic [0:0] m_ready,
  output logic [0:0] s_ready,
  output logic signed[31:0] m_port[0:3],
  output logic [0:0] m_valid
);
  logic [5:0] v_scclang_global_0;
  logic signed[31:0] w_scclang_global_1[0:5];
  logic signed[31:0] x_scclang_global_2[0:5];
  logic signed[31:0] y_scclang_global_3[0:5];
  logic signed[31:0] z_scclang_global_4[0:5];
  logic signed[31:0] stages_scclang_global_5 = 6;
  always @(m_ready or x_scclang_global_2[((6) - (1))] or y_scclang_global_3[((6) - (1))] or z_scclang_global_4[((6) - (1))] or w_scclang_global_1[((6) - (1))] or v_scclang_global_0) begin: mc_proc
    
    
    s_ready <= m_ready;
    m_port[0] = x_scclang_global_2[(6) - (1)];
    m_port[1] = y_scclang_global_3[(6) - (1)];
    m_port[2] = z_scclang_global_4[(6) - (1)];
    m_port[3] = w_scclang_global_1[(6) - (1)];
    m_valid <= v_scclang_global_0[(6) - (1)];
  end
  always_ff @(posedge clk) begin: ms_proc
    logic signed[31:0] i_ms_proc_local_2;
    
    if ((reset) == (0)) begin
      for (i_ms_proc_local_2 = 0;(i_ms_proc_local_2) < (6);i_ms_proc_local_2++) begin
        x_scclang_global_2[i_ms_proc_local_2] <= 32'd0;
        y_scclang_global_3[i_ms_proc_local_2] <= 32'd0;
        z_scclang_global_4[i_ms_proc_local_2] <= 32'd0;
        w_scclang_global_1[i_ms_proc_local_2] <= 32'd0;
      end
      v_scclang_global_0 <= 6'd0;
    end else begin
      if (m_ready) begin
        x_scclang_global_2[0] <= ((s_port[0]) + (s_port[3])) >>> (1);
        y_scclang_global_3[0] <= s_port[1];
        z_scclang_global_4[0] <= ((s_port[2]) + (s_port[1])) >>> (1);
        w_scclang_global_1[0] <= s_port[3];
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
        v_scclang_global_0 <= ((v_scclang_global_0) << (1)) | (s_valid);
      end
    end

  end
endmodule
module block_buffer_sc_module_6 (
  input logic [0:0] clk,
  input logic [0:0] reset,
  input logic [31:0] s_block[0:15],
  input logic [0:0] s_valid,
  input logic [0:0] m_ready,
  output logic [0:0] s_ready,
  output logic [31:0] m_block[0:15],
  output logic [0:0] m_valid
);
  logic [511:0] c_bi_scclang_global_0;
  logic [511:0] c_bo_scclang_global_1;
  fifo_cc_sc_module_18 u_bbuf(
    .clk(clk),
    .reset(reset),
    .din(c_bi_scclang_global_0),
    .wr_en(s_valid),
    .full(s_ready),
    .dout(c_bo_scclang_global_1),
    .rd_en(m_ready),
    .empty(m_valid)
  );
  always @(*) begin
  end

  always @(s_block[(0)] or s_block[(1)] or s_block[(2)] or s_block[(3)] or s_block[(4)] or s_block[(5)] or s_block[(6)] or s_block[(7)] or s_block[(8)] or s_block[(9)] or s_block[(10)] or s_block[(11)] or s_block[(12)] or s_block[(13)] or s_block[(14)] or s_block[(15)] or c_bo_scclang_global_1) begin: mc_proc
    logic [511:0] bbufi_mc_proc_local_2;
    logic [511:0] bbufo_mc_proc_local_3;
    logic signed[31:0] i_mc_proc_local_4;
    logic signed[31:0] i_mc_proc_local_5;
    
    for (i_mc_proc_local_4 = 0;(i_mc_proc_local_4) < (16);i_mc_proc_local_4++) begin
      bbufi_mc_proc_local_2[((i_mc_proc_local_4) * (32)) +: (32)] = s_block[i_mc_proc_local_4];
    end
    c_bi_scclang_global_0 <= bbufi_mc_proc_local_2;
    bbufo_mc_proc_local_3 = c_bo_scclang_global_1;
    for (i_mc_proc_local_5 = 0;(i_mc_proc_local_5) < (16);i_mc_proc_local_5++) begin
      m_block[i_mc_proc_local_5] = bbufo_mc_proc_local_3[((i_mc_proc_local_5) * (32)) +: (32)];
    end
  end
endmodule
module fifo_cc_sc_module_18 (
  input logic [0:0] clk,
  input logic [0:0] reset,
  input logic [511:0] din,
  input logic [0:0] wr_en,
  input logic [0:0] rd_en,
  output logic [0:0] full,
  output logic [511:0] dout,
  output logic [0:0] empty
);
  logic [511:0] data_scclang_global_0[0:1];
  logic [0:0] empty_i_scclang_global_1;
  logic [0:0] full_i_scclang_global_2;
  logic [0:0] rd_en_i_scclang_global_3;
  logic [0:0] rd_idx_scclang_global_4;
  logic [0:0] wr_en_i_scclang_global_5;
  logic [0:0] wr_idx_scclang_global_6;
  logic signed[31:0] MAX_DEPTH_scclang_global_7 = 2;
  initial begin
    if ((2) <= (0)) begin
    end
  end
  always @(wr_en or rd_en or full_i_scclang_global_2 or empty_i_scclang_global_1 or rd_idx_scclang_global_4 or data_scclang_global_0[(0)] or data_scclang_global_0[(1)]) begin: mc_proc
    
    
    if (1) begin
      dout <= data_scclang_global_0[rd_idx_scclang_global_4];
    end
    wr_en_i_scclang_global_5 <= (wr_en) && (!(full_i_scclang_global_2));
    rd_en_i_scclang_global_3 <= (rd_en) && (!(empty_i_scclang_global_1));
    full <= (full_i_scclang_global_2) == (0);
    empty <= (empty_i_scclang_global_1) == (0);
  end
  always_ff @(posedge clk) begin: ms_proc
    logic [0:0] wr_inc_ms_proc_local_3;
    logic [0:0] rd_inc_ms_proc_local_4;
    logic [31:0] i_ms_proc_local_5;
    
    wr_inc_ms_proc_local_3 = ((wr_idx_scclang_global_6) + (1)) % (2);
    rd_inc_ms_proc_local_4 = ((rd_idx_scclang_global_4) + (1)) % (2);
    if ((reset) == (0)) begin
      if (!(1)) begin
        dout <= 0;
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
          dout <= data_scclang_global_0[rd_idx_scclang_global_4];
        end
      end
      if (wr_en_i_scclang_global_5) begin
        data_scclang_global_0[wr_idx_scclang_global_6] <= din;
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
endmodule
module encode_ints_sc_module_7 (
  input logic [0:0] clk,
  input logic [0:0] reset,
  input logic [31:0] s_block[0:15],
  input logic [0:0] s_valid,
  input logic [0:0] s_flush,
  input logic [0:0] m_ready,
  output logic [0:0] s_ready,
  output logic [5:0] m_bc,
  output logic [32:0] m_bp,
  output logic [0:0] m_last,
  output logic [0:0] m_valid
);
  logic [4:0] bc1_scclang_global_0;
  logic [15:0] bp1_scclang_global_1;
  logic [15:0] c_bplane_scclang_global_2[0:31];
  logic [0:0] flush1_scclang_global_3;
  logic [4:0] k0_scclang_global_4;
  logic [0:0] last1_scclang_global_5;
  logic [4:0] n0_scclang_global_6;
  logic [4:0] n1_scclang_global_7;
  logic [0:0] valid1_scclang_global_8;
  logic signed[31:0] planes_scclang_global_9 = 32;
  always @(s_block[(0)] or s_block[(1)] or s_block[(2)] or s_block[(3)] or s_block[(4)] or s_block[(5)] or s_block[(6)] or s_block[(7)] or s_block[(8)] or s_block[(9)] or s_block[(10)] or s_block[(11)] or s_block[(12)] or s_block[(13)] or s_block[(14)] or s_block[(15)] or k0_scclang_global_4 or flush1_scclang_global_3 or m_ready or s_valid or s_flush) begin: mc_proc
    logic [0:0] last_mc_proc_local_3;
    logic [15:0] tmp_mc_proc_local_4;
    logic signed[31:0] j_mc_proc_local_5;
    logic signed[31:0] i_mc_proc_local_6;
    
    last_mc_proc_local_3 = ((k0_scclang_global_4) == ((32) - (1))) || ((s_flush) && ((s_flush) != (flush1_scclang_global_3)));
    for (j_mc_proc_local_5 = 0;(j_mc_proc_local_5) < (32);j_mc_proc_local_5++) begin
      for (i_mc_proc_local_6 = 0;(i_mc_proc_local_6) < (16);i_mc_proc_local_6++) begin
        tmp_mc_proc_local_4[i_mc_proc_local_6] = s_block[i_mc_proc_local_6][j_mc_proc_local_5];
      end
      c_bplane_scclang_global_2[((32) - (1)) - (j_mc_proc_local_5)] <= tmp_mc_proc_local_4;
    end
    s_ready <= ((last_mc_proc_local_3) && (m_ready)) || (!(s_valid));
  end
  always_ff @(posedge clk) begin: ms_proc
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
    
    if ((reset) == (0)) begin
      k0_scclang_global_4 <= 5'd0;
      n0_scclang_global_6 <= 5'd0;
      n1_scclang_global_7 <= 5'd0;
      bc1_scclang_global_0 <= 5'd0;
      m_bc <= 6'd0;
      bp1_scclang_global_1 <= 16'd0;
      m_bp <= 33'd0;
      last1_scclang_global_5 <= 0;
      m_last <= 0;
      valid1_scclang_global_8 <= 0;
      m_valid <= 0;
    end else begin
      if (m_ready) begin
        last_ms_proc_local_7 = ((k0_scclang_global_4) == ((32) - (1))) || ((s_flush) && ((s_flush) != (flush1_scclang_global_3)));
        b_ms_proc_local_8 = 0;
        frst_ms_proc_local_9 = 1;
        for (i_ms_proc_local_10 = 16;(i_ms_proc_local_10) > (0);i_ms_proc_local_10--) begin
          if (c_bplane_scclang_global_2[k0_scclang_global_4][((i_ms_proc_local_10) - (1))]) begin
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
        valid1_scclang_global_8 <= s_valid;
        flush1_scclang_global_3 <= s_flush;
        if (s_valid) begin
          if ((b_ms_proc_local_8) > (n0_scclang_global_6)) begin
            n0_scclang_global_6 <= b_ms_proc_local_8;
          end
          if (last_ms_proc_local_7) begin
            k0_scclang_global_4 <= 5'd0;
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
            if (bp1_scclang_global_1[(0)]) begin
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
                first_after_bc_ms_proc_local_15 = shiftamt_ms_proc_local_13;
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
            if (((!(bp1_scclang_global_1[(n1_scclang_global_7)])) && (!(bp1_scclang_global_1[((n1_scclang_global_7) - (1))]))) || ((bp1_scclang_global_1[(n1_scclang_global_7)]) && (!(bp1_scclang_global_1[((n1_scclang_global_7) - (1))])))) begin
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

        m_bc <= j_ms_proc_local_21;
        m_bp <= tmp_ms_proc_local_11;
        m_last <= last1_scclang_global_5;
        m_valid <= valid1_scclang_global_8;
      end else begin
        if (!(s_flush)) begin
          flush1_scclang_global_3 <= 0;
        end
      end

    end

  end
endmodule
module encode_stream_sc_module_8 (
  input logic [0:0] clk,
  input logic [0:0] reset,
  input logic [15:0] minbits,
  input logic [15:0] maxbits,
  input logic [15:0] maxprec,
  input logic signed[15:0] minexp,
  input logic [5:0] s_bc,
  input logic [32:0] s_bp,
  input logic [0:0] s_last,
  input logic [0:0] s_valid,
  input logic [7:0] s_ex_data,
  input logic [0:0] s_ex_valid,
  output logic [0:0] s_ex_ready,
  output logic [0:0] s_ready,
  output logic [0:0] m_flush,
  output logic [31:0] m_bits_data_tdata,
  output logic [0:0] m_bits_data_tlast,
  output logic [0:0] m_bits_valid,
  input logic [0:0] m_bits_ready
);
  logic [2:0] cs_scclang_global_0_s;
  logic [5:0] cs_scclang_global_0_prec;
  logic [5:0] cs_scclang_global_0_planes;
  logic [64:0] cs_scclang_global_0_buf;
  logic [10:0] cs_scclang_global_0_bits;
  logic [6:0] cs_scclang_global_0_posr;
  logic [6:0] cs_scclang_global_0_posw;
  logic [2:0] ns_scclang_global_1_s;
  logic [5:0] ns_scclang_global_1_prec;
  logic [5:0] ns_scclang_global_1_planes;
  logic [64:0] ns_scclang_global_1_buf;
  logic [10:0] ns_scclang_global_1_bits;
  logic [6:0] ns_scclang_global_1_posr;
  logic [6:0] ns_scclang_global_1_posw;
  logic signed[31:0] pre_w_scclang_global_2 = 6;
  logic signed[31:0] tbc_w_scclang_global_3 = 11;
  logic signed[31:0] buf_w_scclang_global_4 = 65;
  logic signed[31:0] pos_w_scclang_global_5 = 7;
  logic [10:0] bc_actual_scclang_global_6;
  logic [64:0] bp_actual_scclang_global_7;
  logic [0:0] done_actual_scclang_global_8;
  always @(s_ex_valid or s_ex_data or m_bits_ready or minbits or maxbits or maxprec or minexp or s_bc or s_bp or s_last or s_valid or cs_scclang_global_0_s or cs_scclang_global_0_prec or cs_scclang_global_0_planes or cs_scclang_global_0_buf or cs_scclang_global_0_bits or cs_scclang_global_0_posr or cs_scclang_global_0_posw) begin: mc_proc
    logic [10:0] pbits_mc_proc_local_7;
    logic [10:0] abits_mc_proc_local_8;
    logic [10:0] n_mc_proc_local_9;
    logic [2:0] ts_mc_proc_local_2_s;
    logic [5:0] ts_mc_proc_local_2_prec;
    logic [5:0] ts_mc_proc_local_2_planes;
    logic [64:0] ts_mc_proc_local_2_buf;
    logic [10:0] ts_mc_proc_local_2_bits;
    logic [6:0] ts_mc_proc_local_2_posr;
    logic [6:0] ts_mc_proc_local_2_posw;
    logic [0:0] dis_done_mc_proc_local_3;
    logic [0:0] add_done_mc_proc_local_4;
    logic [0:0] pad_done_mc_proc_local_5;
    logic [0:0] fls_done_mc_proc_local_6;
    logic [2:0] ts_mc_proc_local_2_s__ref_0;
    logic [5:0] ts_mc_proc_local_2_planes__ref_0;
    logic [6:0] ts_mc_proc_local_2_posr__ref_0;
    logic [10:0] ts_mc_proc_local_2_bits__ref_0;
    logic [5:0] ts_mc_proc_local_2_prec__ref_0;
    logic [64:0] ts_mc_proc_local_2_buf__ref_0;
    logic [6:0] ts_mc_proc_local_2_posw__ref_0;
    
    ts_mc_proc_local_2_posw__ref_0 = ts_mc_proc_local_2_posw;
    ts_mc_proc_local_2_buf__ref_0 = ts_mc_proc_local_2_buf;
    ts_mc_proc_local_2_prec__ref_0 = ts_mc_proc_local_2_prec;
    ts_mc_proc_local_2_bits__ref_0 = ts_mc_proc_local_2_bits;
    ts_mc_proc_local_2_posr__ref_0 = ts_mc_proc_local_2_posr;
    ts_mc_proc_local_2_planes__ref_0 = ts_mc_proc_local_2_planes;
    ts_mc_proc_local_2_s__ref_0 = ts_mc_proc_local_2_s;
    s_ex_ready <= 0;
    s_ready <= 0;
    m_flush <= 0;
    m_bits_valid <= 0;
    ts_mc_proc_local_2_s__ref_0 = cs_scclang_global_0_s;
    ts_mc_proc_local_2_prec__ref_0 = cs_scclang_global_0_prec;
    ts_mc_proc_local_2_planes__ref_0 = cs_scclang_global_0_planes;
    ts_mc_proc_local_2_buf__ref_0 = cs_scclang_global_0_buf;
    ts_mc_proc_local_2_bits__ref_0 = cs_scclang_global_0_bits;
    ts_mc_proc_local_2_posr__ref_0 = cs_scclang_global_0_posr;
    ts_mc_proc_local_2_posw__ref_0 = cs_scclang_global_0_posw;
    case(ts_mc_proc_local_2_s__ref_0)
      0: begin
        ts_mc_proc_local_2_prec__ref_0 = ((maxprec) < (((0) < (((s_ex_data) - (minexp)) + ((2) * ((2) + (1)))) ? (((s_ex_data) - (minexp)) + ((2) * ((2) + (1)))) : (0))) ? (maxprec) : (((0) < (((s_ex_data) - (minexp)) + ((2) * ((2) + (1)))) ? (((s_ex_data) - (minexp)) + ((2) * ((2) + (1)))) : (0))));
        ts_mc_proc_local_2_bits__ref_0 = 0;
        ts_mc_proc_local_2_planes__ref_0 = 0;
        ts_mc_proc_local_2_buf__ref_0 = 0;
        ts_mc_proc_local_2_posr__ref_0 = 0;
        ts_mc_proc_local_2_posw__ref_0 = 0;
        if (s_ex_valid) begin
          if (((s_ex_data) != (0)) && ((ts_mc_proc_local_2_prec__ref_0) != (0))) begin
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
        s_ex_ready <= 1;
        ts_mc_proc_local_2_s__ref_0 = 4;

      end
      2: begin
        ts_mc_proc_local_2_buf__ref_0[0] = 1;
        ts_mc_proc_local_2_buf__ref_0[8:1] = s_ex_data;
        ts_mc_proc_local_2_bits__ref_0 = (8) + (1);
        ts_mc_proc_local_2_posw__ref_0 = (8) + (1);
        s_ex_ready <= 1;
        if ((ts_mc_proc_local_2_planes__ref_0) < (ts_mc_proc_local_2_prec__ref_0)) begin
          ts_mc_proc_local_2_s__ref_0 = 3;
        end else begin
          ts_mc_proc_local_2_s__ref_0 = 4;
        end


      end
      3: begin
        if (s_valid) begin
          if (zhw__encode_streamfp_t8_23_2_bits_t32__pack_bits_func_1(ts_mc_proc_local_2_s__ref_0,ts_mc_proc_local_2_prec__ref_0,ts_mc_proc_local_2_planes__ref_0,ts_mc_proc_local_2_buf__ref_0,ts_mc_proc_local_2_bits__ref_0,ts_mc_proc_local_2_posr__ref_0,ts_mc_proc_local_2_posw__ref_0,s_bc,s_bp,ts_mc_proc_local_2_s__ref_0,ts_mc_proc_local_2_prec__ref_0,ts_mc_proc_local_2_planes__ref_0,ts_mc_proc_local_2_buf__ref_0,ts_mc_proc_local_2_bits__ref_0,ts_mc_proc_local_2_posr__ref_0,ts_mc_proc_local_2_posw__ref_0)) begin
            s_ready <= 1;
            if (s_last) begin
              ts_mc_proc_local_2_planes__ref_0 = 32;
            end else begin
              ts_mc_proc_local_2_planes__ref_0 = (ts_mc_proc_local_2_planes__ref_0) + (1);
            end

          end
        end else begin
          s_ready <= 1;
        end

        dis_done_mc_proc_local_3 = (ts_mc_proc_local_2_planes__ref_0) == (32);
        add_done_mc_proc_local_4 = ((ts_mc_proc_local_2_planes__ref_0) >= (ts_mc_proc_local_2_prec__ref_0)) || ((ts_mc_proc_local_2_bits__ref_0) >= (maxbits));
        pad_done_mc_proc_local_5 = (ts_mc_proc_local_2_bits__ref_0) >= (minbits);
        fls_done_mc_proc_local_6 = zhw__encode_streamfp_t8_23_2_bits_t32__out_bits_func_2(ts_mc_proc_local_2_s__ref_0,ts_mc_proc_local_2_prec__ref_0,ts_mc_proc_local_2_planes__ref_0,ts_mc_proc_local_2_buf__ref_0,ts_mc_proc_local_2_bits__ref_0,ts_mc_proc_local_2_posr__ref_0,ts_mc_proc_local_2_posw__ref_0,(add_done_mc_proc_local_4) && (pad_done_mc_proc_local_5),ts_mc_proc_local_2_s__ref_0,ts_mc_proc_local_2_prec__ref_0,ts_mc_proc_local_2_planes__ref_0,ts_mc_proc_local_2_buf__ref_0,ts_mc_proc_local_2_bits__ref_0,ts_mc_proc_local_2_posr__ref_0,ts_mc_proc_local_2_posw__ref_0);
        if (add_done_mc_proc_local_4) begin
          if (((dis_done_mc_proc_local_3) && (pad_done_mc_proc_local_5)) && (fls_done_mc_proc_local_6)) begin
            ts_mc_proc_local_2_s__ref_0 = 0;
          end else begin
            ts_mc_proc_local_2_s__ref_0 = 4;
          end

        end

      end
      4: begin
        if (s_valid) begin
          if ((ts_mc_proc_local_2_planes__ref_0) < (32)) begin
            if ((ts_mc_proc_local_2_planes__ref_0) < ((32) - (2))) begin
              m_flush <= 1;
            end
            s_ready <= 1;
            if (s_last) begin
              ts_mc_proc_local_2_planes__ref_0 = 32;
            end else begin
              ts_mc_proc_local_2_planes__ref_0 = (ts_mc_proc_local_2_planes__ref_0) + (1);
            end

          end
        end else begin
          s_ready <= 1;
        end

        if ((ts_mc_proc_local_2_bits__ref_0) < (minbits)) begin
          pbits_mc_proc_local_7 = (minbits) - (ts_mc_proc_local_2_bits__ref_0);
          abits_mc_proc_local_8 = (32) - ((ts_mc_proc_local_2_bits__ref_0) & ((32) - (1)));
          n_mc_proc_local_9 = ((pbits_mc_proc_local_7) < (abits_mc_proc_local_8) ? (pbits_mc_proc_local_7) : (abits_mc_proc_local_8));
          zhw__encode_streamfp_t8_23_2_bits_t32__pack_bits_func_1(ts_mc_proc_local_2_s__ref_0,ts_mc_proc_local_2_prec__ref_0,ts_mc_proc_local_2_planes__ref_0,ts_mc_proc_local_2_buf__ref_0,ts_mc_proc_local_2_bits__ref_0,ts_mc_proc_local_2_posr__ref_0,ts_mc_proc_local_2_posw__ref_0,n_mc_proc_local_9,32'd0,ts_mc_proc_local_2_s__ref_0,ts_mc_proc_local_2_prec__ref_0,ts_mc_proc_local_2_planes__ref_0,ts_mc_proc_local_2_buf__ref_0,ts_mc_proc_local_2_bits__ref_0,ts_mc_proc_local_2_posr__ref_0,ts_mc_proc_local_2_posw__ref_0);
        end
        dis_done_mc_proc_local_3 = (ts_mc_proc_local_2_planes__ref_0) == (32);
        pad_done_mc_proc_local_5 = (ts_mc_proc_local_2_bits__ref_0) >= (minbits);
        fls_done_mc_proc_local_6 = zhw__encode_streamfp_t8_23_2_bits_t32__out_bits_func_2(ts_mc_proc_local_2_s__ref_0,ts_mc_proc_local_2_prec__ref_0,ts_mc_proc_local_2_planes__ref_0,ts_mc_proc_local_2_buf__ref_0,ts_mc_proc_local_2_bits__ref_0,ts_mc_proc_local_2_posr__ref_0,ts_mc_proc_local_2_posw__ref_0,pad_done_mc_proc_local_5,ts_mc_proc_local_2_s__ref_0,ts_mc_proc_local_2_prec__ref_0,ts_mc_proc_local_2_planes__ref_0,ts_mc_proc_local_2_buf__ref_0,ts_mc_proc_local_2_bits__ref_0,ts_mc_proc_local_2_posr__ref_0,ts_mc_proc_local_2_posw__ref_0);
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
    ts_mc_proc_local_2_s = ts_mc_proc_local_2_s__ref_0;
    ts_mc_proc_local_2_planes = ts_mc_proc_local_2_planes__ref_0;
    ts_mc_proc_local_2_posr = ts_mc_proc_local_2_posr__ref_0;
    ts_mc_proc_local_2_bits = ts_mc_proc_local_2_bits__ref_0;
    ts_mc_proc_local_2_prec = ts_mc_proc_local_2_prec__ref_0;
    ts_mc_proc_local_2_buf = ts_mc_proc_local_2_buf__ref_0;
    ts_mc_proc_local_2_posw = ts_mc_proc_local_2_posw__ref_0;
  end
  always_ff @(posedge clk) begin: ms_proc
    
    
    if ((reset) == (0)) begin
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
  function automatic logic [0:0] zhw__encode_streamfp_t8_23_2_bits_t32__pack_bits_func_1 (input logic [2:0] ts_s, input logic [5:0] ts_prec, input logic [5:0] ts_planes, input logic [64:0] ts_buf, input logic [10:0] ts_bits, input logic [6:0] ts_posr, input logic [6:0] ts_posw, input logic [10:0] bc, input logic [64:0] bp, inout logic [2:0] ts_s__ref_0, inout logic [5:0] ts_prec__ref_0, inout logic [5:0] ts_planes__ref_0, inout logic [64:0] ts_buf__ref_0, inout logic [10:0] ts_bits__ref_0, inout logic [6:0] ts_posr__ref_0, inout logic [6:0] ts_posw__ref_0);
  begin
    logic [10:0] n__local_10;
    bc_actual_scclang_global_6 = bc;
    bp_actual_scclang_global_7 = bp;
    n__local_10 = ((bc_actual_scclang_global_6) < ((maxbits) - (ts_bits)) ? (bc_actual_scclang_global_6) : ((maxbits) - (ts_bits)));
    if (((65) - (ts_posw)) >= (n__local_10)) begin
      ts_buf__ref_0 = ((((bp_actual_scclang_global_7) >> (0)) & ~(~($bits(bp_actual_scclang_global_7)'('b0)) << (((n__local_10) - (1)) - (0) + 1)) & ~(~($bits(bp_actual_scclang_global_7)'('b0)) << ((((ts_posw) + (n__local_10)) - (1))-(ts_posw)+1))) << (ts_posw)) | ((ts_buf__ref_0) & ((~($bits(ts_buf__ref_0)'('b0)) ) << ((((ts_posw) + (n__local_10)) - (1)) + 1) | ~(( ~($bits(ts_buf__ref_0)'('b0)) ) << (ts_posw))));
      ts_bits__ref_0 = (ts_bits) + (n__local_10);
      ts_posw__ref_0 = (ts_posw) + (n__local_10);
      return 1;
    end
    return 0;
  end
  endfunction
  function automatic logic [0:0] zhw__encode_streamfp_t8_23_2_bits_t32__out_bits_func_2 (input logic [2:0] ts_s, input logic [5:0] ts_prec, input logic [5:0] ts_planes, input logic [64:0] ts_buf, input logic [10:0] ts_bits, input logic [6:0] ts_posr, input logic [6:0] ts_posw, input logic [0:0] done, inout logic [2:0] ts_s__ref_0, inout logic [5:0] ts_prec__ref_0, inout logic [5:0] ts_planes__ref_0, inout logic [64:0] ts_buf__ref_0, inout logic [10:0] ts_bits__ref_0, inout logic [6:0] ts_posr__ref_0, inout logic [6:0] ts_posw__ref_0);
  begin
    logic [6:0] bbits__local_11;
    logic [31:0] flit__local_12_tdata;
    logic [0:0] flit__local_12_tlast;
    done_actual_scclang_global_8 = done;
    bbits__local_11 = (ts_posw) - (ts_posr);
    if (((bbits__local_11) >= (32)) && (m_bits_ready)) begin
      flit__local_12_tdata = ((ts_buf__ref_0) >> (ts_posr)) & ~(~($bits(ts_buf__ref_0)'('b0)) << ((((ts_posr) + (32)) - (1)) - (ts_posr) + 1));
      flit__local_12_tlast = (done_actual_scclang_global_8) && ((bbits__local_11) == (32));
      m_bits_data_tdata <= flit__local_12_tdata;
      m_bits_data_tlast <= flit__local_12_tlast;
      m_bits_valid <= 1;
      ts_buf__ref_0 = ((((ts_buf__ref_0) >> ((ts_posr) + (32))) & ~(~($bits(ts_buf__ref_0)'('b0)) << (((ts_posw) - (1)) - ((ts_posr) + (32)) + 1)) & ~(~($bits(ts_buf__ref_0)'('b0)) << (((ts_posw) - (32))-(0)+1))) << (0)) | ((ts_buf__ref_0) & ((~($bits(ts_buf__ref_0)'('b0)) ) << (((ts_posw) - (32)) + 1) | ~(( ~($bits(ts_buf__ref_0)'('b0)) ) << (0))));
      ts_posr__ref_0 = 0;
      ts_posw__ref_0 = (bbits__local_11) - (32);
      return flit__local_12_tlast;
    end
    return (done_actual_scclang_global_8) && ((bbits__local_11) == (0));
  end
  endfunction
  function automatic void zhw__encode_streamfp_t8_23_2_bits_t32__state_t__state_t_func_0 (input logic [2:0] hthis_s, input logic [5:0] hthis_prec, input logic [5:0] hthis_planes, input logic [64:0] hthis_buf, input logic [10:0] hthis_bits, input logic [6:0] hthis_posr, input logic [6:0] hthis_posw, inout logic [2:0] hthis_s__ref_0, inout logic [5:0] hthis_prec__ref_0, inout logic [5:0] hthis_planes__ref_0, inout logic [64:0] hthis_buf__ref_0, inout logic [10:0] hthis_bits__ref_0, inout logic [6:0] hthis_posr__ref_0, inout logic [6:0] hthis_posw__ref_0);
  begin


  end
  endfunction
  function automatic void bits_t32__bits_t_func_3 (input logic [31:0] hthis_tdata, input logic [0:0] hthis_tlast, inout logic [31:0] hthis_tdata__ref_0, inout logic [0:0] hthis_tlast__ref_0);
  begin


  end
  endfunction
endmodule