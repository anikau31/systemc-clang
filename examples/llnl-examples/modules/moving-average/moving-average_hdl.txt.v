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
  logic [63:0] cur_max_scclang_global_1;
  logic [63:0] cur_min_scclang_global_2;
  logic [0:0] datardy_scclang_global_3;
  logic [7:0] insert_scclang_global_4;
  logic [7:0] n_scclang_global_5;
  logic [63:0] sum_scclang_global_6;
  logic [63:0] window_scclang_global_7[0:63];
  always_ff @(posedge clk) begin: mc_proc
    
    
    min_out_valid <= datardy_scclang_global_3;
    min_out_data <= cur_min_scclang_global_2;
    max_out_valid <= datardy_scclang_global_3;
    max_out_data <= cur_max_scclang_global_1;
    avg_out_valid <= datardy_scclang_global_3;
    avg_out_data <= cur_avg_scclang_global_0;
    datastrm_ready <= 1;
  end
  always_ff @(posedge clk) begin: ms_proc
    logic signed[31:0] i_ms_proc_local_2;
    
    if ((reset) == (0)) begin
      n_scclang_global_5 <= 8'd0;
      insert_scclang_global_4 <= 8'd0;
      cur_avg_scclang_global_0 <= 64'd0;
      cur_max_scclang_global_1 <= cur_avg_scclang_global_0;
      cur_min_scclang_global_2 <= cur_max_scclang_global_1;
      sum_scclang_global_6 <= 64'd0;
      for (i_ms_proc_local_2 = 0;(i_ms_proc_local_2) < (64);i_ms_proc_local_2++) begin
        window_scclang_global_7[i_ms_proc_local_2] <= 64'd0;
      end
      datardy_scclang_global_3 <= 1;
    end else begin
      if (datastrm_valid) begin
        if ((cur_min_scclang_global_2) > (datastrm_data)) begin
          cur_min_scclang_global_2 <= datastrm_data;
        end
        if ((cur_max_scclang_global_1) < (datastrm_data)) begin
          cur_max_scclang_global_1 <= datastrm_data;
        end
        window_scclang_global_7[$unsigned(insert_scclang_global_4)] <= datastrm_data;
        if (($unsigned(n_scclang_global_5)) < (64)) begin
          n_scclang_global_5 <= ($unsigned(n_scclang_global_5)) + (1);
        end
        cur_avg_scclang_global_0 <= (($unsigned(sum_scclang_global_6)) + ($signed(datastrm_data))) / (($unsigned(n_scclang_global_5)) + (1));
        sum_scclang_global_6 <= (($unsigned(sum_scclang_global_6)) + ($unsigned(datastrm_data))) - ($unsigned(window_scclang_global_7[$unsigned(insert_scclang_global_4)]));
        if ((insert_scclang_global_4) >= ((64) - (1))) begin
          insert_scclang_global_4 <= 8'd0;
        end else begin
          insert_scclang_global_4 <= (insert_scclang_global_4) + (1);
        end

        datardy_scclang_global_3 <= 1;
      end
    end

  end
endmodule