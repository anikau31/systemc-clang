module mymodule_sc_module_0 (
);
  logic [0:0] clk_scclang_global_0;
  logic [15:0] maxbits_scclang_global_1;
  logic [15:0] maxprec_scclang_global_2;
  logic [15:0] minbits_scclang_global_3;
  logic signed[15:0] minexp_scclang_global_4;
  logic [0:0] reset_scclang_global_5;
  logic [22:0] c_dut_fp_scclang_global_6_data_frac;
  logic [7:0] c_dut_fp_scclang_global_6_data_expo;
  logic [0:0] c_dut_fp_scclang_global_6_data_sign;
  logic [0:0] c_dut_fp_scclang_global_6_valid;
  logic [0:0] c_dut_fp_scclang_global_6_ready;
  logic [31:0] c_driver_enc_scclang_global_7_data_tdata;
  logic [0:0] c_driver_enc_scclang_global_7_data_tlast;
  logic [0:0] c_driver_enc_scclang_global_7_valid;
  logic [0:0] c_driver_enc_scclang_global_7_ready;
  initial begin
    minbits_scclang_global_3 = 128;
    maxbits_scclang_global_1 = 128;
    maxprec_scclang_global_2 = 32;
    minexp_scclang_global_4 = -(1074);
  end
endmodule