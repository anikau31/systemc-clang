module avg_0(
  input clk,
  input wire[7:0] i_data,
  output reg[7:0] o_data
);


  reg[7:0] sum2;
  reg[7:0] sum3;
  reg[7:0] tap0;
  reg[7:0] tap1;
  reg[7:0] tap2;
  reg[7:0] tap3;

  always @(*) begin : first_function
    tap0 <= i_data;
  end
  always @(posedge clk) begin : second_function
    tap1 <= tap0;
    tap2 <= tap1;
    tap3 <= tap2;
  end

  always @(*) begin: third_function
    // coefs?
    integer coef1;
    integer coef2;
    integer coef3;

    integer prod1;
    integer prod2;
    integer prod3;

    integer result2, result3;
    reg[7:0] tmpTap1, tmpTap2, tmpTap3;

    coef1 = 2;
    coef2 = 2;
    coef3 = 2;

    tmpTap1 <= tap1;
    tmpTap2 <= tap2;
    tmpTap3 <= tap3;

    prod1 = tmpTap1 * coef1;
    prod2 = tmpTap2 * coef2;
    prod3 = tmpTap3 * coef3;

    result2 = prod1 + prod2;
    result3 = prod1 + prod2 + prod3;

    sum2 <= result2;
    sum3 <= result3;

    o_data = sum3;

  end


endmodule
