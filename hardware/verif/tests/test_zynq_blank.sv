`define zynq $root.tb.dut.sw.shell_i.zynq_ultra_ps_e_0.inst
`define clk25mhz $root.tb.dut.sw.clk25mhz_0
`define clk50mhz $root.tb.dut.sw.clk50mhz_0
`define clk100mhz $root.tb.dut.sw.clk100mhz_0
`define clkps   $root.tb.dut.sw.shell_i.zynq_ultra_ps_e_0.pl_clk0

`define DMA_BASE 32'hA000_0000
`define GPIO     32'hA001_0000

`define MM2S_DMACR     (`DMA_BASE + 32'h00)
`define MM2S_DMASR     (`DMA_BASE + 32'h04)
`define MM2S_SA_LO     (`DMA_BASE + 32'h18)
`define MM2S_SA_HI     (`DMA_BASE + 32'h1C)
`define MM2S_LENGTH    (`DMA_BASE + 32'h28)

`define S2MM_DMACR     (`DMA_BASE + 32'h30)
`define S2MM_DMASR     (`DMA_BASE + 32'h34)
`define S2MM_DA_LO     (`DMA_BASE + 32'h48)
`define S2MM_DA_HI     (`DMA_BASE + 32'h4C)
`define S2MM_LENGTH    (`DMA_BASE + 32'h58)

program automatic test_case(); // interface to the design
  bit[1:0] rsp;
  bit[127:0] data;
  integer i;
  initial begin


    $display("Test case");
    `zynq.por_srstb_reset(1'b1); // must be called
    repeat (17) begin
      @(posedge `clkps);
    end
    `zynq.por_srstb_reset(1'b0);
    `zynq.fpga_soft_reset(4'hF);
    repeat (17 * 4) begin
      @(posedge `clkps);
    end

    // minimum 16 clock pulse width delay
    `zynq.por_srstb_reset(1'b1);
    `zynq.fpga_soft_reset(4'h0);

    // `zynq.pre_load_mem(2'b00, 32'h0001_0000, 1024);
    `zynq.pre_load_mem_from_file("../../../data/test_zynq_blank.mem", 32'h0001_0000, 512);

    // Custom logic reset
    `zynq.write_data(`GPIO, 8'h4, 32'b1, rsp);
    
    // Reset DMA
    `zynq.write_data(`MM2S_DMACR,  8'h4, 32'b100, rsp);
    // MM2S
    // transfer 64 bytes
    `zynq.write_data(`MM2S_DMACR,  8'h4, 32'b1000000000001, rsp);
    `zynq.write_data(`MM2S_SA_LO,  8'h4, 32'h0001_0000, rsp);
    `zynq.write_data(`MM2S_SA_HI,  8'h4, 32'h00000000, rsp);
    `zynq.write_data(`MM2S_LENGTH, 8'h4, 512, rsp);

    // wait for DMA done
    i = 0;
    do begin
      data = 0;
      `zynq.read_data(`MM2S_DMASR, 8'h4, data, rsp);
      $display("SRRet: %b", data);
      `zynq.read_data(`MM2S_DMACR, 8'h4, data, rsp);
      $display("SRRet: %b", data);
      i = i + 1;
    end while(data[12] != 1'b1);

    // S2MM
    // transfer 64 bytes
    `zynq.write_data(`S2MM_DMACR,  8'h4, 32'b1000000000001, rsp);
    `zynq.write_data(`S2MM_DA_LO,  8'h4, 32'h0001_0200, rsp);
    `zynq.write_data(`S2MM_DA_HI,  8'h4, 32'h00000000, rsp);
    `zynq.write_data(`S2MM_LENGTH, 8'h4, 512, rsp);
    // wait for DMA done
    i = 0;
    do begin
      `zynq.read_data(`S2MM_DMASR, 8'h4, data, rsp);
      i = i + 1;
    end while(data[12] != 1'b1);

    repeat (8192) begin
      @(posedge `clkps);
    end

    `zynq.peek_mem_to_file("../../../data/dump.txt", 32'h0001_0000, 1024);
    $finish;
  end
endprogram


