module mymodule_sc_module_0 (
);
  logic [0:0] clk_scclang_global_0;
  logic [0:0] reset_scclang_global_1;
  logic [63:0] driv_data_scclang_global_2_data;
  logic [0:0] driv_data_scclang_global_2_valid;
  logic [0:0] driv_data_scclang_global_2_ready;
  logic [63:0] dut_min_scclang_global_3_data;
  logic [0:0] dut_min_scclang_global_3_valid;
  logic [0:0] dut_min_scclang_global_3_ready;
  logic [63:0] dut_max_scclang_global_4_data;
  logic [0:0] dut_max_scclang_global_4_valid;
  logic [0:0] dut_max_scclang_global_4_ready;
  logic [63:0] dut_avg_scclang_global_5_data;
  logic [0:0] dut_avg_scclang_global_5_valid;
  logic [0:0] dut_avg_scclang_global_5_ready;
  moving_average_sc_module_1 u_dut(
    .clk(clk_scclang_global_0),
    .reset(reset_scclang_global_1),
    .datastrm_data(driv_data_scclang_global_2_data),
    .datastrm_valid(driv_data_scclang_global_2_valid),
    .datastrm_ready(driv_data_scclang_global_2_ready),
    .min_out_data(dut_min_scclang_global_3_data),
    .min_out_valid(dut_min_scclang_global_3_valid),
    .min_out_ready(dut_min_scclang_global_3_ready),
    .max_out_data(dut_max_scclang_global_4_data),
    .max_out_valid(dut_max_scclang_global_4_valid),
    .max_out_ready(dut_max_scclang_global_4_ready),
    .avg_out_data(dut_avg_scclang_global_5_data),
    .avg_out_valid(dut_avg_scclang_global_5_valid),
    .avg_out_ready(dut_avg_scclang_global_5_ready)
  );
  always @(*) begin
  end

