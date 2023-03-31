module mymodule_sc_module_0 (
);
  logic [7:0] A_scclang_global_0[0:7];
  logic [7:0] B_scclang_global_1[0:7];
  logic [15:0] D_scclang_global_2[0:7];
  logic [0:0] clk_scclang_global_3;
  logic [0:0] enable_row_count_A_scclang_global_4;
  logic [3:0] pixel_cntr_A_scclang_global_5;
  logic [0:0] pixel_cntr_B_scclang_global_6;
  logic [3:0] rd_addr_A_scclang_global_7;
  logic [3:0] rd_addr_B_scclang_global_8;
  logic [0:0] rst_scclang_global_9;
  logic [0:0] slice_cntr_A_scclang_global_10;
  logic [3:0] slice_cntr_B_scclang_global_11;
  logic [7:0] valid_D_scclang_global_12;
  systolic_sc_module_1 u_dut(
    .clk(clk_scclang_global_3),
    .rst(rst_scclang_global_9),
    .enable_row_count_A(enable_row_count_A_scclang_global_4),
    .pixel_cntr_A(pixel_cntr_A_scclang_global_5),
    .slice_cntr_A(slice_cntr_A_scclang_global_10),
    .pixel_cntr_B(pixel_cntr_B_scclang_global_6),
    .slice_cntr_B(slice_cntr_B_scclang_global_11),
    .rd_addr_A(rd_addr_A_scclang_global_7),
    .rd_addr_B(rd_addr_B_scclang_global_8)
  );
  always @(*) begin
  end

