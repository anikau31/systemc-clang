module tb;
  import uvm_pkg::*;
  top dut();
  test_case tc();
  bit clk;
  always clk = #1 ~clk;
  initial begin
    clk = 0;
    $display("UVM imported");
  end
endmodule