endmodule
module moving_average_sc_module_1 (
  input logic [0:0] clk,
  input logic [0:0] reset,
  input logic [63:0] datastrm_data,
  input logic [0:0] datastrm_valid,
  output logic [0:0] datastrm_ready,
  output logic [63:0] min_out_data,
  output logic [0:0] min_out_valid,
  input logic [0:0] min_out_ready,
  output logic [63:0] max_out_data,
  output logic [0:0] max_out_valid,
  input logic [0:0] max_out_ready,
  output logic [63:0] avg_out_data,
  output logic [0:0] avg_out_valid,
  input logic [0:0] avg_out_ready
);
  logic [63:0] cur_avg_scclang_global_0;
  logic [63:0] cur_avg_pipe_scclang_global_1[0:1];
  logic [63:0] cur_max_scclang_global_2;
  logic [63:0] cur_max_pipe_scclang_global_3[0:1];
  logic [63:0] cur_min_scclang_global_4;
  logic [63:0] cur_min_pipe_scclang_global_5[0:1];
  logic [0:0] datardy_scclang_global_6;
  logic [63:0] datastrm_data_pipe_scclang_global_7[0:1];
  logic [0:0] datastrm_valid_pipe_scclang_global_8[0:3];
  logic [7:0] insert_scclang_global_9;
  logic [7:0] n_scclang_global_10;
  logic [63:0] sum_scclang_global_11;
  logic [63:0] window_scclang_global_12[0:63];
  logic signed[31:0] PIPE_DEPTH_scclang_global_13 = 2;
  logic signed[31:0] PIPE_DEPTH_2_scclang_global_14 = 2;
  always_ff @(posedge clk) begin: mc_proc
    
    
    min_out_valid <= datastrm_valid_pipe_scclang_global_8[((PIPE_DEPTH_scclang_global_13) + (PIPE_DEPTH_2_scclang_global_14)) - (1)];
    min_out_data <= cur_min_pipe_scclang_global_5[(PIPE_DEPTH_2_scclang_global_14) - (1)];
    max_out_valid <= datastrm_valid_pipe_scclang_global_8[((PIPE_DEPTH_scclang_global_13) + (PIPE_DEPTH_2_scclang_global_14)) - (1)];
    max_out_data <= cur_max_pipe_scclang_global_3[(PIPE_DEPTH_2_scclang_global_14) - (1)];
    avg_out_valid <= datastrm_valid_pipe_scclang_global_8[((PIPE_DEPTH_scclang_global_13) + (PIPE_DEPTH_2_scclang_global_14)) - (1)];
    avg_out_data <= cur_avg_pipe_scclang_global_1[(PIPE_DEPTH_2_scclang_global_14) - (1)];
    datastrm_ready <= 1;
  end
  always_ff @(posedge clk) begin: ms_proc
    logic signed[31:0] i_ms_proc_local_2;
    logic signed[31:0] i_ms_proc_local_3;
    logic signed[31:0] i_ms_proc_local_4;
    logic signed[31:0] i_ms_proc_local_5;
    logic signed[31:0] i_ms_proc_local_6;
    
    if ((reset) == (0)) begin
      n_scclang_global_10 <= 8'd0;
      insert_scclang_global_9 <= 8'd0;
      cur_avg_scclang_global_0 <= 64'd0;
      cur_max_scclang_global_2 <= cur_avg_scclang_global_0;
      cur_min_scclang_global_4 <= cur_max_scclang_global_2;
      sum_scclang_global_11 <= 64'd0;
      for (i_ms_proc_local_2 = 0;(i_ms_proc_local_2) < (64);i_ms_proc_local_2++) begin
        window_scclang_global_12[i_ms_proc_local_2] <= 64'd0;
      end
      for (i_ms_proc_local_3 = 0;(i_ms_proc_local_3) < ((PIPE_DEPTH_scclang_global_13) + (PIPE_DEPTH_2_scclang_global_14));i_ms_proc_local_3++) begin
        datastrm_valid_pipe_scclang_global_8[i_ms_proc_local_3] <= 0;
      end
      datardy_scclang_global_6 <= 1;
    end else begin
      datastrm_data_pipe_scclang_global_7[0] <= datastrm_data;
      datastrm_valid_pipe_scclang_global_8[0] <= datastrm_valid;
      for (i_ms_proc_local_4 = 1;(i_ms_proc_local_4) < (PIPE_DEPTH_scclang_global_13);i_ms_proc_local_4++) begin
        datastrm_data_pipe_scclang_global_7[i_ms_proc_local_4] <= datastrm_data_pipe_scclang_global_7[(i_ms_proc_local_4) - (1)];
        datastrm_valid_pipe_scclang_global_8[i_ms_proc_local_4] <= datastrm_valid_pipe_scclang_global_8[(i_ms_proc_local_4) - (1)];
      end
      if (datastrm_valid_pipe_scclang_global_8[(PIPE_DEPTH_scclang_global_13) - (1)]) begin
        if ((cur_min_scclang_global_4) > (datastrm_data_pipe_scclang_global_7[(PIPE_DEPTH_scclang_global_13) - (1)])) begin
          cur_min_scclang_global_4 <= datastrm_data_pipe_scclang_global_7[(PIPE_DEPTH_scclang_global_13) - (1)];
        end
        if ((cur_max_scclang_global_2) < (datastrm_data_pipe_scclang_global_7[(PIPE_DEPTH_scclang_global_13) - (1)])) begin
          cur_max_scclang_global_2 <= datastrm_data_pipe_scclang_global_7[(PIPE_DEPTH_scclang_global_13) - (1)];
        end
        window_scclang_global_12[$unsigned(insert_scclang_global_9)] <= datastrm_data_pipe_scclang_global_7[(PIPE_DEPTH_scclang_global_13) - (1)];
        if (($unsigned(n_scclang_global_10)) < (64)) begin
          n_scclang_global_10 <= ($unsigned(n_scclang_global_10)) + (1);
        end
        cur_avg_scclang_global_0 <= (($unsigned(sum_scclang_global_11)) + ($signed(datastrm_data_pipe_scclang_global_7[(PIPE_DEPTH_scclang_global_13) - (1)]))) / (($unsigned(n_scclang_global_10)) + (1));
        sum_scclang_global_11 <= (($unsigned(sum_scclang_global_11)) + ($signed(datastrm_data_pipe_scclang_global_7[(PIPE_DEPTH_scclang_global_13) - (1)]))) - ($unsigned(window_scclang_global_12[$unsigned(insert_scclang_global_9)]));
        if ((insert_scclang_global_9) >= ((64) - (1))) begin
          insert_scclang_global_9 <= 8'd0;
        end else begin
          insert_scclang_global_9 <= (insert_scclang_global_9) + (1);
        end

      end
      for (i_ms_proc_local_5 = PIPE_DEPTH_scclang_global_13;(i_ms_proc_local_5) < ((PIPE_DEPTH_scclang_global_13) + (PIPE_DEPTH_2_scclang_global_14));i_ms_proc_local_5++) begin
        datastrm_valid_pipe_scclang_global_8[i_ms_proc_local_5] <= datastrm_valid_pipe_scclang_global_8[(i_ms_proc_local_5) - (1)];
      end
      cur_min_pipe_scclang_global_5[0] <= cur_min_scclang_global_4;
      cur_max_pipe_scclang_global_3[0] <= cur_max_scclang_global_2;
      cur_avg_pipe_scclang_global_1[0] <= cur_avg_scclang_global_0;
      for (i_ms_proc_local_6 = 1;(i_ms_proc_local_6) < (PIPE_DEPTH_2_scclang_global_14);i_ms_proc_local_6++) begin
        cur_min_pipe_scclang_global_5[i_ms_proc_local_6] <= cur_min_pipe_scclang_global_5[(i_ms_proc_local_6) - (1)];
        cur_max_pipe_scclang_global_3[i_ms_proc_local_6] <= cur_max_pipe_scclang_global_3[(i_ms_proc_local_6) - (1)];
        cur_avg_pipe_scclang_global_1[i_ms_proc_local_6] <= cur_avg_pipe_scclang_global_1[(i_ms_proc_local_6) - (1)];
      end
    end

  end
endmodule