endmodule
module systolic_sc_module_1 (
  input logic [0:0] clk,
  input logic [0:0] rst,
  input logic [0:0] enable_row_count_A,
  input logic [7:0] A[0:7],
  input logic [7:0] B[0:7],
  output logic [3:0] pixel_cntr_A,
  output logic [0:0] slice_cntr_A,
  output logic [0:0] pixel_cntr_B,
  output logic [3:0] slice_cntr_B,
  output logic [3:0] rd_addr_A,
  output logic [3:0] rd_addr_B,
  output logic [15:0] D[0:7],
  output logic [7:0] valid_D
);
  logic [7:0] a_wire_scclang_global_0[0:7][0:8];
  logic [7:0] b_wire_scclang_global_1[0:8][0:7];
  logic [15:0] data_wire_scclang_global_2[0:7][0:8];
  logic [0:0] patch_scclang_global_3;
  logic [0:0] shift_scclang_global_4[0:15];
  logic [0:0] valid_wire_scclang_global_5[0:7][0:8];
  control_sc_module_2 control_inst(
    .clk(clk),
    .rst(rst),
    .enable_row_count(enable_row_count_A),
    .pixel_cntr_B(pixel_cntr_B),
    .slice_cntr_B(slice_cntr_B),
    .pixel_cntr_A(pixel_cntr_A),
    .slice_cntr_A(slice_cntr_A),
    .rd_addr_A(rd_addr_A),
    .rd_addr_B(rd_addr_B)
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_0_0(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((0) + (0)) + (1)]),
    .in_a(a_wire_scclang_global_0[0][0]),
    .in_b(b_wire_scclang_global_1[0][0]),
    .out_a(a_wire_scclang_global_0[0][(0) + (1)]),
    .out_b(b_wire_scclang_global_1[(0) + (1)][0]),
    .in_valid(valid_wire_scclang_global_5[0][0]),
    .in_data(data_wire_scclang_global_2[0][0]),
    .out_valid(valid_wire_scclang_global_5[0][(0) + (1)]),
    .out_data(data_wire_scclang_global_2[0][(0) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_0_1(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((1) + (0)) + (1)]),
    .in_a(a_wire_scclang_global_0[1][0]),
    .in_b(b_wire_scclang_global_1[1][0]),
    .out_a(a_wire_scclang_global_0[1][(0) + (1)]),
    .out_b(b_wire_scclang_global_1[(1) + (1)][0]),
    .in_valid(valid_wire_scclang_global_5[1][0]),
    .in_data(data_wire_scclang_global_2[1][0]),
    .out_valid(valid_wire_scclang_global_5[1][(0) + (1)]),
    .out_data(data_wire_scclang_global_2[1][(0) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_0_2(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((2) + (0)) + (1)]),
    .in_a(a_wire_scclang_global_0[2][0]),
    .in_b(b_wire_scclang_global_1[2][0]),
    .out_a(a_wire_scclang_global_0[2][(0) + (1)]),
    .out_b(b_wire_scclang_global_1[(2) + (1)][0]),
    .in_valid(valid_wire_scclang_global_5[2][0]),
    .in_data(data_wire_scclang_global_2[2][0]),
    .out_valid(valid_wire_scclang_global_5[2][(0) + (1)]),
    .out_data(data_wire_scclang_global_2[2][(0) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_0_3(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((3) + (0)) + (1)]),
    .in_a(a_wire_scclang_global_0[3][0]),
    .in_b(b_wire_scclang_global_1[3][0]),
    .out_a(a_wire_scclang_global_0[3][(0) + (1)]),
    .out_b(b_wire_scclang_global_1[(3) + (1)][0]),
    .in_valid(valid_wire_scclang_global_5[3][0]),
    .in_data(data_wire_scclang_global_2[3][0]),
    .out_valid(valid_wire_scclang_global_5[3][(0) + (1)]),
    .out_data(data_wire_scclang_global_2[3][(0) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_0_4(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((4) + (0)) + (1)]),
    .in_a(a_wire_scclang_global_0[4][0]),
    .in_b(b_wire_scclang_global_1[4][0]),
    .out_a(a_wire_scclang_global_0[4][(0) + (1)]),
    .out_b(b_wire_scclang_global_1[(4) + (1)][0]),
    .in_valid(valid_wire_scclang_global_5[4][0]),
    .in_data(data_wire_scclang_global_2[4][0]),
    .out_valid(valid_wire_scclang_global_5[4][(0) + (1)]),
    .out_data(data_wire_scclang_global_2[4][(0) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_0_5(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((5) + (0)) + (1)]),
    .in_a(a_wire_scclang_global_0[5][0]),
    .in_b(b_wire_scclang_global_1[5][0]),
    .out_a(a_wire_scclang_global_0[5][(0) + (1)]),
    .out_b(b_wire_scclang_global_1[(5) + (1)][0]),
    .in_valid(valid_wire_scclang_global_5[5][0]),
    .in_data(data_wire_scclang_global_2[5][0]),
    .out_valid(valid_wire_scclang_global_5[5][(0) + (1)]),
    .out_data(data_wire_scclang_global_2[5][(0) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_0_6(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((6) + (0)) + (1)]),
    .in_a(a_wire_scclang_global_0[6][0]),
    .in_b(b_wire_scclang_global_1[6][0]),
    .out_a(a_wire_scclang_global_0[6][(0) + (1)]),
    .out_b(b_wire_scclang_global_1[(6) + (1)][0]),
    .in_valid(valid_wire_scclang_global_5[6][0]),
    .in_data(data_wire_scclang_global_2[6][0]),
    .out_valid(valid_wire_scclang_global_5[6][(0) + (1)]),
    .out_data(data_wire_scclang_global_2[6][(0) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_0_7(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((7) + (0)) + (1)]),
    .in_a(a_wire_scclang_global_0[7][0]),
    .in_b(b_wire_scclang_global_1[7][0]),
    .out_a(a_wire_scclang_global_0[7][(0) + (1)]),
    .out_b(b_wire_scclang_global_1[(7) + (1)][0]),
    .in_valid(valid_wire_scclang_global_5[7][0]),
    .in_data(data_wire_scclang_global_2[7][0]),
    .out_valid(valid_wire_scclang_global_5[7][(0) + (1)]),
    .out_data(data_wire_scclang_global_2[7][(0) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_1_0(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((0) + (1)) + (1)]),
    .in_a(a_wire_scclang_global_0[0][1]),
    .in_b(b_wire_scclang_global_1[0][1]),
    .out_a(a_wire_scclang_global_0[0][(1) + (1)]),
    .out_b(b_wire_scclang_global_1[(0) + (1)][1]),
    .in_valid(valid_wire_scclang_global_5[0][1]),
    .in_data(data_wire_scclang_global_2[0][1]),
    .out_valid(valid_wire_scclang_global_5[0][(1) + (1)]),
    .out_data(data_wire_scclang_global_2[0][(1) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_1_1(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((1) + (1)) + (1)]),
    .in_a(a_wire_scclang_global_0[1][1]),
    .in_b(b_wire_scclang_global_1[1][1]),
    .out_a(a_wire_scclang_global_0[1][(1) + (1)]),
    .out_b(b_wire_scclang_global_1[(1) + (1)][1]),
    .in_valid(valid_wire_scclang_global_5[1][1]),
    .in_data(data_wire_scclang_global_2[1][1]),
    .out_valid(valid_wire_scclang_global_5[1][(1) + (1)]),
    .out_data(data_wire_scclang_global_2[1][(1) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_1_2(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((2) + (1)) + (1)]),
    .in_a(a_wire_scclang_global_0[2][1]),
    .in_b(b_wire_scclang_global_1[2][1]),
    .out_a(a_wire_scclang_global_0[2][(1) + (1)]),
    .out_b(b_wire_scclang_global_1[(2) + (1)][1]),
    .in_valid(valid_wire_scclang_global_5[2][1]),
    .in_data(data_wire_scclang_global_2[2][1]),
    .out_valid(valid_wire_scclang_global_5[2][(1) + (1)]),
    .out_data(data_wire_scclang_global_2[2][(1) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_1_3(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((3) + (1)) + (1)]),
    .in_a(a_wire_scclang_global_0[3][1]),
    .in_b(b_wire_scclang_global_1[3][1]),
    .out_a(a_wire_scclang_global_0[3][(1) + (1)]),
    .out_b(b_wire_scclang_global_1[(3) + (1)][1]),
    .in_valid(valid_wire_scclang_global_5[3][1]),
    .in_data(data_wire_scclang_global_2[3][1]),
    .out_valid(valid_wire_scclang_global_5[3][(1) + (1)]),
    .out_data(data_wire_scclang_global_2[3][(1) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_1_4(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((4) + (1)) + (1)]),
    .in_a(a_wire_scclang_global_0[4][1]),
    .in_b(b_wire_scclang_global_1[4][1]),
    .out_a(a_wire_scclang_global_0[4][(1) + (1)]),
    .out_b(b_wire_scclang_global_1[(4) + (1)][1]),
    .in_valid(valid_wire_scclang_global_5[4][1]),
    .in_data(data_wire_scclang_global_2[4][1]),
    .out_valid(valid_wire_scclang_global_5[4][(1) + (1)]),
    .out_data(data_wire_scclang_global_2[4][(1) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_1_5(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((5) + (1)) + (1)]),
    .in_a(a_wire_scclang_global_0[5][1]),
    .in_b(b_wire_scclang_global_1[5][1]),
    .out_a(a_wire_scclang_global_0[5][(1) + (1)]),
    .out_b(b_wire_scclang_global_1[(5) + (1)][1]),
    .in_valid(valid_wire_scclang_global_5[5][1]),
    .in_data(data_wire_scclang_global_2[5][1]),
    .out_valid(valid_wire_scclang_global_5[5][(1) + (1)]),
    .out_data(data_wire_scclang_global_2[5][(1) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_1_6(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((6) + (1)) + (1)]),
    .in_a(a_wire_scclang_global_0[6][1]),
    .in_b(b_wire_scclang_global_1[6][1]),
    .out_a(a_wire_scclang_global_0[6][(1) + (1)]),
    .out_b(b_wire_scclang_global_1[(6) + (1)][1]),
    .in_valid(valid_wire_scclang_global_5[6][1]),
    .in_data(data_wire_scclang_global_2[6][1]),
    .out_valid(valid_wire_scclang_global_5[6][(1) + (1)]),
    .out_data(data_wire_scclang_global_2[6][(1) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_1_7(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((7) + (1)) + (1)]),
    .in_a(a_wire_scclang_global_0[7][1]),
    .in_b(b_wire_scclang_global_1[7][1]),
    .out_a(a_wire_scclang_global_0[7][(1) + (1)]),
    .out_b(b_wire_scclang_global_1[(7) + (1)][1]),
    .in_valid(valid_wire_scclang_global_5[7][1]),
    .in_data(data_wire_scclang_global_2[7][1]),
    .out_valid(valid_wire_scclang_global_5[7][(1) + (1)]),
    .out_data(data_wire_scclang_global_2[7][(1) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_2_0(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((0) + (2)) + (1)]),
    .in_a(a_wire_scclang_global_0[0][2]),
    .in_b(b_wire_scclang_global_1[0][2]),
    .out_a(a_wire_scclang_global_0[0][(2) + (1)]),
    .out_b(b_wire_scclang_global_1[(0) + (1)][2]),
    .in_valid(valid_wire_scclang_global_5[0][2]),
    .in_data(data_wire_scclang_global_2[0][2]),
    .out_valid(valid_wire_scclang_global_5[0][(2) + (1)]),
    .out_data(data_wire_scclang_global_2[0][(2) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_2_1(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((1) + (2)) + (1)]),
    .in_a(a_wire_scclang_global_0[1][2]),
    .in_b(b_wire_scclang_global_1[1][2]),
    .out_a(a_wire_scclang_global_0[1][(2) + (1)]),
    .out_b(b_wire_scclang_global_1[(1) + (1)][2]),
    .in_valid(valid_wire_scclang_global_5[1][2]),
    .in_data(data_wire_scclang_global_2[1][2]),
    .out_valid(valid_wire_scclang_global_5[1][(2) + (1)]),
    .out_data(data_wire_scclang_global_2[1][(2) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_2_2(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((2) + (2)) + (1)]),
    .in_a(a_wire_scclang_global_0[2][2]),
    .in_b(b_wire_scclang_global_1[2][2]),
    .out_a(a_wire_scclang_global_0[2][(2) + (1)]),
    .out_b(b_wire_scclang_global_1[(2) + (1)][2]),
    .in_valid(valid_wire_scclang_global_5[2][2]),
    .in_data(data_wire_scclang_global_2[2][2]),
    .out_valid(valid_wire_scclang_global_5[2][(2) + (1)]),
    .out_data(data_wire_scclang_global_2[2][(2) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_2_3(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((3) + (2)) + (1)]),
    .in_a(a_wire_scclang_global_0[3][2]),
    .in_b(b_wire_scclang_global_1[3][2]),
    .out_a(a_wire_scclang_global_0[3][(2) + (1)]),
    .out_b(b_wire_scclang_global_1[(3) + (1)][2]),
    .in_valid(valid_wire_scclang_global_5[3][2]),
    .in_data(data_wire_scclang_global_2[3][2]),
    .out_valid(valid_wire_scclang_global_5[3][(2) + (1)]),
    .out_data(data_wire_scclang_global_2[3][(2) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_2_4(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((4) + (2)) + (1)]),
    .in_a(a_wire_scclang_global_0[4][2]),
    .in_b(b_wire_scclang_global_1[4][2]),
    .out_a(a_wire_scclang_global_0[4][(2) + (1)]),
    .out_b(b_wire_scclang_global_1[(4) + (1)][2]),
    .in_valid(valid_wire_scclang_global_5[4][2]),
    .in_data(data_wire_scclang_global_2[4][2]),
    .out_valid(valid_wire_scclang_global_5[4][(2) + (1)]),
    .out_data(data_wire_scclang_global_2[4][(2) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_2_5(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((5) + (2)) + (1)]),
    .in_a(a_wire_scclang_global_0[5][2]),
    .in_b(b_wire_scclang_global_1[5][2]),
    .out_a(a_wire_scclang_global_0[5][(2) + (1)]),
    .out_b(b_wire_scclang_global_1[(5) + (1)][2]),
    .in_valid(valid_wire_scclang_global_5[5][2]),
    .in_data(data_wire_scclang_global_2[5][2]),
    .out_valid(valid_wire_scclang_global_5[5][(2) + (1)]),
    .out_data(data_wire_scclang_global_2[5][(2) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_2_6(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((6) + (2)) + (1)]),
    .in_a(a_wire_scclang_global_0[6][2]),
    .in_b(b_wire_scclang_global_1[6][2]),
    .out_a(a_wire_scclang_global_0[6][(2) + (1)]),
    .out_b(b_wire_scclang_global_1[(6) + (1)][2]),
    .in_valid(valid_wire_scclang_global_5[6][2]),
    .in_data(data_wire_scclang_global_2[6][2]),
    .out_valid(valid_wire_scclang_global_5[6][(2) + (1)]),
    .out_data(data_wire_scclang_global_2[6][(2) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_2_7(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((7) + (2)) + (1)]),
    .in_a(a_wire_scclang_global_0[7][2]),
    .in_b(b_wire_scclang_global_1[7][2]),
    .out_a(a_wire_scclang_global_0[7][(2) + (1)]),
    .out_b(b_wire_scclang_global_1[(7) + (1)][2]),
    .in_valid(valid_wire_scclang_global_5[7][2]),
    .in_data(data_wire_scclang_global_2[7][2]),
    .out_valid(valid_wire_scclang_global_5[7][(2) + (1)]),
    .out_data(data_wire_scclang_global_2[7][(2) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_3_0(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((0) + (3)) + (1)]),
    .in_a(a_wire_scclang_global_0[0][3]),
    .in_b(b_wire_scclang_global_1[0][3]),
    .out_a(a_wire_scclang_global_0[0][(3) + (1)]),
    .out_b(b_wire_scclang_global_1[(0) + (1)][3]),
    .in_valid(valid_wire_scclang_global_5[0][3]),
    .in_data(data_wire_scclang_global_2[0][3]),
    .out_valid(valid_wire_scclang_global_5[0][(3) + (1)]),
    .out_data(data_wire_scclang_global_2[0][(3) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_3_1(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((1) + (3)) + (1)]),
    .in_a(a_wire_scclang_global_0[1][3]),
    .in_b(b_wire_scclang_global_1[1][3]),
    .out_a(a_wire_scclang_global_0[1][(3) + (1)]),
    .out_b(b_wire_scclang_global_1[(1) + (1)][3]),
    .in_valid(valid_wire_scclang_global_5[1][3]),
    .in_data(data_wire_scclang_global_2[1][3]),
    .out_valid(valid_wire_scclang_global_5[1][(3) + (1)]),
    .out_data(data_wire_scclang_global_2[1][(3) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_3_2(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((2) + (3)) + (1)]),
    .in_a(a_wire_scclang_global_0[2][3]),
    .in_b(b_wire_scclang_global_1[2][3]),
    .out_a(a_wire_scclang_global_0[2][(3) + (1)]),
    .out_b(b_wire_scclang_global_1[(2) + (1)][3]),
    .in_valid(valid_wire_scclang_global_5[2][3]),
    .in_data(data_wire_scclang_global_2[2][3]),
    .out_valid(valid_wire_scclang_global_5[2][(3) + (1)]),
    .out_data(data_wire_scclang_global_2[2][(3) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_3_3(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((3) + (3)) + (1)]),
    .in_a(a_wire_scclang_global_0[3][3]),
    .in_b(b_wire_scclang_global_1[3][3]),
    .out_a(a_wire_scclang_global_0[3][(3) + (1)]),
    .out_b(b_wire_scclang_global_1[(3) + (1)][3]),
    .in_valid(valid_wire_scclang_global_5[3][3]),
    .in_data(data_wire_scclang_global_2[3][3]),
    .out_valid(valid_wire_scclang_global_5[3][(3) + (1)]),
    .out_data(data_wire_scclang_global_2[3][(3) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_3_4(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((4) + (3)) + (1)]),
    .in_a(a_wire_scclang_global_0[4][3]),
    .in_b(b_wire_scclang_global_1[4][3]),
    .out_a(a_wire_scclang_global_0[4][(3) + (1)]),
    .out_b(b_wire_scclang_global_1[(4) + (1)][3]),
    .in_valid(valid_wire_scclang_global_5[4][3]),
    .in_data(data_wire_scclang_global_2[4][3]),
    .out_valid(valid_wire_scclang_global_5[4][(3) + (1)]),
    .out_data(data_wire_scclang_global_2[4][(3) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_3_5(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((5) + (3)) + (1)]),
    .in_a(a_wire_scclang_global_0[5][3]),
    .in_b(b_wire_scclang_global_1[5][3]),
    .out_a(a_wire_scclang_global_0[5][(3) + (1)]),
    .out_b(b_wire_scclang_global_1[(5) + (1)][3]),
    .in_valid(valid_wire_scclang_global_5[5][3]),
    .in_data(data_wire_scclang_global_2[5][3]),
    .out_valid(valid_wire_scclang_global_5[5][(3) + (1)]),
    .out_data(data_wire_scclang_global_2[5][(3) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_3_6(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((6) + (3)) + (1)]),
    .in_a(a_wire_scclang_global_0[6][3]),
    .in_b(b_wire_scclang_global_1[6][3]),
    .out_a(a_wire_scclang_global_0[6][(3) + (1)]),
    .out_b(b_wire_scclang_global_1[(6) + (1)][3]),
    .in_valid(valid_wire_scclang_global_5[6][3]),
    .in_data(data_wire_scclang_global_2[6][3]),
    .out_valid(valid_wire_scclang_global_5[6][(3) + (1)]),
    .out_data(data_wire_scclang_global_2[6][(3) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_3_7(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((7) + (3)) + (1)]),
    .in_a(a_wire_scclang_global_0[7][3]),
    .in_b(b_wire_scclang_global_1[7][3]),
    .out_a(a_wire_scclang_global_0[7][(3) + (1)]),
    .out_b(b_wire_scclang_global_1[(7) + (1)][3]),
    .in_valid(valid_wire_scclang_global_5[7][3]),
    .in_data(data_wire_scclang_global_2[7][3]),
    .out_valid(valid_wire_scclang_global_5[7][(3) + (1)]),
    .out_data(data_wire_scclang_global_2[7][(3) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_4_0(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((0) + (4)) + (1)]),
    .in_a(a_wire_scclang_global_0[0][4]),
    .in_b(b_wire_scclang_global_1[0][4]),
    .out_a(a_wire_scclang_global_0[0][(4) + (1)]),
    .out_b(b_wire_scclang_global_1[(0) + (1)][4]),
    .in_valid(valid_wire_scclang_global_5[0][4]),
    .in_data(data_wire_scclang_global_2[0][4]),
    .out_valid(valid_wire_scclang_global_5[0][(4) + (1)]),
    .out_data(data_wire_scclang_global_2[0][(4) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_4_1(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((1) + (4)) + (1)]),
    .in_a(a_wire_scclang_global_0[1][4]),
    .in_b(b_wire_scclang_global_1[1][4]),
    .out_a(a_wire_scclang_global_0[1][(4) + (1)]),
    .out_b(b_wire_scclang_global_1[(1) + (1)][4]),
    .in_valid(valid_wire_scclang_global_5[1][4]),
    .in_data(data_wire_scclang_global_2[1][4]),
    .out_valid(valid_wire_scclang_global_5[1][(4) + (1)]),
    .out_data(data_wire_scclang_global_2[1][(4) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_4_2(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((2) + (4)) + (1)]),
    .in_a(a_wire_scclang_global_0[2][4]),
    .in_b(b_wire_scclang_global_1[2][4]),
    .out_a(a_wire_scclang_global_0[2][(4) + (1)]),
    .out_b(b_wire_scclang_global_1[(2) + (1)][4]),
    .in_valid(valid_wire_scclang_global_5[2][4]),
    .in_data(data_wire_scclang_global_2[2][4]),
    .out_valid(valid_wire_scclang_global_5[2][(4) + (1)]),
    .out_data(data_wire_scclang_global_2[2][(4) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_4_3(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((3) + (4)) + (1)]),
    .in_a(a_wire_scclang_global_0[3][4]),
    .in_b(b_wire_scclang_global_1[3][4]),
    .out_a(a_wire_scclang_global_0[3][(4) + (1)]),
    .out_b(b_wire_scclang_global_1[(3) + (1)][4]),
    .in_valid(valid_wire_scclang_global_5[3][4]),
    .in_data(data_wire_scclang_global_2[3][4]),
    .out_valid(valid_wire_scclang_global_5[3][(4) + (1)]),
    .out_data(data_wire_scclang_global_2[3][(4) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_4_4(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((4) + (4)) + (1)]),
    .in_a(a_wire_scclang_global_0[4][4]),
    .in_b(b_wire_scclang_global_1[4][4]),
    .out_a(a_wire_scclang_global_0[4][(4) + (1)]),
    .out_b(b_wire_scclang_global_1[(4) + (1)][4]),
    .in_valid(valid_wire_scclang_global_5[4][4]),
    .in_data(data_wire_scclang_global_2[4][4]),
    .out_valid(valid_wire_scclang_global_5[4][(4) + (1)]),
    .out_data(data_wire_scclang_global_2[4][(4) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_4_5(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((5) + (4)) + (1)]),
    .in_a(a_wire_scclang_global_0[5][4]),
    .in_b(b_wire_scclang_global_1[5][4]),
    .out_a(a_wire_scclang_global_0[5][(4) + (1)]),
    .out_b(b_wire_scclang_global_1[(5) + (1)][4]),
    .in_valid(valid_wire_scclang_global_5[5][4]),
    .in_data(data_wire_scclang_global_2[5][4]),
    .out_valid(valid_wire_scclang_global_5[5][(4) + (1)]),
    .out_data(data_wire_scclang_global_2[5][(4) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_4_6(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((6) + (4)) + (1)]),
    .in_a(a_wire_scclang_global_0[6][4]),
    .in_b(b_wire_scclang_global_1[6][4]),
    .out_a(a_wire_scclang_global_0[6][(4) + (1)]),
    .out_b(b_wire_scclang_global_1[(6) + (1)][4]),
    .in_valid(valid_wire_scclang_global_5[6][4]),
    .in_data(data_wire_scclang_global_2[6][4]),
    .out_valid(valid_wire_scclang_global_5[6][(4) + (1)]),
    .out_data(data_wire_scclang_global_2[6][(4) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_4_7(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((7) + (4)) + (1)]),
    .in_a(a_wire_scclang_global_0[7][4]),
    .in_b(b_wire_scclang_global_1[7][4]),
    .out_a(a_wire_scclang_global_0[7][(4) + (1)]),
    .out_b(b_wire_scclang_global_1[(7) + (1)][4]),
    .in_valid(valid_wire_scclang_global_5[7][4]),
    .in_data(data_wire_scclang_global_2[7][4]),
    .out_valid(valid_wire_scclang_global_5[7][(4) + (1)]),
    .out_data(data_wire_scclang_global_2[7][(4) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_5_0(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((0) + (5)) + (1)]),
    .in_a(a_wire_scclang_global_0[0][5]),
    .in_b(b_wire_scclang_global_1[0][5]),
    .out_a(a_wire_scclang_global_0[0][(5) + (1)]),
    .out_b(b_wire_scclang_global_1[(0) + (1)][5]),
    .in_valid(valid_wire_scclang_global_5[0][5]),
    .in_data(data_wire_scclang_global_2[0][5]),
    .out_valid(valid_wire_scclang_global_5[0][(5) + (1)]),
    .out_data(data_wire_scclang_global_2[0][(5) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_5_1(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((1) + (5)) + (1)]),
    .in_a(a_wire_scclang_global_0[1][5]),
    .in_b(b_wire_scclang_global_1[1][5]),
    .out_a(a_wire_scclang_global_0[1][(5) + (1)]),
    .out_b(b_wire_scclang_global_1[(1) + (1)][5]),
    .in_valid(valid_wire_scclang_global_5[1][5]),
    .in_data(data_wire_scclang_global_2[1][5]),
    .out_valid(valid_wire_scclang_global_5[1][(5) + (1)]),
    .out_data(data_wire_scclang_global_2[1][(5) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_5_2(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((2) + (5)) + (1)]),
    .in_a(a_wire_scclang_global_0[2][5]),
    .in_b(b_wire_scclang_global_1[2][5]),
    .out_a(a_wire_scclang_global_0[2][(5) + (1)]),
    .out_b(b_wire_scclang_global_1[(2) + (1)][5]),
    .in_valid(valid_wire_scclang_global_5[2][5]),
    .in_data(data_wire_scclang_global_2[2][5]),
    .out_valid(valid_wire_scclang_global_5[2][(5) + (1)]),
    .out_data(data_wire_scclang_global_2[2][(5) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_5_3(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((3) + (5)) + (1)]),
    .in_a(a_wire_scclang_global_0[3][5]),
    .in_b(b_wire_scclang_global_1[3][5]),
    .out_a(a_wire_scclang_global_0[3][(5) + (1)]),
    .out_b(b_wire_scclang_global_1[(3) + (1)][5]),
    .in_valid(valid_wire_scclang_global_5[3][5]),
    .in_data(data_wire_scclang_global_2[3][5]),
    .out_valid(valid_wire_scclang_global_5[3][(5) + (1)]),
    .out_data(data_wire_scclang_global_2[3][(5) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_5_4(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((4) + (5)) + (1)]),
    .in_a(a_wire_scclang_global_0[4][5]),
    .in_b(b_wire_scclang_global_1[4][5]),
    .out_a(a_wire_scclang_global_0[4][(5) + (1)]),
    .out_b(b_wire_scclang_global_1[(4) + (1)][5]),
    .in_valid(valid_wire_scclang_global_5[4][5]),
    .in_data(data_wire_scclang_global_2[4][5]),
    .out_valid(valid_wire_scclang_global_5[4][(5) + (1)]),
    .out_data(data_wire_scclang_global_2[4][(5) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_5_5(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((5) + (5)) + (1)]),
    .in_a(a_wire_scclang_global_0[5][5]),
    .in_b(b_wire_scclang_global_1[5][5]),
    .out_a(a_wire_scclang_global_0[5][(5) + (1)]),
    .out_b(b_wire_scclang_global_1[(5) + (1)][5]),
    .in_valid(valid_wire_scclang_global_5[5][5]),
    .in_data(data_wire_scclang_global_2[5][5]),
    .out_valid(valid_wire_scclang_global_5[5][(5) + (1)]),
    .out_data(data_wire_scclang_global_2[5][(5) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_5_6(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((6) + (5)) + (1)]),
    .in_a(a_wire_scclang_global_0[6][5]),
    .in_b(b_wire_scclang_global_1[6][5]),
    .out_a(a_wire_scclang_global_0[6][(5) + (1)]),
    .out_b(b_wire_scclang_global_1[(6) + (1)][5]),
    .in_valid(valid_wire_scclang_global_5[6][5]),
    .in_data(data_wire_scclang_global_2[6][5]),
    .out_valid(valid_wire_scclang_global_5[6][(5) + (1)]),
    .out_data(data_wire_scclang_global_2[6][(5) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_5_7(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((7) + (5)) + (1)]),
    .in_a(a_wire_scclang_global_0[7][5]),
    .in_b(b_wire_scclang_global_1[7][5]),
    .out_a(a_wire_scclang_global_0[7][(5) + (1)]),
    .out_b(b_wire_scclang_global_1[(7) + (1)][5]),
    .in_valid(valid_wire_scclang_global_5[7][5]),
    .in_data(data_wire_scclang_global_2[7][5]),
    .out_valid(valid_wire_scclang_global_5[7][(5) + (1)]),
    .out_data(data_wire_scclang_global_2[7][(5) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_6_0(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((0) + (6)) + (1)]),
    .in_a(a_wire_scclang_global_0[0][6]),
    .in_b(b_wire_scclang_global_1[0][6]),
    .out_a(a_wire_scclang_global_0[0][(6) + (1)]),
    .out_b(b_wire_scclang_global_1[(0) + (1)][6]),
    .in_valid(valid_wire_scclang_global_5[0][6]),
    .in_data(data_wire_scclang_global_2[0][6]),
    .out_valid(valid_wire_scclang_global_5[0][(6) + (1)]),
    .out_data(data_wire_scclang_global_2[0][(6) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_6_1(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((1) + (6)) + (1)]),
    .in_a(a_wire_scclang_global_0[1][6]),
    .in_b(b_wire_scclang_global_1[1][6]),
    .out_a(a_wire_scclang_global_0[1][(6) + (1)]),
    .out_b(b_wire_scclang_global_1[(1) + (1)][6]),
    .in_valid(valid_wire_scclang_global_5[1][6]),
    .in_data(data_wire_scclang_global_2[1][6]),
    .out_valid(valid_wire_scclang_global_5[1][(6) + (1)]),
    .out_data(data_wire_scclang_global_2[1][(6) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_6_2(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((2) + (6)) + (1)]),
    .in_a(a_wire_scclang_global_0[2][6]),
    .in_b(b_wire_scclang_global_1[2][6]),
    .out_a(a_wire_scclang_global_0[2][(6) + (1)]),
    .out_b(b_wire_scclang_global_1[(2) + (1)][6]),
    .in_valid(valid_wire_scclang_global_5[2][6]),
    .in_data(data_wire_scclang_global_2[2][6]),
    .out_valid(valid_wire_scclang_global_5[2][(6) + (1)]),
    .out_data(data_wire_scclang_global_2[2][(6) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_6_3(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((3) + (6)) + (1)]),
    .in_a(a_wire_scclang_global_0[3][6]),
    .in_b(b_wire_scclang_global_1[3][6]),
    .out_a(a_wire_scclang_global_0[3][(6) + (1)]),
    .out_b(b_wire_scclang_global_1[(3) + (1)][6]),
    .in_valid(valid_wire_scclang_global_5[3][6]),
    .in_data(data_wire_scclang_global_2[3][6]),
    .out_valid(valid_wire_scclang_global_5[3][(6) + (1)]),
    .out_data(data_wire_scclang_global_2[3][(6) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_6_4(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((4) + (6)) + (1)]),
    .in_a(a_wire_scclang_global_0[4][6]),
    .in_b(b_wire_scclang_global_1[4][6]),
    .out_a(a_wire_scclang_global_0[4][(6) + (1)]),
    .out_b(b_wire_scclang_global_1[(4) + (1)][6]),
    .in_valid(valid_wire_scclang_global_5[4][6]),
    .in_data(data_wire_scclang_global_2[4][6]),
    .out_valid(valid_wire_scclang_global_5[4][(6) + (1)]),
    .out_data(data_wire_scclang_global_2[4][(6) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_6_5(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((5) + (6)) + (1)]),
    .in_a(a_wire_scclang_global_0[5][6]),
    .in_b(b_wire_scclang_global_1[5][6]),
    .out_a(a_wire_scclang_global_0[5][(6) + (1)]),
    .out_b(b_wire_scclang_global_1[(5) + (1)][6]),
    .in_valid(valid_wire_scclang_global_5[5][6]),
    .in_data(data_wire_scclang_global_2[5][6]),
    .out_valid(valid_wire_scclang_global_5[5][(6) + (1)]),
    .out_data(data_wire_scclang_global_2[5][(6) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_6_6(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((6) + (6)) + (1)]),
    .in_a(a_wire_scclang_global_0[6][6]),
    .in_b(b_wire_scclang_global_1[6][6]),
    .out_a(a_wire_scclang_global_0[6][(6) + (1)]),
    .out_b(b_wire_scclang_global_1[(6) + (1)][6]),
    .in_valid(valid_wire_scclang_global_5[6][6]),
    .in_data(data_wire_scclang_global_2[6][6]),
    .out_valid(valid_wire_scclang_global_5[6][(6) + (1)]),
    .out_data(data_wire_scclang_global_2[6][(6) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_6_7(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((7) + (6)) + (1)]),
    .in_a(a_wire_scclang_global_0[7][6]),
    .in_b(b_wire_scclang_global_1[7][6]),
    .out_a(a_wire_scclang_global_0[7][(6) + (1)]),
    .out_b(b_wire_scclang_global_1[(7) + (1)][6]),
    .in_valid(valid_wire_scclang_global_5[7][6]),
    .in_data(data_wire_scclang_global_2[7][6]),
    .out_valid(valid_wire_scclang_global_5[7][(6) + (1)]),
    .out_data(data_wire_scclang_global_2[7][(6) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_7_0(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((0) + (7)) + (1)]),
    .in_a(a_wire_scclang_global_0[0][7]),
    .in_b(b_wire_scclang_global_1[0][7]),
    .out_a(a_wire_scclang_global_0[0][(7) + (1)]),
    .out_b(b_wire_scclang_global_1[(0) + (1)][7]),
    .in_valid(valid_wire_scclang_global_5[0][7]),
    .in_data(data_wire_scclang_global_2[0][7]),
    .out_valid(valid_wire_scclang_global_5[0][(7) + (1)]),
    .out_data(data_wire_scclang_global_2[0][(7) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_7_1(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((1) + (7)) + (1)]),
    .in_a(a_wire_scclang_global_0[1][7]),
    .in_b(b_wire_scclang_global_1[1][7]),
    .out_a(a_wire_scclang_global_0[1][(7) + (1)]),
    .out_b(b_wire_scclang_global_1[(1) + (1)][7]),
    .in_valid(valid_wire_scclang_global_5[1][7]),
    .in_data(data_wire_scclang_global_2[1][7]),
    .out_valid(valid_wire_scclang_global_5[1][(7) + (1)]),
    .out_data(data_wire_scclang_global_2[1][(7) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_7_2(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((2) + (7)) + (1)]),
    .in_a(a_wire_scclang_global_0[2][7]),
    .in_b(b_wire_scclang_global_1[2][7]),
    .out_a(a_wire_scclang_global_0[2][(7) + (1)]),
    .out_b(b_wire_scclang_global_1[(2) + (1)][7]),
    .in_valid(valid_wire_scclang_global_5[2][7]),
    .in_data(data_wire_scclang_global_2[2][7]),
    .out_valid(valid_wire_scclang_global_5[2][(7) + (1)]),
    .out_data(data_wire_scclang_global_2[2][(7) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_7_3(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((3) + (7)) + (1)]),
    .in_a(a_wire_scclang_global_0[3][7]),
    .in_b(b_wire_scclang_global_1[3][7]),
    .out_a(a_wire_scclang_global_0[3][(7) + (1)]),
    .out_b(b_wire_scclang_global_1[(3) + (1)][7]),
    .in_valid(valid_wire_scclang_global_5[3][7]),
    .in_data(data_wire_scclang_global_2[3][7]),
    .out_valid(valid_wire_scclang_global_5[3][(7) + (1)]),
    .out_data(data_wire_scclang_global_2[3][(7) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_7_4(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((4) + (7)) + (1)]),
    .in_a(a_wire_scclang_global_0[4][7]),
    .in_b(b_wire_scclang_global_1[4][7]),
    .out_a(a_wire_scclang_global_0[4][(7) + (1)]),
    .out_b(b_wire_scclang_global_1[(4) + (1)][7]),
    .in_valid(valid_wire_scclang_global_5[4][7]),
    .in_data(data_wire_scclang_global_2[4][7]),
    .out_valid(valid_wire_scclang_global_5[4][(7) + (1)]),
    .out_data(data_wire_scclang_global_2[4][(7) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_7_5(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((5) + (7)) + (1)]),
    .in_a(a_wire_scclang_global_0[5][7]),
    .in_b(b_wire_scclang_global_1[5][7]),
    .out_a(a_wire_scclang_global_0[5][(7) + (1)]),
    .out_b(b_wire_scclang_global_1[(5) + (1)][7]),
    .in_valid(valid_wire_scclang_global_5[5][7]),
    .in_data(data_wire_scclang_global_2[5][7]),
    .out_valid(valid_wire_scclang_global_5[5][(7) + (1)]),
    .out_data(data_wire_scclang_global_2[5][(7) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_7_6(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((6) + (7)) + (1)]),
    .in_a(a_wire_scclang_global_0[6][7]),
    .in_b(b_wire_scclang_global_1[6][7]),
    .out_a(a_wire_scclang_global_0[6][(7) + (1)]),
    .out_b(b_wire_scclang_global_1[(6) + (1)][7]),
    .in_valid(valid_wire_scclang_global_5[6][7]),
    .in_data(data_wire_scclang_global_2[6][7]),
    .out_valid(valid_wire_scclang_global_5[6][(7) + (1)]),
    .out_data(data_wire_scclang_global_2[6][(7) + (1)])
  );
  always @(*) begin
  end

  pe_sc_module_3 pe_inst_7_7(
    .clk(clk),
    .rst(rst),
    .init(shift_scclang_global_4[((7) + (7)) + (1)]),
    .in_a(a_wire_scclang_global_0[7][7]),
    .in_b(b_wire_scclang_global_1[7][7]),
    .out_a(a_wire_scclang_global_0[7][(7) + (1)]),
    .out_b(b_wire_scclang_global_1[(7) + (1)][7]),
    .in_valid(valid_wire_scclang_global_5[7][7]),
    .in_data(data_wire_scclang_global_2[7][7]),
    .out_valid(valid_wire_scclang_global_5[7][(7) + (1)]),
    .out_data(data_wire_scclang_global_2[7][(7) + (1)])
  );
  always @(*) begin
  end

  always_ff @(posedge clk) begin: ms_proc
    logic signed[31:0] i_ms_proc_local_8;
    logic signed[31:0] i_ms_proc_local_9;
    
    if (rst) begin
      for (i_ms_proc_local_8 = 0;(i_ms_proc_local_8) < ((2) * (8));i_ms_proc_local_8++) begin
        shift_scclang_global_4[i_ms_proc_local_8] <= 0;
      end
      patch_scclang_global_3 <= 1'd0;
    end else begin
      if (((pixel_cntr_A) == ((8) - (1))) && ((patch_scclang_global_3) < (((8) * (8)) / ((8) * (8))))) begin
        patch_scclang_global_3 <= (patch_scclang_global_3) + (1);
        shift_scclang_global_4[0] <= 1;
      end else begin
        shift_scclang_global_4[0] <= 0;
      end

      for (i_ms_proc_local_9 = 1;(i_ms_proc_local_9) < ((2) * (8));i_ms_proc_local_9++) begin
        shift_scclang_global_4[i_ms_proc_local_9] <= shift_scclang_global_4[(i_ms_proc_local_9) - (1)];
      end
    end

  end
  always @(valid_wire_scclang_global_5[0][(0)] or data_wire_scclang_global_2[0][(0)] or valid_wire_scclang_global_5[1][(0)] or data_wire_scclang_global_2[1][(0)] or valid_wire_scclang_global_5[2][(0)] or data_wire_scclang_global_2[2][(0)] or valid_wire_scclang_global_5[3][(0)] or data_wire_scclang_global_2[3][(0)] or valid_wire_scclang_global_5[4][(0)] or data_wire_scclang_global_2[4][(0)] or valid_wire_scclang_global_5[5][(0)] or data_wire_scclang_global_2[5][(0)] or valid_wire_scclang_global_5[6][(0)] or data_wire_scclang_global_2[6][(0)] or valid_wire_scclang_global_5[7][(0)] or data_wire_scclang_global_2[7][(0)] or A[(0)] or A[(1)] or A[(2)] or A[(3)] or A[(4)] or A[(5)] or A[(6)] or A[(7)] or B[(0)] or B[(1)] or B[(2)] or B[(3)] or B[(4)] or B[(5)] or B[(6)] or B[(7)]) begin: sa_mc_proc
    logic [7:0] valid_D_d_sa_mc_proc_local_10;
    logic signed[31:0] i_sa_mc_proc_local_11;
    logic signed[31:0] j_sa_mc_proc_local_12;
    
    for (i_sa_mc_proc_local_11 = 0;(i_sa_mc_proc_local_11) < (8);i_sa_mc_proc_local_11++) begin
      valid_D_d_sa_mc_proc_local_10[i_sa_mc_proc_local_11] = valid_wire_scclang_global_5[i_sa_mc_proc_local_11][8];
      D[i_sa_mc_proc_local_11] = data_wire_scclang_global_2[i_sa_mc_proc_local_11][8];
      a_wire_scclang_global_0[i_sa_mc_proc_local_11][0] <= A[i_sa_mc_proc_local_11];
      valid_wire_scclang_global_5[i_sa_mc_proc_local_11][0] <= 0;
      data_wire_scclang_global_2[i_sa_mc_proc_local_11][0] <= 16'd0;
    end
    for (j_sa_mc_proc_local_12 = 0;(j_sa_mc_proc_local_12) < (8);j_sa_mc_proc_local_12++) begin
      b_wire_scclang_global_1[0][j_sa_mc_proc_local_12] <= B[j_sa_mc_proc_local_12];
    end
    valid_D <= valid_D_d_sa_mc_proc_local_10;
  end
endmodule
module control_sc_module_2 (
  input logic [0:0] clk,
  input logic [0:0] rst,
  input logic [0:0] enable_row_count,
  output logic [3:0] pixel_cntr_A,
  output logic [0:0] slice_cntr_A,
  output logic [0:0] pixel_cntr_B,
  output logic [3:0] slice_cntr_B,
  output logic [3:0] rd_addr_A,
  output logic [3:0] rd_addr_B
);
  logic [0:0] const_true_scclang_global_0;
  counter_sc_module_4 counter_B(
    .clk(clk),
    .reset(rst),
    .enable_row_count(const_true_scclang_global_0),
    .pixel_cntr(slice_cntr_B),
    .slice_cntr(pixel_cntr_B)
  );
  always @(*) begin
  end

  counter_sc_module_5 counter_A(
    .clk(clk),
    .reset(rst),
    .enable_row_count(enable_row_count),
    .pixel_cntr(pixel_cntr_A),
    .slice_cntr(slice_cntr_A)
  );
  always @(*) begin
  end

  always @(pixel_cntr_A or pixel_cntr_B or slice_cntr_A or slice_cntr_B) begin: mc_proc
    
    
    rd_addr_A <= ((slice_cntr_A) * (8)) + (pixel_cntr_A);
    rd_addr_B <= ((pixel_cntr_B) * (8)) + (slice_cntr_B);
    const_true_scclang_global_0 <= 1;
  end
endmodule
module counter_sc_module_4 (
  input logic [0:0] clk,
  input logic [0:0] reset,
  input logic [0:0] enable_row_count,
  output logic [3:0] pixel_cntr,
  output logic [0:0] slice_cntr
);
  always_ff @(posedge clk) begin: ms_proc_1
    
    
    if (reset) begin
      pixel_cntr <= 4'd0;
    end else begin
      if ((pixel_cntr) == ((8) - (1))) begin
        pixel_cntr <= 4'd0;
      end else begin
        pixel_cntr <= (pixel_cntr) + (1);
      end

    end

  end
  always_ff @(posedge clk) begin: ms_proc_2
    
    
    if (reset) begin
      slice_cntr <= 1'd0;
    end else begin
      if ((enable_row_count) && ((pixel_cntr) == ((8) - (1)))) begin
        if ((slice_cntr) == ((1) - (1))) begin
          slice_cntr <= 1'd0;
        end else begin
          slice_cntr <= (slice_cntr) + (1);
        end

      end
    end

  end
endmodule
module counter_sc_module_5 (
  input logic [0:0] clk,
  input logic [0:0] reset,
  input logic [0:0] enable_row_count,
  output logic [3:0] pixel_cntr,
  output logic [0:0] slice_cntr
);
  always_ff @(posedge clk) begin: ms_proc_1
    
    
    if (reset) begin
      pixel_cntr <= 4'd0;
    end else begin
      if ((pixel_cntr) == ((8) - (1))) begin
        pixel_cntr <= 4'd0;
      end else begin
        pixel_cntr <= (pixel_cntr) + (1);
      end

    end

  end
  always_ff @(posedge clk) begin: ms_proc_2
    
    
    if (reset) begin
      slice_cntr <= 1'd0;
    end else begin
      if ((enable_row_count) && ((pixel_cntr) == ((8) - (1)))) begin
        if ((slice_cntr) == ((1) - (1))) begin
          slice_cntr <= 1'd0;
        end else begin
          slice_cntr <= (slice_cntr) + (1);
        end

      end
    end

  end
endmodule
module pe_sc_module_3 (
  input logic [0:0] clk,
  input logic [0:0] rst,
  input logic [0:0] init,
  input logic [7:0] in_a,
  input logic [7:0] in_b,
  input logic [15:0] in_data,
  input logic [0:0] in_valid,
  output logic [7:0] out_a,
  output logic [7:0] out_b,
  output logic [15:0] out_data,
  output logic [0:0] out_valid
);
  logic [15:0] in_data_r_scclang_global_0;
  logic [0:0] in_valid_r_scclang_global_1;
  logic [15:0] mult_op_scclang_global_2;
  logic [15:0] out_sum_scclang_global_3;
  logic [15:0] out_sum_d_scclang_global_4;
  always @(in_a or in_b or out_sum_scclang_global_3 or mult_op_scclang_global_2) begin: mc_proc
    
    
    mult_op_scclang_global_2 <= (in_a) * (in_b);
    out_sum_d_scclang_global_4 <= (out_sum_scclang_global_3) + (mult_op_scclang_global_2);
  end
  always_ff @(posedge clk) begin: ms_proc
    
    
    if (rst) begin
      out_a <= 8'd0;
      out_b <= 8'd0;
      in_valid_r_scclang_global_1 <= 0;
      in_data_r_scclang_global_0 <= 16'd0;
      out_valid <= 0;
      out_data <= 16'd0;
      out_sum_scclang_global_3 <= 16'd0;
    end else begin
      out_a <= in_a;
      out_b <= in_b;
      in_valid_r_scclang_global_1 <= in_valid;
      in_data_r_scclang_global_0 <= in_data;
      if (init) begin
        out_sum_scclang_global_3 <= mult_op_scclang_global_2;
        out_valid <= 1;
        out_data <= out_sum_scclang_global_3;
      end else begin
        out_sum_scclang_global_3 <= out_sum_d_scclang_global_4;
        out_valid <= in_valid_r_scclang_global_1;
        out_data <= in_data_r_scclang_global_0;
      end

    end

  end
endmodule