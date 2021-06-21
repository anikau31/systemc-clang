module mymodule_sc_module_0 (
);
  logic [0:0] clk;
  logic [15:0] maxbits;
  logic [15:0] maxprec;
  logic [15:0] minbits;
  logic signed[15:0] minexp;
  logic [0:0] reset;
  logic [51:0] c_driver_fp_data_frac;
  logic [10:0] c_driver_fp_data_expo;
  logic [0:0] c_driver_fp_data_sign;
  logic [0:0] c_driver_fp_valid;
  logic [0:0] c_driver_fp_ready;
  logic [63:0] c_dut_enc_data_tdata;
  logic [0:0] c_dut_enc_data_tlast;
  logic [0:0] c_dut_enc_valid;
  logic [0:0] c_dut_enc_ready;
  initial begin
    minbits = 1024;
    maxbits = 1024;
    maxprec = 64;
    minexp = ((1) - (1023)) - (52);
  end
  encode_sc_module_1 u_dut(
    .clk(clk),
    .reset(reset),
    .minbits(minbits),
    .maxbits(maxbits),
    .maxprec(maxprec),
    .minexp(minexp),
    .s_fp_data_frac(c_driver_fp_data_frac),
    .s_fp_data_expo(c_driver_fp_data_expo),
    .s_fp_data_sign(c_driver_fp_data_sign),
    .s_fp_valid(c_driver_fp_valid),
    .s_fp_ready(c_driver_fp_ready),
    .m_bits_data_tdata(c_dut_enc_data_tdata),
    .m_bits_data_tlast(c_dut_enc_data_tlast),
    .m_bits_valid(c_dut_enc_valid),
    .m_bits_ready(c_dut_enc_ready)
  );
endmodule
module encode_sc_module_1 (
  input logic [0:0] clk,
  input logic [0:0] reset,
  input logic [15:0] minbits,
  input logic [15:0] maxbits,
  input logic [15:0] maxprec,
  input logic signed[15:0] minexp,
  input logic [51:0] s_fp_data_frac,
  input logic [10:0] s_fp_data_expo,
  input logic [0:0] s_fp_data_sign,
  input logic [0:0] s_fp_valid,
  output logic [0:0] s_fp_ready,
  output logic [63:0] m_bits_data_tdata,
  output logic [0:0] m_bits_data_tlast,
  output logic [0:0] m_bits_valid,
  input logic [0:0] m_bits_ready
);
  logic [63:0] c_bb_block[0:15];
  logic [0:0] c_bb_ready;
  logic [0:0] c_bb_valid;
  logic [63:0] c_eb_block[0:15];
  logic [0:0] c_eb_ready;
  logic [0:0] c_eb_valid;
  logic [5:0] c_ei_bc;
  logic [32:0] c_ei_bp;
  logic [0:0] c_ei_last;
  logic [0:0] c_ei_ready;
  logic [0:0] c_ei_valid;
  logic [0:0] c_es_flush;
  logic signed[63:0] c_fc_block[0:15];
  logic [0:0] c_fc_ready;
  logic [0:0] c_fc_valid;
  logic [51:0] c_fe_fp_data_frac;
  logic [10:0] c_fe_fp_data_expo;
  logic [0:0] c_fe_fp_data_sign;
  logic [0:0] c_fe_fp_valid;
  logic [0:0] c_fe_fp_ready;
  logic [10:0] c_fe_ex_data;
  logic [0:0] c_fe_ex_valid;
  logic [0:0] c_fe_ex_ready;
  logic [10:0] c_sp_ex1_data;
  logic [0:0] c_sp_ex1_valid;
  logic [0:0] c_sp_ex1_ready;
  logic [10:0] c_sp_ex2_data;
  logic [0:0] c_sp_ex2_valid;
  logic [0:0] c_sp_ex2_ready;
  find_emax_sc_module_2 u_find_emax(
    .clk(clk),
    .reset(reset),
    .s_fp_data_frac(s_fp_data_frac),
    .s_fp_data_expo(s_fp_data_expo),
    .s_fp_data_sign(s_fp_data_sign),
    .s_fp_valid(s_fp_valid),
    .s_fp_ready(s_fp_ready),
    .m_fp_data_frac(c_fe_fp_data_frac),
    .m_fp_data_expo(c_fe_fp_data_expo),
    .m_fp_data_sign(c_fe_fp_data_sign),
    .m_fp_valid(c_fe_fp_valid),
    .m_fp_ready(c_fe_fp_ready),
    .m_ex_data(c_fe_ex_data),
    .m_ex_valid(c_fe_ex_valid),
    .m_ex_ready(c_fe_ex_ready)
  );
  ssplit_sc_module_3 u_ssplit_ex(
    .clk(clk),
    .reset(reset),
    .s_port_data(c_fe_ex_data),
    .s_port_valid(c_fe_ex_valid),
    .s_port_ready(c_fe_ex_ready),
    .m_port1_data(c_sp_ex1_data),
    .m_port1_valid(c_sp_ex1_valid),
    .m_port1_ready(c_sp_ex1_ready),
    .m_port2_data(c_sp_ex2_data),
    .m_port2_valid(c_sp_ex2_valid),
    .m_port2_ready(c_sp_ex2_ready)
  );
  fwd_cast_sc_module_4 u_fwd_cast(
    .clk(clk),
    .reset(reset),
    .s_fp_data_frac(c_fe_fp_data_frac),
    .s_fp_data_expo(c_fe_fp_data_expo),
    .s_fp_data_sign(c_fe_fp_data_sign),
    .s_fp_valid(c_fe_fp_valid),
    .s_fp_ready(c_fe_fp_ready),
    .s_ex_data(c_sp_ex1_data),
    .s_ex_valid(c_sp_ex1_valid),
    .s_ex_ready(c_sp_ex1_ready),
    .m_valid(c_fc_valid),
    .m_ready(c_fc_ready),
    .m_block('{ c_fc_block[0],c_fc_block[1],c_fc_block[2],c_fc_block[3],c_fc_block[4],c_fc_block[5],c_fc_block[6],c_fc_block[7],c_fc_block[8],c_fc_block[9],c_fc_block[10],c_fc_block[11],c_fc_block[12],c_fc_block[13],c_fc_block[14],c_fc_block[15] })
  );
  encode_block_sc_module_5 u_encode_block(
    .clk(clk),
    .reset(reset),
    .s_valid(c_fc_valid),
    .s_ready(c_fc_ready),
    .m_valid(c_eb_valid),
    .m_ready(c_eb_ready),
    .s_block('{ c_fc_block[0],c_fc_block[1],c_fc_block[2],c_fc_block[3],c_fc_block[4],c_fc_block[5],c_fc_block[6],c_fc_block[7],c_fc_block[8],c_fc_block[9],c_fc_block[10],c_fc_block[11],c_fc_block[12],c_fc_block[13],c_fc_block[14],c_fc_block[15] }),
    .m_block('{ c_eb_block[0],c_eb_block[1],c_eb_block[2],c_eb_block[3],c_eb_block[4],c_eb_block[5],c_eb_block[6],c_eb_block[7],c_eb_block[8],c_eb_block[9],c_eb_block[10],c_eb_block[11],c_eb_block[12],c_eb_block[13],c_eb_block[14],c_eb_block[15] })
  );
  block_buffer_sc_module_6 u_block_buffer(
    .clk(clk),
    .reset(reset),
    .s_valid(c_eb_valid),
    .s_ready(c_eb_ready),
    .m_valid(c_bb_valid),
    .m_ready(c_bb_ready),
    .s_block('{ c_eb_block[0],c_eb_block[1],c_eb_block[2],c_eb_block[3],c_eb_block[4],c_eb_block[5],c_eb_block[6],c_eb_block[7],c_eb_block[8],c_eb_block[9],c_eb_block[10],c_eb_block[11],c_eb_block[12],c_eb_block[13],c_eb_block[14],c_eb_block[15] }),
    .m_block('{ c_bb_block[0],c_bb_block[1],c_bb_block[2],c_bb_block[3],c_bb_block[4],c_bb_block[5],c_bb_block[6],c_bb_block[7],c_bb_block[8],c_bb_block[9],c_bb_block[10],c_bb_block[11],c_bb_block[12],c_bb_block[13],c_bb_block[14],c_bb_block[15] })
  );
  encode_ints_sc_module_7 u_encode_ints(
    .clk(clk),
    .reset(reset),
    .s_valid(c_bb_valid),
    .s_ready(c_bb_ready),
    .s_flush(c_es_flush),
    .m_bc(c_ei_bc),
    .m_bp(c_ei_bp),
    .m_last(c_ei_last),
    .m_valid(c_ei_valid),
    .m_ready(c_ei_ready),
    .s_block('{ c_bb_block[0],c_bb_block[1],c_bb_block[2],c_bb_block[3],c_bb_block[4],c_bb_block[5],c_bb_block[6],c_bb_block[7],c_bb_block[8],c_bb_block[9],c_bb_block[10],c_bb_block[11],c_bb_block[12],c_bb_block[13],c_bb_block[14],c_bb_block[15] })
  );
  encode_stream_sc_module_8 u_encode_stream(
    .clk(clk),
    .reset(reset),
    .minbits(minbits),
    .maxbits(maxbits),
    .maxprec(maxprec),
    .minexp(minexp),
    .s_ex_data(c_sp_ex2_data),
    .s_ex_valid(c_sp_ex2_valid),
    .s_ex_ready(c_sp_ex2_ready),
    .s_bc(c_ei_bc),
    .s_bp(c_ei_bp),
    .s_last(c_ei_last),
    .s_valid(c_ei_valid),
    .s_ready(c_ei_ready),
    .m_flush(c_es_flush),
    .m_bits_data_tdata(m_bits_data_tdata),
    .m_bits_data_tlast(m_bits_data_tlast),
    .m_bits_valid(m_bits_valid),
    .m_bits_ready(m_bits_ready)
  );
endmodule
module find_emax_sc_module_2 (
  input logic [0:0] clk,
  input logic [0:0] reset,
  input logic [51:0] s_fp_data_frac,
  input logic [10:0] s_fp_data_expo,
  input logic [0:0] s_fp_data_sign,
  input logic [0:0] s_fp_valid,
  output logic [0:0] s_fp_ready,
  output logic [51:0] m_fp_data_frac,
  output logic [10:0] m_fp_data_expo,
  output logic [0:0] m_fp_data_sign,
  output logic [0:0] m_fp_valid,
  input logic [0:0] m_fp_ready,
  output logic [10:0] m_ex_data,
  output logic [0:0] m_ex_valid,
  input logic [0:0] m_ex_ready
);
  logic [0:0] c_sync;
  logic [3:0] count;
  logic [10:0] emax;
  logic [0:0] emax_v;
  logic [51:0] c_fp_data_frac;
  logic [10:0] c_fp_data_expo;
  logic [0:0] c_fp_data_sign;
  logic [0:0] c_fp_valid;
  logic [0:0] c_fp_ready;
  logic [10:0] c_ex_data;
  logic [0:0] c_ex_valid;
  logic [0:0] c_ex_ready;
  initial begin


  end
  rvfifo_cc_sc_module_9 u_que_fp(
    .clk(clk),
    .reset(reset),
    .s_port_data_frac(c_fp_data_frac),
    .s_port_data_expo(c_fp_data_expo),
    .s_port_data_sign(c_fp_data_sign),
    .s_port_valid(c_fp_valid),
    .s_port_ready(c_fp_ready),
    .m_port_data_frac(m_fp_data_frac),
    .m_port_data_expo(m_fp_data_expo),
    .m_port_data_sign(m_fp_data_sign),
    .m_port_valid(m_fp_valid),
    .m_port_ready(m_fp_ready)
  );
  sreg_sc_module_10 u_reg_ex(
    .clk(clk),
    .reset(reset),
    .s_port_data(c_ex_data),
    .s_port_valid(c_ex_valid),
    .s_port_ready(c_ex_ready),
    .m_port_data(m_ex_data),
    .m_port_valid(m_ex_valid),
    .m_port_ready(m_ex_ready)
  );
  always @(s_fp_valid or s_fp_data_frac or s_fp_data_expo or s_fp_data_sign or c_fp_ready or c_ex_ready or emax or emax_v or c_sync) begin: mc_proc
    
    
    s_fp_ready <= c_sync;
    c_fp_data_frac <= s_fp_data_frac;
    c_fp_data_expo <= s_fp_data_expo;
    c_fp_data_sign <= s_fp_data_sign;
    c_fp_valid <= c_sync;
    c_ex_data <= emax;
    c_ex_valid <= emax_v;
    c_sync <= ((s_fp_valid) && (c_fp_ready)) && ((!(emax_v)) || (c_ex_ready));
  end
  always @(posedge clk) begin: ms_proc
    logic [0:0] last_local_0;
    logic [51:0] fp_local_1_frac;
    logic [10:0] fp_local_1_expo;
    logic [0:0] fp_local_1_sign;
    logic [10:0] expo_local_2;
    
    if ((reset) == (0)) begin
      count <= (16) - (1);
      emax <= 11'd0;
      emax_v <= 0;
    end else begin
      last_local_0 = (count) == (0);
      fp_local_1_frac = s_fp_data_frac;
      fp_local_1_expo = s_fp_data_expo;
      fp_local_1_sign = s_fp_data_sign;
      if (((fp_local_1_expo) == (0)) && ((fp_local_1_frac) == (0))) begin
        expo_local_2 = fp_local_1_expo;
      end else begin
        expo_local_2 = (fp_local_1_expo) + (11'd1);
      end

      if (c_sync) begin
        if (last_local_0) begin
          count <= (16) - (1);
        end else begin
          count <= (count) - (1);
        end

      end
      if ((emax_v) && (c_ex_ready)) begin
        if (s_fp_valid) begin
          emax <= expo_local_2;
        end else begin
          emax <= 11'd0;
        end

      end else begin
        if ((s_fp_valid) && ((expo_local_2) > (emax))) begin
          emax <= expo_local_2;
        end
      end

      if ((emax_v) && (c_ex_ready)) begin
        emax_v <= 0;
      end else begin
        if ((c_sync) && (last_local_0)) begin
          emax_v <= 1;
        end
      end

    end

  end
endmodule
module rvfifo_cc_sc_module_9 (
  input logic [0:0] clk,
  input logic [0:0] reset,
  input logic [51:0] s_port_data_frac,
  input logic [10:0] s_port_data_expo,
  input logic [0:0] s_port_data_sign,
  input logic [0:0] s_port_valid,
  output logic [0:0] s_port_ready,
  output logic [51:0] m_port_data_frac,
  output logic [10:0] m_port_data_expo,
  output logic [0:0] m_port_data_sign,
  output logic [0:0] m_port_valid,
  input logic [0:0] m_port_ready
);
  logic signed[31:0] MAX_DEPTH = 32;
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
endmodule
module fifo_cc_sc_module_11 (
  input logic [0:0] clk,
  input logic [0:0] reset,
  input logic [51:0] din_frac,
  input logic [10:0] din_expo,
  input logic [0:0] din_sign,
  input logic [0:0] wr_en,
  input logic [0:0] rd_en,
  output logic [0:0] full,
  output logic [51:0] dout_frac,
  output logic [10:0] dout_expo,
  output logic [0:0] dout_sign,
  output logic [0:0] empty
);
  logic [51:0] data_frac[0:31];
  logic [10:0] data_expo[0:31];
  logic [0:0] data_sign[0:31];
  logic [0:0] empty_i;
  logic [0:0] full_i;
  logic [0:0] rd_en_i;
  logic [4:0] rd_idx;
  logic [0:0] wr_en_i;
  logic [4:0] wr_idx;
  logic signed[31:0] MAX_DEPTH = 32;
  initial begin
    if ((32) <= (0)) begin

    end


  end
  always @(wr_en or rd_en or full_i or empty_i or rd_idx or data_frac[(0)] or data_expo[(0)] or data_sign[(0)] or data_frac[(1)] or data_expo[(1)] or data_sign[(1)] or data_frac[(2)] or data_expo[(2)] or data_sign[(2)] or data_frac[(3)] or data_expo[(3)] or data_sign[(3)] or data_frac[(4)] or data_expo[(4)] or data_sign[(4)] or data_frac[(5)] or data_expo[(5)] or data_sign[(5)] or data_frac[(6)] or data_expo[(6)] or data_sign[(6)] or data_frac[(7)] or data_expo[(7)] or data_sign[(7)] or data_frac[(8)] or data_expo[(8)] or data_sign[(8)] or data_frac[(9)] or data_expo[(9)] or data_sign[(9)] or data_frac[(10)] or data_expo[(10)] or data_sign[(10)] or data_frac[(11)] or data_expo[(11)] or data_sign[(11)] or data_frac[(12)] or data_expo[(12)] or data_sign[(12)] or data_frac[(13)] or data_expo[(13)] or data_sign[(13)] or data_frac[(14)] or data_expo[(14)] or data_sign[(14)] or data_frac[(15)] or data_expo[(15)] or data_sign[(15)] or data_frac[(16)] or data_expo[(16)] or data_sign[(16)] or data_frac[(17)] or data_expo[(17)] or data_sign[(17)] or data_frac[(18)] or data_expo[(18)] or data_sign[(18)] or data_frac[(19)] or data_expo[(19)] or data_sign[(19)] or data_frac[(20)] or data_expo[(20)] or data_sign[(20)] or data_frac[(21)] or data_expo[(21)] or data_sign[(21)] or data_frac[(22)] or data_expo[(22)] or data_sign[(22)] or data_frac[(23)] or data_expo[(23)] or data_sign[(23)] or data_frac[(24)] or data_expo[(24)] or data_sign[(24)] or data_frac[(25)] or data_expo[(25)] or data_sign[(25)] or data_frac[(26)] or data_expo[(26)] or data_sign[(26)] or data_frac[(27)] or data_expo[(27)] or data_sign[(27)] or data_frac[(28)] or data_expo[(28)] or data_sign[(28)] or data_frac[(29)] or data_expo[(29)] or data_sign[(29)] or data_frac[(30)] or data_expo[(30)] or data_sign[(30)] or data_frac[(31)] or data_expo[(31)] or data_sign[(31)]) begin: mc_proc
    
    
    if (1) begin
      dout_frac <= data_frac[rd_idx];
      dout_expo <= data_expo[rd_idx];
      dout_sign <= data_sign[rd_idx];
    end
    wr_en_i <= (wr_en) && (!(full_i));
    rd_en_i <= (rd_en) && (!(empty_i));
    full <= (full_i) == (0);
    empty <= (empty_i) == (0);
  end
  always @(posedge clk) begin: ms_proc
    logic [4:0] wr_inc_local_0;
    logic [4:0] rd_inc_local_1;
    logic [31:0] i_local_2;
    
    wr_inc_local_0 = ((wr_idx) + (1)) % (32);
    rd_inc_local_1 = ((rd_idx) + (1)) % (32);
    if ((reset) == (0)) begin
      if (!(1)) begin
        dout_frac <= 0;
        dout_expo <= 0;
        dout_sign <= 0;
      end
      for (i_local_2 = 0;(i_local_2) < (32);i_local_2 = i_local_2 + 1) begin
        data_frac[i_local_2] <= 0;
        data_expo[i_local_2] <= 0;
        data_sign[i_local_2] <= 0;
      end
      rd_idx <= 5'd0;
      wr_idx <= 5'd0;
      full_i <= 0;
      empty_i <= 1;
    end else begin
      if (!(1)) begin
        if (rd_en_i) begin
          dout_frac <= data_frac[rd_idx];
          dout_expo <= data_expo[rd_idx];
          dout_sign <= data_sign[rd_idx];
        end
      end
      if (wr_en_i) begin
        data_frac[wr_idx] <= din_frac;
        data_expo[wr_idx] <= din_expo;
        data_sign[wr_idx] <= din_sign;
        wr_idx <= wr_inc_local_0;
        if (!(rd_en_i)) begin
          if ((wr_inc_local_0) == (rd_idx)) begin
            full_i <= 1;
          end
          empty_i <= 0;
        end
      end
      if (rd_en_i) begin
        rd_idx <= rd_inc_local_1;
        if (!(wr_en_i)) begin
          full_i <= 0;
          if ((rd_inc_local_1) == (wr_idx)) begin
            empty_i <= 1;
          end
        end
      end
    end

  end
endmodule
module sreg_sc_module_10 (
  input logic [0:0] clk,
  input logic [0:0] reset,
  input logic [10:0] s_port_data,
  input logic [0:0] s_port_valid,
  output logic [0:0] s_port_ready,
  output logic [10:0] m_port_data,
  output logic [0:0] m_port_valid,
  input logic [0:0] m_port_ready
);
  logic [10:0] data[0:1];
  logic [0:0] empty_i;
  logic [0:0] full_i;
  logic [0:0] rd_en_i;
  logic [0:0] rd_idx;
  logic [0:0] wr_en_i;
  logic [0:0] wr_idx;
  logic signed[31:0] IW = 1;
  logic [31:0] depth = 2;
  initial begin


  end
  always @(s_port_valid or m_port_ready or full_i or empty_i or rd_idx or data[(0)] or data[(1)]) begin: mc_proc
    
    
    m_port_data <= data[rd_idx];
    wr_en_i <= (s_port_valid) && (!(full_i));
    rd_en_i <= (m_port_ready) && (!(empty_i));
    s_port_ready <= !(full_i);
    m_port_valid <= !(empty_i);
  end
  always @(posedge clk) begin: ms_proc
    logic [0:0] wr_inc_local_0;
    logic [0:0] rd_inc_local_1;
    logic [31:0] i_local_2;
    
    wr_inc_local_0 = ((wr_idx) + (1)) % (2);
    rd_inc_local_1 = ((rd_idx) + (1)) % (2);
    if ((reset) == (0)) begin
      for (i_local_2 = 0;(i_local_2) < (2);i_local_2 = i_local_2 + 1) begin
        data[i_local_2] <= 0;
      end
      rd_idx <= 1'd0;
      wr_idx <= 1'd0;
      full_i <= 0;
      empty_i <= 1;
    end else begin
      if (wr_en_i) begin
        data[wr_idx] <= s_port_data;
        wr_idx <= wr_inc_local_0;
        if (!(rd_en_i)) begin
          if ((wr_inc_local_0) == (rd_idx)) begin
            full_i <= 1;
          end
          empty_i <= 0;
        end
      end
      if (rd_en_i) begin
        rd_idx <= rd_inc_local_1;
        if (!(wr_en_i)) begin
          full_i <= 0;
          if ((rd_inc_local_1) == (wr_idx)) begin
            empty_i <= 1;
          end
        end
      end
    end

  end
endmodule
module ssplit_sc_module_3 (
  input logic [0:0] clk,
  input logic [0:0] reset,
  input logic [10:0] s_port_data,
  input logic [0:0] s_port_valid,
  output logic [0:0] s_port_ready,
  output logic [10:0] m_port1_data,
  output logic [0:0] m_port1_valid,
  input logic [0:0] m_port1_ready,
  output logic [10:0] m_port2_data,
  output logic [0:0] m_port2_valid,
  input logic [0:0] m_port2_ready
);
  logic [10:0] c_port1_data;
  logic [0:0] c_port1_valid;
  logic [0:0] c_port1_ready;
  logic [10:0] c_port2_data;
  logic [0:0] c_port2_valid;
  logic [0:0] c_port2_ready;
  initial begin

  end
  rvfifo_cc_sc_module_12 u_que1(
    .clk(clk),
    .reset(reset),
    .s_port_data(c_port1_data),
    .s_port_valid(c_port1_valid),
    .s_port_ready(c_port1_ready),
    .m_port_data(m_port1_data),
    .m_port_valid(m_port1_valid),
    .m_port_ready(m_port1_ready)
  );
  rvfifo_cc_sc_module_13 u_que2(
    .clk(clk),
    .reset(reset),
    .s_port_data(c_port2_data),
    .s_port_valid(c_port2_valid),
    .s_port_ready(c_port2_ready),
    .m_port_data(m_port2_data),
    .m_port_valid(m_port2_valid),
    .m_port_ready(m_port2_ready)
  );
  always @(s_port_data or s_port_valid or c_port1_ready or c_port2_ready) begin: mc_proc
    
    
    c_port1_data <= s_port_data;
    c_port2_data <= s_port_data;
    c_port1_valid <= (s_port_valid) && (c_port2_ready);
    c_port2_valid <= (s_port_valid) && (c_port1_ready);
    s_port_ready <= (c_port1_ready) && (c_port2_ready);
  end
endmodule
module rvfifo_cc_sc_module_12 (
  input logic [0:0] clk,
  input logic [0:0] reset,
  input logic [10:0] s_port_data,
  input logic [0:0] s_port_valid,
  output logic [0:0] s_port_ready,
  output logic [10:0] m_port_data,
  output logic [0:0] m_port_valid,
  input logic [0:0] m_port_ready
);
  logic signed[31:0] MAX_DEPTH = 2;
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
endmodule
module fifo_cc_sc_module_14 (
  input logic [0:0] clk,
  input logic [0:0] reset,
  input logic [10:0] din,
  input logic [0:0] wr_en,
  input logic [0:0] rd_en,
  output logic [0:0] full,
  output logic [10:0] dout,
  output logic [0:0] empty
);
  logic [10:0] data[0:1];
  logic [0:0] empty_i;
  logic [0:0] full_i;
  logic [0:0] rd_en_i;
  logic [0:0] rd_idx;
  logic [0:0] wr_en_i;
  logic [0:0] wr_idx;
  logic signed[31:0] MAX_DEPTH = 2;
  initial begin
    if ((2) <= (0)) begin

    end


  end
  always @(wr_en or rd_en or full_i or empty_i or rd_idx or data[(0)] or data[(1)]) begin: mc_proc
    
    
    if (1) begin
      dout <= data[rd_idx];
    end
    wr_en_i <= (wr_en) && (!(full_i));
    rd_en_i <= (rd_en) && (!(empty_i));
    full <= (full_i) == (0);
    empty <= (empty_i) == (0);
  end
  always @(posedge clk) begin: ms_proc
    logic [0:0] wr_inc_local_0;
    logic [0:0] rd_inc_local_1;
    logic [31:0] i_local_2;
    
    wr_inc_local_0 = ((wr_idx) + (1)) % (2);
    rd_inc_local_1 = ((rd_idx) + (1)) % (2);
    if ((reset) == (0)) begin
      if (!(1)) begin
        dout <= 0;
      end
      for (i_local_2 = 0;(i_local_2) < (2);i_local_2 = i_local_2 + 1) begin
        data[i_local_2] <= 0;
      end
      rd_idx <= 1'd0;
      wr_idx <= 1'd0;
      full_i <= 0;
      empty_i <= 1;
    end else begin
      if (!(1)) begin
        if (rd_en_i) begin
          dout <= data[rd_idx];
        end
      end
      if (wr_en_i) begin
        data[wr_idx] <= din;
        wr_idx <= wr_inc_local_0;
        if (!(rd_en_i)) begin
          if ((wr_inc_local_0) == (rd_idx)) begin
            full_i <= 1;
          end
          empty_i <= 0;
        end
      end
      if (rd_en_i) begin
        rd_idx <= rd_inc_local_1;
        if (!(wr_en_i)) begin
          full_i <= 0;
          if ((rd_inc_local_1) == (wr_idx)) begin
            empty_i <= 1;
          end
        end
      end
    end

  end
endmodule
module rvfifo_cc_sc_module_13 (
  input logic [0:0] clk,
  input logic [0:0] reset,
  input logic [10:0] s_port_data,
  input logic [0:0] s_port_valid,
  output logic [0:0] s_port_ready,
  output logic [10:0] m_port_data,
  output logic [0:0] m_port_valid,
  input logic [0:0] m_port_ready
);
  logic signed[31:0] MAX_DEPTH = 16;
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
endmodule
module fifo_cc_sc_module_15 (
  input logic [0:0] clk,
  input logic [0:0] reset,
  input logic [10:0] din,
  input logic [0:0] wr_en,
  input logic [0:0] rd_en,
  output logic [0:0] full,
  output logic [10:0] dout,
  output logic [0:0] empty
);
  logic [10:0] data[0:15];
  logic [0:0] empty_i;
  logic [0:0] full_i;
  logic [0:0] rd_en_i;
  logic [3:0] rd_idx;
  logic [0:0] wr_en_i;
  logic [3:0] wr_idx;
  logic signed[31:0] MAX_DEPTH = 16;
  initial begin
    if ((16) <= (0)) begin

    end


  end
  always @(wr_en or rd_en or full_i or empty_i or rd_idx or data[(0)] or data[(1)] or data[(2)] or data[(3)] or data[(4)] or data[(5)] or data[(6)] or data[(7)] or data[(8)] or data[(9)] or data[(10)] or data[(11)] or data[(12)] or data[(13)] or data[(14)] or data[(15)]) begin: mc_proc
    
    
    if (1) begin
      dout <= data[rd_idx];
    end
    wr_en_i <= (wr_en) && (!(full_i));
    rd_en_i <= (rd_en) && (!(empty_i));
    full <= (full_i) == (0);
    empty <= (empty_i) == (0);
  end
  always @(posedge clk) begin: ms_proc
    logic [3:0] wr_inc_local_0;
    logic [3:0] rd_inc_local_1;
    logic [31:0] i_local_2;
    
    wr_inc_local_0 = ((wr_idx) + (1)) % (16);
    rd_inc_local_1 = ((rd_idx) + (1)) % (16);
    if ((reset) == (0)) begin
      if (!(1)) begin
        dout <= 0;
      end
      for (i_local_2 = 0;(i_local_2) < (16);i_local_2 = i_local_2 + 1) begin
        data[i_local_2] <= 0;
      end
      rd_idx <= 4'd0;
      wr_idx <= 4'd0;
      full_i <= 0;
      empty_i <= 1;
    end else begin
      if (!(1)) begin
        if (rd_en_i) begin
          dout <= data[rd_idx];
        end
      end
      if (wr_en_i) begin
        data[wr_idx] <= din;
        wr_idx <= wr_inc_local_0;
        if (!(rd_en_i)) begin
          if ((wr_inc_local_0) == (rd_idx)) begin
            full_i <= 1;
          end
          empty_i <= 0;
        end
      end
      if (rd_en_i) begin
        rd_idx <= rd_inc_local_1;
        if (!(wr_en_i)) begin
          full_i <= 0;
          if ((rd_inc_local_1) == (wr_idx)) begin
            empty_i <= 1;
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
  input logic [10:0] s_ex_data,
  input logic [0:0] s_ex_valid,
  output logic [0:0] s_ex_ready,
  input logic [51:0] s_fp_data_frac,
  input logic [10:0] s_fp_data_expo,
  input logic [0:0] s_fp_data_sign,
  input logic [0:0] s_fp_valid,
  output logic [0:0] s_fp_ready,
  output logic signed[63:0] m_block[0:15],
  output logic [0:0] m_valid
);
  logic signed[63:0] c_int;
  logic [0:0] c_sync;
  logic [3:0] count;
  initial begin


  end
  always @(s_ex_valid or s_ex_data or s_fp_valid or s_fp_data_frac or s_fp_data_expo or s_fp_data_sign or m_ready or count or c_sync or c_int) begin: mc_proc
    logic [10:0] emax_local_0;
    logic [51:0] fp_local_1_frac;
    logic [10:0] fp_local_1_expo;
    logic [0:0] fp_local_1_sign;
    logic [2:0] hid_local_2;
    logic [63:0] ui_local_3;
    logic signed[63:0] si_local_4;
    
    c_sync <= ((s_ex_valid) && (s_fp_valid)) && (m_ready);
    s_ex_ready <= (c_sync) && ((count) == ((16) - (1)));
    s_fp_ready <= c_sync;
    emax_local_0 = s_ex_data;
    if ((emax_local_0) != (0)) begin
      emax_local_0 = (emax_local_0) - (11'd1);
    end
    fp_local_1_frac = s_fp_data_frac;
    fp_local_1_expo = s_fp_data_expo;
    fp_local_1_sign = s_fp_data_sign;
    hid_local_2 = (fp_local_1_expo) != (0);
    ui_local_3 = ({({(hid_local_2), (fp_local_1_frac)}), (9'd0)}) >>> ((emax_local_0) - (fp_local_1_expo));
    if (fp_local_1_sign) begin
      si_local_4 = -(ui_local_3);
    end else begin
      si_local_4 = ui_local_3;
    end

    c_int <= si_local_4;
  end
  always @(posedge clk) begin: ms_proc
    
    
    if ((reset) == (0)) begin
      count <= 4'd0;
      m_valid <= 0;
    end else begin
      if (c_sync) begin
        if ((count) == ((16) - (1))) begin
          count <= 4'd0;
          m_valid <= 1;
        end else begin
          count <= (count) + (1);
          m_valid <= 0;
        end

        m_block[count] = c_int;
      end else begin
        if (m_ready) begin
          m_valid <= 0;
        end
      end

    end

  end
endmodule
module encode_block_sc_module_5 (
  input logic [0:0] clk,
  input logic [0:0] reset,
  input logic signed[63:0] s_block[0:15],
  input logic [0:0] s_valid,
  input logic [0:0] m_ready,
  output logic [0:0] s_ready,
  output logic [63:0] m_block[0:15],
  output logic [0:0] m_valid
);
  logic signed[63:0] c_xt_data[0:3][0:3];
  logic [0:0] c_xt_ready[0:3];
  logic [0:0] c_xt_valid[0:3];
  logic signed[63:0] c_yt_data[0:3][0:3];
  logic [0:0] c_yt_ready[0:3];
  logic [0:0] c_yt_valid[0:3];
  initial begin


  end
  fwd_lift_sc_module_16 u_xt_0(
    .clk(clk),
    .reset(reset),
    .s_valid(s_valid),
    .s_ready(c_xt_ready[0]),
    .m_valid(c_xt_valid[0]),
    .m_ready(m_ready),
    .s_port('{ s_block[((0) * (4)) + (0)],s_block[((0) * (4)) + (1)],s_block[((0) * (4)) + (2)],s_block[((0) * (4)) + (3)] }),
    .m_port('{ c_xt_data[0][0],c_xt_data[0][1],c_xt_data[0][2],c_xt_data[0][3] })
  );
  fwd_lift_sc_module_16 u_xt_1(
    .clk(clk),
    .reset(reset),
    .s_valid(s_valid),
    .s_ready(c_xt_ready[1]),
    .m_valid(c_xt_valid[1]),
    .m_ready(m_ready),
    .s_port('{ s_block[((1) * (4)) + (0)],s_block[((1) * (4)) + (1)],s_block[((1) * (4)) + (2)],s_block[((1) * (4)) + (3)] }),
    .m_port('{ c_xt_data[1][0],c_xt_data[1][1],c_xt_data[1][2],c_xt_data[1][3] })
  );
  fwd_lift_sc_module_16 u_xt_2(
    .clk(clk),
    .reset(reset),
    .s_valid(s_valid),
    .s_ready(c_xt_ready[2]),
    .m_valid(c_xt_valid[2]),
    .m_ready(m_ready),
    .s_port('{ s_block[((2) * (4)) + (0)],s_block[((2) * (4)) + (1)],s_block[((2) * (4)) + (2)],s_block[((2) * (4)) + (3)] }),
    .m_port('{ c_xt_data[2][0],c_xt_data[2][1],c_xt_data[2][2],c_xt_data[2][3] })
  );
  fwd_lift_sc_module_16 u_xt_3(
    .clk(clk),
    .reset(reset),
    .s_valid(s_valid),
    .s_ready(c_xt_ready[3]),
    .m_valid(c_xt_valid[3]),
    .m_ready(m_ready),
    .s_port('{ s_block[((3) * (4)) + (0)],s_block[((3) * (4)) + (1)],s_block[((3) * (4)) + (2)],s_block[((3) * (4)) + (3)] }),
    .m_port('{ c_xt_data[3][0],c_xt_data[3][1],c_xt_data[3][2],c_xt_data[3][3] })
  );
  fwd_lift_sc_module_17 u_yt_0(
    .clk(clk),
    .reset(reset),
    .s_valid(c_xt_valid[0]),
    .s_ready(c_yt_ready[0]),
    .m_valid(c_yt_valid[0]),
    .m_ready(m_ready),
    .s_port('{ c_xt_data[0][0],c_xt_data[1][0],c_xt_data[2][0],c_xt_data[3][0] }),
    .m_port('{ c_yt_data[0][0],c_yt_data[1][0],c_yt_data[2][0],c_yt_data[3][0] })
  );
  fwd_lift_sc_module_17 u_yt_1(
    .clk(clk),
    .reset(reset),
    .s_valid(c_xt_valid[0]),
    .s_ready(c_yt_ready[1]),
    .m_valid(c_yt_valid[1]),
    .m_ready(m_ready),
    .s_port('{ c_xt_data[0][1],c_xt_data[1][1],c_xt_data[2][1],c_xt_data[3][1] }),
    .m_port('{ c_yt_data[0][1],c_yt_data[1][1],c_yt_data[2][1],c_yt_data[3][1] })
  );
  fwd_lift_sc_module_17 u_yt_2(
    .clk(clk),
    .reset(reset),
    .s_valid(c_xt_valid[0]),
    .s_ready(c_yt_ready[2]),
    .m_valid(c_yt_valid[2]),
    .m_ready(m_ready),
    .s_port('{ c_xt_data[0][2],c_xt_data[1][2],c_xt_data[2][2],c_xt_data[3][2] }),
    .m_port('{ c_yt_data[0][2],c_yt_data[1][2],c_yt_data[2][2],c_yt_data[3][2] })
  );
  fwd_lift_sc_module_17 u_yt_3(
    .clk(clk),
    .reset(reset),
    .s_valid(c_xt_valid[0]),
    .s_ready(c_yt_ready[3]),
    .m_valid(c_yt_valid[3]),
    .m_ready(m_ready),
    .s_port('{ c_xt_data[0][3],c_xt_data[1][3],c_xt_data[2][3],c_xt_data[3][3] }),
    .m_port('{ c_yt_data[0][3],c_yt_data[1][3],c_yt_data[2][3],c_yt_data[3][3] })
  );
  always @(m_ready) begin: mc_proc
    
    
    s_ready <= m_ready;
  end
  always @(posedge clk) begin: ms_fwd_order
    
    
    if (m_ready) begin
      m_block[0] = ((c_yt_data[(0)][(0)]) + (-64'd6148914691236517206)) ^ (-64'd6148914691236517206);
      m_block[1] = ((c_yt_data[(0)][(1)]) + (-64'd6148914691236517206)) ^ (-64'd6148914691236517206);
      m_block[2] = ((c_yt_data[(1)][(0)]) + (-64'd6148914691236517206)) ^ (-64'd6148914691236517206);
      m_block[3] = ((c_yt_data[(1)][(1)]) + (-64'd6148914691236517206)) ^ (-64'd6148914691236517206);
      m_block[4] = ((c_yt_data[(0)][(2)]) + (-64'd6148914691236517206)) ^ (-64'd6148914691236517206);
      m_block[5] = ((c_yt_data[(2)][(0)]) + (-64'd6148914691236517206)) ^ (-64'd6148914691236517206);
      m_block[6] = ((c_yt_data[(1)][(2)]) + (-64'd6148914691236517206)) ^ (-64'd6148914691236517206);
      m_block[7] = ((c_yt_data[(2)][(1)]) + (-64'd6148914691236517206)) ^ (-64'd6148914691236517206);
      m_block[8] = ((c_yt_data[(0)][(3)]) + (-64'd6148914691236517206)) ^ (-64'd6148914691236517206);
      m_block[9] = ((c_yt_data[(3)][(0)]) + (-64'd6148914691236517206)) ^ (-64'd6148914691236517206);
      m_block[10] = ((c_yt_data[(2)][(2)]) + (-64'd6148914691236517206)) ^ (-64'd6148914691236517206);
      m_block[11] = ((c_yt_data[(1)][(3)]) + (-64'd6148914691236517206)) ^ (-64'd6148914691236517206);
      m_block[12] = ((c_yt_data[(3)][(1)]) + (-64'd6148914691236517206)) ^ (-64'd6148914691236517206);
      m_block[13] = ((c_yt_data[(2)][(3)]) + (-64'd6148914691236517206)) ^ (-64'd6148914691236517206);
      m_block[14] = ((c_yt_data[(3)][(2)]) + (-64'd6148914691236517206)) ^ (-64'd6148914691236517206);
      m_block[15] = ((c_yt_data[(3)][(3)]) + (-64'd6148914691236517206)) ^ (-64'd6148914691236517206);
      m_valid <= c_yt_valid[0];
    end
  end
endmodule
module fwd_lift_sc_module_16 (
  input logic [0:0] clk,
  input logic [0:0] reset,
  input logic signed[63:0] s_port[0:3],
  input logic [0:0] s_valid,
  input logic [0:0] m_ready,
  output logic [0:0] s_ready,
  output logic signed[63:0] m_port[0:3],
  output logic [0:0] m_valid
);
  logic [5:0] v;
  logic signed[63:0] w[0:5];
  logic signed[63:0] x[0:5];
  logic signed[63:0] y[0:5];
  logic signed[63:0] z[0:5];
  logic signed[31:0] stages = 6;
  initial begin


  end
  always @(m_ready or x[((6) - (1))] or y[((6) - (1))] or z[((6) - (1))] or w[((6) - (1))] or v) begin: mc_proc
    
    
    s_ready <= m_ready;
    m_port[0] = x[(6) - (1)];
    m_port[1] = y[(6) - (1)];
    m_port[2] = z[(6) - (1)];
    m_port[3] = w[(6) - (1)];
    m_valid <= v[(6) - (1)];
  end
  always @(posedge clk) begin: ms_proc
    logic signed[31:0] i_local_0;
    
    if ((reset) == (0)) begin
      for (i_local_0 = 0;(i_local_0) < (6);i_local_0 = i_local_0 + 1) begin
        x[i_local_0] <= 64'd0;
        y[i_local_0] <= 64'd0;
        z[i_local_0] <= 64'd0;
        w[i_local_0] <= 64'd0;
      end
      v <= 6'd0;
    end else begin
      if (m_ready) begin
        x[0] <= ((s_port[(0)]) + (s_port[(3)])) >>> (1);
        y[0] <= s_port[1];
        z[0] <= ((s_port[(2)]) + (s_port[(1)])) >>> (1);
        w[0] <= s_port[3];
        x[1] <= ((x[(0)]) + (z[(0)])) >>> (1);
        y[1] <= (y[(0)]) - (z[(0)]);
        z[1] <= z[0];
        w[1] <= (w[(0)]) - (x[(0)]);
        x[2] <= x[1];
        y[2] <= y[1];
        z[2] <= (z[(1)]) - (x[(1)]);
        w[2] <= ((w[(1)]) + (y[(1)])) >>> (1);
        x[3] <= x[2];
        y[3] <= (y[(2)]) - (w[(2)]);
        z[3] <= z[2];
        w[3] <= w[2];
        x[4] <= x[3];
        y[4] <= y[3];
        z[4] <= z[3];
        w[4] <= (w[(3)]) + ((y[(3)]) >>> (1));
        x[5] <= x[4];
        y[5] <= (y[(4)]) - ((w[(4)]) >>> (1));
        z[5] <= z[4];
        w[5] <= w[4];
        v <= ((v) << (1)) | (s_valid);
      end
    end

  end
endmodule
module fwd_lift_sc_module_17 (
  input logic [0:0] clk,
  input logic [0:0] reset,
  input logic signed[63:0] s_port[0:3],
  input logic [0:0] s_valid,
  input logic [0:0] m_ready,
  output logic [0:0] s_ready,
  output logic signed[63:0] m_port[0:3],
  output logic [0:0] m_valid
);
  logic [5:0] v;
  logic signed[63:0] w[0:5];
  logic signed[63:0] x[0:5];
  logic signed[63:0] y[0:5];
  logic signed[63:0] z[0:5];
  logic signed[31:0] stages = 6;
  initial begin


  end
  always @(m_ready or x[((6) - (1))] or y[((6) - (1))] or z[((6) - (1))] or w[((6) - (1))] or v) begin: mc_proc
    
    
    s_ready <= m_ready;
    m_port[0] = x[(6) - (1)];
    m_port[1] = y[(6) - (1)];
    m_port[2] = z[(6) - (1)];
    m_port[3] = w[(6) - (1)];
    m_valid <= v[(6) - (1)];
  end
  always @(posedge clk) begin: ms_proc
    logic signed[31:0] i_local_0;
    
    if ((reset) == (0)) begin
      for (i_local_0 = 0;(i_local_0) < (6);i_local_0 = i_local_0 + 1) begin
        x[i_local_0] <= 64'd0;
        y[i_local_0] <= 64'd0;
        z[i_local_0] <= 64'd0;
        w[i_local_0] <= 64'd0;
      end
      v <= 6'd0;
    end else begin
      if (m_ready) begin
        x[0] <= ((s_port[(0)]) + (s_port[(3)])) >>> (1);
        y[0] <= s_port[1];
        z[0] <= ((s_port[(2)]) + (s_port[(1)])) >>> (1);
        w[0] <= s_port[3];
        x[1] <= ((x[(0)]) + (z[(0)])) >>> (1);
        y[1] <= (y[(0)]) - (z[(0)]);
        z[1] <= z[0];
        w[1] <= (w[(0)]) - (x[(0)]);
        x[2] <= x[1];
        y[2] <= y[1];
        z[2] <= (z[(1)]) - (x[(1)]);
        w[2] <= ((w[(1)]) + (y[(1)])) >>> (1);
        x[3] <= x[2];
        y[3] <= (y[(2)]) - (w[(2)]);
        z[3] <= z[2];
        w[3] <= w[2];
        x[4] <= x[3];
        y[4] <= y[3];
        z[4] <= z[3];
        w[4] <= (w[(3)]) + ((y[(3)]) >>> (1));
        x[5] <= x[4];
        y[5] <= (y[(4)]) - ((w[(4)]) >>> (1));
        z[5] <= z[4];
        w[5] <= w[4];
        v <= ((v) << (1)) | (s_valid);
      end
    end

  end
endmodule
module block_buffer_sc_module_6 (
  input logic [0:0] clk,
  input logic [0:0] reset,
  input logic [63:0] s_block[0:15],
  input logic [0:0] s_valid,
  input logic [0:0] m_ready,
  output logic [0:0] s_ready,
  output logic [63:0] m_block[0:15],
  output logic [0:0] m_valid
);
  logic [1023:0] c_bi;
  logic [1023:0] c_bo;
  initial begin

  end
  fifo_cc_sc_module_18 u_bbuf(
    .clk(clk),
    .reset(reset),
    .din(c_bi),
    .wr_en(s_valid),
    .full(s_ready),
    .dout(c_bo),
    .rd_en(m_ready),
    .empty(m_valid)
  );
  always @(s_block[(0)] or s_block[(1)] or s_block[(2)] or s_block[(3)] or s_block[(4)] or s_block[(5)] or s_block[(6)] or s_block[(7)] or s_block[(8)] or s_block[(9)] or s_block[(10)] or s_block[(11)] or s_block[(12)] or s_block[(13)] or s_block[(14)] or s_block[(15)] or c_bo) begin: mc_proc
    logic [1023:0] bbufi_local_0;
    logic [1023:0] bbufo_local_1;
    logic signed[31:0] i_local_2;
    logic signed[31:0] i_local_3;
    
    for (i_local_2 = 0;(i_local_2) < (16);i_local_2 = i_local_2 + 1) begin
      bbufi_local_0[((i_local_2) * (64)) +: (64)] = s_block[i_local_2];
    end
    c_bi <= bbufi_local_0;
    bbufo_local_1 = c_bo;
    for (i_local_3 = 0;(i_local_3) < (16);i_local_3 = i_local_3 + 1) begin
      m_block[i_local_3] = bbufo_local_1[((i_local_3) * (64)) +: (64)];
    end
  end
endmodule
module fifo_cc_sc_module_18 (
  input logic [0:0] clk,
  input logic [0:0] reset,
  input logic [1023:0] din,
  input logic [0:0] wr_en,
  input logic [0:0] rd_en,
  output logic [0:0] full,
  output logic [1023:0] dout,
  output logic [0:0] empty
);
  logic [1023:0] data[0:1];
  logic [0:0] empty_i;
  logic [0:0] full_i;
  logic [0:0] rd_en_i;
  logic [0:0] rd_idx;
  logic [0:0] wr_en_i;
  logic [0:0] wr_idx;
  logic signed[31:0] MAX_DEPTH = 2;
  initial begin
    if ((2) <= (0)) begin

    end


  end
  always @(wr_en or rd_en or full_i or empty_i or rd_idx or data[(0)] or data[(1)]) begin: mc_proc
    
    
    if (1) begin
      dout <= data[rd_idx];
    end
    wr_en_i <= (wr_en) && (!(full_i));
    rd_en_i <= (rd_en) && (!(empty_i));
    full <= (full_i) == (0);
    empty <= (empty_i) == (0);
  end
  always @(posedge clk) begin: ms_proc
    logic [0:0] wr_inc_local_0;
    logic [0:0] rd_inc_local_1;
    logic [31:0] i_local_2;
    
    wr_inc_local_0 = ((wr_idx) + (1)) % (2);
    rd_inc_local_1 = ((rd_idx) + (1)) % (2);
    if ((reset) == (0)) begin
      if (!(1)) begin
        dout <= 0;
      end
      for (i_local_2 = 0;(i_local_2) < (2);i_local_2 = i_local_2 + 1) begin
        data[i_local_2] <= 0;
      end
      rd_idx <= 1'd0;
      wr_idx <= 1'd0;
      full_i <= 0;
      empty_i <= 1;
    end else begin
      if (!(1)) begin
        if (rd_en_i) begin
          dout <= data[rd_idx];
        end
      end
      if (wr_en_i) begin
        data[wr_idx] <= din;
        wr_idx <= wr_inc_local_0;
        if (!(rd_en_i)) begin
          if ((wr_inc_local_0) == (rd_idx)) begin
            full_i <= 1;
          end
          empty_i <= 0;
        end
      end
      if (rd_en_i) begin
        rd_idx <= rd_inc_local_1;
        if (!(wr_en_i)) begin
          full_i <= 0;
          if ((rd_inc_local_1) == (wr_idx)) begin
            empty_i <= 1;
          end
        end
      end
    end

  end
endmodule
module encode_ints_sc_module_7 (
  input logic [0:0] clk,
  input logic [0:0] reset,
  input logic [63:0] s_block[0:15],
  input logic [0:0] s_valid,
  input logic [0:0] s_flush,
  input logic [0:0] m_ready,
  output logic [0:0] s_ready,
  output logic [5:0] m_bc,
  output logic [32:0] m_bp,
  output logic [0:0] m_last,
  output logic [0:0] m_valid
);
  logic [4:0] bc1;
  logic [15:0] bp1;
  logic [15:0] c_bplane[0:63];
  logic [0:0] flush1;
  logic [5:0] k0;
  logic [0:0] last1;
  logic [4:0] n0;
  logic [4:0] n1;
  logic [0:0] valid1;
  logic signed[31:0] planes = 64;
  initial begin


  end
  always @(s_block[(0)] or s_block[(1)] or s_block[(2)] or s_block[(3)] or s_block[(4)] or s_block[(5)] or s_block[(6)] or s_block[(7)] or s_block[(8)] or s_block[(9)] or s_block[(10)] or s_block[(11)] or s_block[(12)] or s_block[(13)] or s_block[(14)] or s_block[(15)] or k0 or flush1 or m_ready or s_valid or s_flush) begin: mc_proc
    logic [0:0] last_local_0;
    logic [15:0] tmp_local_1;
    logic signed[31:0] j_local_2;
    logic signed[31:0] i_local_3;
    
    last_local_0 = ((k0) == ((64) - (1))) || ((s_flush) && ((s_flush) != (flush1)));
    for (j_local_2 = 0;(j_local_2) < (64);j_local_2 = j_local_2 + 1) begin
      for (i_local_3 = 0;(i_local_3) < (16);i_local_3 = i_local_3 + 1) begin
        tmp_local_1[i_local_3] = s_block[(i_local_3)][j_local_2];
      end
      c_bplane[((64) - (1)) - (j_local_2)] <= tmp_local_1;
    end
    s_ready <= ((last_local_0) && (m_ready)) || (!(s_valid));
  end
  always @(posedge clk) begin: ms_proc
    logic [0:0] last_local_0;
    logic [31:0] b_local_1;
    logic [0:0] frst_local_2;
    logic [31:0] i_local_3;
    logic [32:0] tmp_local_4;
    logic [31:0] n_local_5;
    logic [31:0] shiftamt_local_6;
    logic [31:0] n1_shiftamt_local_7;
    logic [31:0] first_after_bc_local_8;
    logic [31:0] last_n_local_9;
    logic [0:0] found_local_10;
    logic [32:0] shiftmask_local_11;
    logic [31:0] delta_local_12;
    logic [32:0] shiftmask_2_local_13;
    logic [31:0] j_local_14;
    
    if ((reset) == (0)) begin
      k0 <= 6'd0;
      n0 <= 5'd0;
      n1 <= 5'd0;
      bc1 <= 5'd0;
      m_bc <= 6'd0;
      bp1 <= 16'd0;
      m_bp <= 33'd0;
      last1 <= 0;
      m_last <= 0;
      valid1 <= 0;
      m_valid <= 0;
    end else begin
      if (m_ready) begin
        last_local_0 = ((k0) == ((64) - (1))) || ((s_flush) && ((s_flush) != (flush1)));
        b_local_1 = 0;
        frst_local_2 = 1;
        for (i_local_3 = 16;(i_local_3) > (0);i_local_3 = i_local_3 - 1) begin
          if (c_bplane[(k0)][((i_local_3) - (1))]) begin
            if (frst_local_2) begin
              frst_local_2 = 0;
              b_local_1 = i_local_3;
            end
          end
        end
        n1 <= n0;
        bc1 <= b_local_1;
        bp1 <= c_bplane[k0];
        last1 <= last_local_0;
        valid1 <= s_valid;
        flush1 <= s_flush;
        if (s_valid) begin
          if ((b_local_1) > (n0)) begin
            n0 <= b_local_1;
          end
          if (last_local_0) begin
            k0 <= 6'd0;
            n0 <= 5'd0;
          end else begin
            k0 <= (k0) + (1);
          end

        end
        tmp_local_4 = 33'd0;
        shiftamt_local_6 = 0;
        n1_shiftamt_local_7 = 0;
        first_after_bc_local_8 = n1;
        last_n_local_9 = n1;
        found_local_10 = 0;
        shiftamt_local_6 = 0;
        for (n_local_5 = 0;(n_local_5) < (16);n_local_5 = n_local_5 + 1) begin
          if ((n_local_5) == (n1)) begin
            n1_shiftamt_local_7 = shiftamt_local_6;
          end
          if ((n_local_5) == (0)) begin
            shiftamt_local_6 = (shiftamt_local_6) + (2);
            if (bp1[(0)]) begin
              tmp_local_4[0] = 1;
              tmp_local_4[1] = 1;
            end else begin
              tmp_local_4[0] = 1;
              tmp_local_4[1] = 0;
            end

          end else begin
            if ((bp1[n_local_5]) && (bp1[(n_local_5) - (1)])) begin
              tmp_local_4[shiftamt_local_6] = 1;
              tmp_local_4[(shiftamt_local_6) + (1)] = 1;
              shiftamt_local_6 = (shiftamt_local_6) + (2);
            end else begin
              if ((bp1[n_local_5]) && (!(bp1[(n_local_5) - (1)]))) begin
                if ((n_local_5) != ((16) - (1))) begin
                  tmp_local_4[shiftamt_local_6] = 1;
                  shiftamt_local_6 = (shiftamt_local_6) + (1);
                end
              end else begin
                if ((!(bp1[n_local_5])) && (!(bp1[(n_local_5) - (1)]))) begin
                  tmp_local_4[shiftamt_local_6] = 0;
                  shiftamt_local_6 = (shiftamt_local_6) + (1);
                end else begin
                  if ((!(bp1[n_local_5])) && (bp1[(n_local_5) - (1)])) begin
                    tmp_local_4[shiftamt_local_6] = 1;
                    tmp_local_4[(shiftamt_local_6) + (1)] = 0;
                    shiftamt_local_6 = (shiftamt_local_6) + (2);
                  end
                end

              end

            end

          end

          if ((((!(found_local_10)) && (bp1[n_local_5])) && ((n_local_5) >= ((bc1) - (1)))) && ((n1) < (bc1))) begin
            found_local_10 = 1;
            if ((bp1[n_local_5]) && (!(bp1[(n_local_5) - (1)]))) begin
              if ((n_local_5) != ((16) - (1))) begin
                first_after_bc_local_8 = shiftamt_local_6;
              end else begin
                first_after_bc_local_8 = (shiftamt_local_6) - (1);
              end

            end else begin
              if ((n_local_5) != ((16) - (1))) begin
                first_after_bc_local_8 = shiftamt_local_6;
              end else begin
                first_after_bc_local_8 = (shiftamt_local_6) - (1);
              end

            end

            last_n_local_9 = (n_local_5) + (1);
          end
        end
        shiftmask_local_11 = -(1);
        if ((first_after_bc_local_8) < (33)) begin
          tmp_local_4 = (tmp_local_4) & (~((shiftmask_local_11) << (first_after_bc_local_8)));
        end
        delta_local_12 = 0;
        if ((last_n_local_9) != (16)) begin
          tmp_local_4[first_after_bc_local_8] = 0;
        end
        if ((n1) >= (bc1)) begin
          tmp_local_4 = 0;
        end else begin
          if ((n1) == (0)) begin

          end else begin
            if (((!(bp1[(n1)])) && (!(bp1[((n1) - (1))]))) || ((bp1[(n1)]) && (!(bp1[((n1) - (1))])))) begin
              tmp_local_4 = ((((tmp_local_4) >>> (n1_shiftamt_local_7)) << (1)) | (1)) << (n1);
              delta_local_12 = 1;
            end else begin
              tmp_local_4 = ((tmp_local_4) >>> (n1_shiftamt_local_7)) << (n1);
            end

          end

        end

        shiftmask_2_local_13 = -(1);
        tmp_local_4 = ((tmp_local_4) & ((shiftmask_2_local_13) << (n1))) | ((bp1) & (~((shiftmask_2_local_13) << (n1))));
        j_local_14 = 0;
        if ((n1) >= (bc1)) begin
          j_local_14 = n1;
          if ((n1) != (16)) begin
            j_local_14 = (j_local_14) + (1);
          end
        end else begin
          j_local_14 = (((first_after_bc_local_8) + (delta_local_12)) - (n1_shiftamt_local_7)) + (n1);
          if ((last_n_local_9) != (16)) begin
            j_local_14 = (j_local_14) + (1);
          end
        end

        m_bc <= j_local_14;
        m_bp <= tmp_local_4;
        m_last <= last1;
        m_valid <= valid1;
      end else begin
        if (!(s_flush)) begin
          flush1 <= 0;
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
  input logic [10:0] s_ex_data,
  input logic [0:0] s_ex_valid,
  output logic [0:0] s_ex_ready,
  output logic [0:0] s_ready,
  output logic [0:0] m_flush,
  output logic [63:0] m_bits_data_tdata,
  output logic [0:0] m_bits_data_tlast,
  output logic [0:0] m_bits_valid,
  input logic [0:0] m_bits_ready
);
  logic [2:0] cs_s;
  logic [6:0] cs_prec;
  logic [6:0] cs_planes;
  logic [127:0] cs_buf;
  logic [11:0] cs_bits;
  logic [7:0] cs_posr;
  logic [7:0] cs_posw;
  logic [2:0] ns_s;
  logic [6:0] ns_prec;
  logic [6:0] ns_planes;
  logic [127:0] ns_buf;
  logic [11:0] ns_bits;
  logic [7:0] ns_posr;
  logic [7:0] ns_posw;
  logic signed[31:0] pre_w = 7;
  logic signed[31:0] tbc_w = 12;
  logic signed[31:0] buf_w = 128;
  logic signed[31:0] pos_w = 8;
  initial begin


  end
  always @(s_ex_valid or s_ex_data or m_bits_ready or minbits or maxbits or maxprec or minexp or s_bc or s_bp or s_last or s_valid or cs_s or cs_prec or cs_planes or cs_buf or cs_bits or cs_posr or cs_posw) begin: mc_proc
    logic [2:0] ts_local_0_s;
    logic [6:0] ts_local_0_prec;
    logic [6:0] ts_local_0_planes;
    logic [127:0] ts_local_0_buf;
    logic [11:0] ts_local_0_bits;
    logic [7:0] ts_local_0_posr;
    logic [7:0] ts_local_0_posw;
    logic [0:0] dis_done_local_1;
    logic [0:0] add_done_local_2;
    logic [0:0] pad_done_local_3;
    logic [0:0] fls_done_local_4;
    logic [11:0] pbits_local_5;
    logic [11:0] abits_local_6;
    logic [11:0] n_local_7;
    logic [7:0] ts_local_0_posw__ref_0;
    logic [7:0] ts_local_0_posr__ref_0;
    logic [6:0] ts_local_0_prec__ref_0;
    logic [6:0] ts_local_0_planes__ref_0;
    logic [127:0] ts_local_0_buf__ref_0;
    logic [2:0] ts_local_0_s__ref_0;
    logic [11:0] ts_local_0_bits__ref_0;
    
    ts_local_0_bits__ref_0 = ts_local_0_bits;
    ts_local_0_s__ref_0 = ts_local_0_s;
    ts_local_0_buf__ref_0 = ts_local_0_buf;
    ts_local_0_planes__ref_0 = ts_local_0_planes;
    ts_local_0_prec__ref_0 = ts_local_0_prec;
    ts_local_0_posr__ref_0 = ts_local_0_posr;
    ts_local_0_posw__ref_0 = ts_local_0_posw;
    s_ex_ready <= 0;
    s_ready <= 0;
    m_flush <= 0;
    m_bits_valid <= 0;
    ts_local_0_s__ref_0 = cs_s;
    ts_local_0_prec__ref_0 = cs_prec;
    ts_local_0_planes__ref_0 = cs_planes;
    ts_local_0_buf__ref_0 = cs_buf;
    ts_local_0_bits__ref_0 = cs_bits;
    ts_local_0_posr__ref_0 = cs_posr;
    ts_local_0_posw__ref_0 = cs_posw;
    case(ts_local_0_s__ref_0)
      0: begin
        ts_local_0_prec__ref_0 = sc_dtsc_min(maxprec,sc_dtsc_max(0,((s_ex_data) - (minexp)) + ((2) * ((2) + (1)))));
        ts_local_0_bits__ref_0 = 0;
        ts_local_0_planes__ref_0 = 0;
        ts_local_0_buf__ref_0 = 0;
        ts_local_0_posr__ref_0 = 0;
        ts_local_0_posw__ref_0 = 0;
        if (s_ex_valid) begin
          if (((s_ex_data) != (0)) && ((ts_local_0_prec__ref_0) != (0))) begin
            ts_local_0_s__ref_0 = 2;
          end else begin
            ts_local_0_s__ref_0 = 1;
          end

        end
      end
      1: begin
        ts_local_0_buf__ref_0[0] = 0;
        ts_local_0_bits__ref_0 = 1;
        ts_local_0_posw__ref_0 = 1;
        s_ex_ready <= 1;
        ts_local_0_s__ref_0 = 4;
      end
      2: begin
        ts_local_0_buf__ref_0[0] = 1;
        ts_local_0_buf__ref_0[11:1] = s_ex_data;
        ts_local_0_bits__ref_0 = (11) + (1);
        ts_local_0_posw__ref_0 = (11) + (1);
        s_ex_ready <= 1;
        if ((ts_local_0_planes__ref_0) < (ts_local_0_prec__ref_0)) begin
          ts_local_0_s__ref_0 = 3;
        end else begin
          ts_local_0_s__ref_0 = 4;
        end

      end
      3: begin
        if (s_valid) begin
          if (zhwencode_streamfp_t11_52_2_bits_t64pack_bits(ts_local_0_s__ref_0,ts_local_0_prec__ref_0,ts_local_0_planes__ref_0,ts_local_0_buf__ref_0,ts_local_0_bits__ref_0,ts_local_0_posr__ref_0,ts_local_0_posw__ref_0,s_bc,s_bp,ts_local_0_s__ref_0,ts_local_0_prec__ref_0,ts_local_0_planes__ref_0,ts_local_0_buf__ref_0,ts_local_0_bits__ref_0,ts_local_0_posr__ref_0,ts_local_0_posw__ref_0)) begin
            s_ready <= 1;
            if (s_last) begin
              ts_local_0_planes__ref_0 = 64;
            end else begin
              ts_local_0_planes__ref_0 = (ts_local_0_planes__ref_0) + (1);
            end

          end
        end else begin
          s_ready <= 1;
        end

        dis_done_local_1 = (ts_local_0_planes__ref_0) == (64);
        add_done_local_2 = ((ts_local_0_planes__ref_0) >= (ts_local_0_prec__ref_0)) || ((ts_local_0_bits__ref_0) >= (maxbits));
        pad_done_local_3 = (ts_local_0_bits__ref_0) >= (minbits);
        fls_done_local_4 = zhwencode_streamfp_t11_52_2_bits_t64out_bits(ts_local_0_s__ref_0,ts_local_0_prec__ref_0,ts_local_0_planes__ref_0,ts_local_0_buf__ref_0,ts_local_0_bits__ref_0,ts_local_0_posr__ref_0,ts_local_0_posw__ref_0,(add_done_local_2) && (pad_done_local_3),ts_local_0_s__ref_0,ts_local_0_prec__ref_0,ts_local_0_planes__ref_0,ts_local_0_buf__ref_0,ts_local_0_bits__ref_0,ts_local_0_posr__ref_0,ts_local_0_posw__ref_0);
        if (add_done_local_2) begin
          if (((dis_done_local_1) && (pad_done_local_3)) && (fls_done_local_4)) begin
            ts_local_0_s__ref_0 = 0;
          end else begin
            ts_local_0_s__ref_0 = 4;
          end

        end
      end
      4: begin
        if (s_valid) begin
          if ((ts_local_0_planes__ref_0) < (64)) begin
            if ((ts_local_0_planes__ref_0) < ((64) - (2))) begin
              m_flush <= 1;
            end
            s_ready <= 1;
            if (s_last) begin
              ts_local_0_planes__ref_0 = 64;
            end else begin
              ts_local_0_planes__ref_0 = (ts_local_0_planes__ref_0) + (1);
            end

          end
        end else begin
          s_ready <= 1;
        end

        if ((ts_local_0_bits__ref_0) < (minbits)) begin
          pbits_local_5 = (minbits) - (ts_local_0_bits__ref_0);
          abits_local_6 = (64) - ((ts_local_0_bits__ref_0) & ((64) - (1)));
          n_local_7 = sc_dtsc_min(pbits_local_5,abits_local_6);
          zhwencode_streamfp_t11_52_2_bits_t64pack_bits(ts_local_0_s__ref_0,ts_local_0_prec__ref_0,ts_local_0_planes__ref_0,ts_local_0_buf__ref_0,ts_local_0_bits__ref_0,ts_local_0_posr__ref_0,ts_local_0_posw__ref_0,n_local_7,64'd0,ts_local_0_s__ref_0,ts_local_0_prec__ref_0,ts_local_0_planes__ref_0,ts_local_0_buf__ref_0,ts_local_0_bits__ref_0,ts_local_0_posr__ref_0,ts_local_0_posw__ref_0);
        end
        dis_done_local_1 = (ts_local_0_planes__ref_0) == (64);
        pad_done_local_3 = (ts_local_0_bits__ref_0) >= (minbits);
        fls_done_local_4 = zhwencode_streamfp_t11_52_2_bits_t64out_bits(ts_local_0_s__ref_0,ts_local_0_prec__ref_0,ts_local_0_planes__ref_0,ts_local_0_buf__ref_0,ts_local_0_bits__ref_0,ts_local_0_posr__ref_0,ts_local_0_posw__ref_0,pad_done_local_3,ts_local_0_s__ref_0,ts_local_0_prec__ref_0,ts_local_0_planes__ref_0,ts_local_0_buf__ref_0,ts_local_0_bits__ref_0,ts_local_0_posr__ref_0,ts_local_0_posw__ref_0);
        if (((dis_done_local_1) && (pad_done_local_3)) && (fls_done_local_4)) begin
          ts_local_0_s__ref_0 = 0;
        end
      end
    endcase
    ns_s <= ts_local_0_s__ref_0;
    ns_prec <= ts_local_0_prec__ref_0;
    ns_planes <= ts_local_0_planes__ref_0;
    ns_buf <= ts_local_0_buf__ref_0;
    ns_bits <= ts_local_0_bits__ref_0;
    ns_posr <= ts_local_0_posr__ref_0;
    ns_posw <= ts_local_0_posw__ref_0;
    ts_local_0_posw = ts_local_0_posw__ref_0;
    ts_local_0_posr = ts_local_0_posr__ref_0;
    ts_local_0_prec = ts_local_0_prec__ref_0;
    ts_local_0_planes = ts_local_0_planes__ref_0;
    ts_local_0_buf = ts_local_0_buf__ref_0;
    ts_local_0_s = ts_local_0_s__ref_0;
    ts_local_0_bits = ts_local_0_bits__ref_0;
  end
  always @(posedge clk) begin: ms_proc
    
    
    if ((reset) == (0)) begin
      cs_s <= 0;
      cs_prec <= 0;
      cs_planes <= 0;
      cs_buf <= 0;
      cs_bits <= 0;
      cs_posr <= 0;
      cs_posw <= 0;
    end else begin
      cs_s <= ns_s;
      cs_prec <= ns_prec;
      cs_planes <= ns_planes;
      cs_buf <= ns_buf;
      cs_bits <= ns_bits;
      cs_posr <= ns_posr;
      cs_posw <= ns_posw;
    end

  end
  function automatic logic [11:0] sc_dtsc_min (input logic [11:0] a, input logic [11:0] b);
  begin

    return (a) <= (b) ? a : b;
  end
  endfunction
  function automatic logic signed[15:0] sc_dtsc_max (input logic signed[15:0] a, input logic signed[15:0] b);
  begin

    return (a) >= (b) ? a : b;
  end
  endfunction
  function automatic logic [0:0] zhwencode_streamfp_t11_52_2_bits_t64pack_bits (input logic [2:0] ts_s, input logic [6:0] ts_prec, input logic [6:0] ts_planes, input logic [127:0] ts_buf, input logic [11:0] ts_bits, input logic [7:0] ts_posr, input logic [7:0] ts_posw, input logic [11:0] bc, input logic [127:0] bp, inout logic [2:0] ts_s__ref_0, inout logic [6:0] ts_prec__ref_0, inout logic [6:0] ts_planes__ref_0, inout logic [127:0] ts_buf__ref_0, inout logic [11:0] ts_bits__ref_0, inout logic [7:0] ts_posr__ref_0, inout logic [7:0] ts_posw__ref_0);
  begin
    logic [11:0] n_local_0;
    n_local_0 = sc_dtsc_min(bc,(maxbits) - (ts_bits));
    if (((128) - (ts_posw)) >= (n_local_0)) begin
      ts_buf__ref_0 = ((((bp) >> (0)) & ~(~($bits(bp)'('b0)) << (((n_local_0) - (1)) - (0) + 1)) & ~(~($bits(bp)'('b0)) << ((((ts_posw) + (n_local_0)) - (1))-(ts_posw)+1))) << (ts_posw)) | ((ts_buf__ref_0) & ((~($bits(ts_buf__ref_0)'('b0)) ) << ((((ts_posw) + (n_local_0)) - (1)) + 1) | ~(( ~($bits(ts_buf__ref_0)'('b0)) ) << (ts_posw))));
      ts_bits__ref_0 = (ts_bits) + (n_local_0);
      ts_posw__ref_0 = (ts_posw) + (n_local_0);
      return 1;
    end
    return 0;
  end
  endfunction
  function automatic logic [0:0] zhwencode_streamfp_t11_52_2_bits_t64out_bits (input logic [2:0] ts_s, input logic [6:0] ts_prec, input logic [6:0] ts_planes, input logic [127:0] ts_buf, input logic [11:0] ts_bits, input logic [7:0] ts_posr, input logic [7:0] ts_posw, input logic [0:0] done, inout logic [2:0] ts_s__ref_0, inout logic [6:0] ts_prec__ref_0, inout logic [6:0] ts_planes__ref_0, inout logic [127:0] ts_buf__ref_0, inout logic [11:0] ts_bits__ref_0, inout logic [7:0] ts_posr__ref_0, inout logic [7:0] ts_posw__ref_0);
  begin
    logic [7:0] bbits_local_0;
    logic [63:0] flit_local_1_tdata;
    logic [0:0] flit_local_1_tlast;
    bbits_local_0 = (ts_posw) - (ts_posr);
    if (((bbits_local_0) >= (64)) && (m_bits_ready)) begin
      flit_local_1_tdata = ((ts_buf__ref_0) >> (ts_posr)) & ~(~($bits(ts_buf__ref_0)'('b0)) << ((((ts_posr) + (64)) - (1)) - (ts_posr) + 1));
      flit_local_1_tlast = (done) && ((bbits_local_0) == (64));
      m_bits_data_tdata <= flit_local_1_tdata;
      m_bits_data_tlast <= flit_local_1_tlast;
      m_bits_valid <= 1;
      ts_buf__ref_0 = ((((ts_buf__ref_0) >> ((ts_posr) + (64))) & ~(~($bits(ts_buf__ref_0)'('b0)) << (((ts_posw) - (1)) - ((ts_posr) + (64)) + 1)) & ~(~($bits(ts_buf__ref_0)'('b0)) << (((ts_posw) - (64))-(0)+1))) << (0)) | ((ts_buf__ref_0) & ((~($bits(ts_buf__ref_0)'('b0)) ) << (((ts_posw) - (64)) + 1) | ~(( ~($bits(ts_buf__ref_0)'('b0)) ) << (0))));
      ts_posr__ref_0 = 0;
      ts_posw__ref_0 = (bbits_local_0) - (64);
      return flit_local_1_tlast;
    end
    return (done) && ((bbits_local_0) == (0));
  end
  endfunction
endmodule