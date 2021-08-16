`define zynq $root.tb.dut.sw.bd_i.zynq_ultra_ps_e_0.inst
`define clk25mhz $root.tb.dut.sw.clk25mhz_0
`define clk50mhz $root.tb.dut.inst_rp.clk50mhz_0
`define clk100mhz $root.tb.dut.sw.clk100mhz_0
`define clkps   $root.tb.dut.sw.bd_i.zynq_ultra_ps_e_0.pl_clk0

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


`define GPIO       32'hA000_0000
`define MCDMA_BASE 32'hA001_0000
`define FIFO       32'hA002_0000

`define MM2S_CHANNEL_BASE(k)   (`MCDMA_BASE + 32'h040 * k)
`define MM2S_CCR               (`MM2S_CHANNEL_BASE(0) + 32'h00)
`define MM2S_CSR               (`MM2S_CHANNEL_BASE(0) + 32'h04)
`define MM2S_CHEN              (`MM2S_CHANNEL_BASE(0) + 32'h08)
`define MM2S_CHSER             (`MM2S_CHANNEL_BASE(0) + 32'h0C)
`define MM2S_ERR               (`MM2S_CHANNEL_BASE(0) + 32'h10)
`define MM2S_CH_SCHD_TYPE      (`MM2S_CHANNEL_BASE(0) + 32'h14)
`define MM2S_WRR_REG1          (`MM2S_CHANNEL_BASE(0) + 32'h18)
`define MM2S_WRR_REG2          (`MM2S_CHANNEL_BASE(0) + 32'h1C)
`define MM2S_CHANNELS_SERVICED (`MM2S_CHANNEL_BASE(0) + 32'h20)
`define MM2S_ARCACHE_ARUSER    (`MM2S_CHANNEL_BASE(0) + 32'h24)
`define MM2S_INTR_STATUS       (`MM2S_CHANNEL_BASE(0) + 32'h28)

`define MM2S_CR(k)            (`MM2S_CHANNEL_BASE(k) + 32'h000)
`define MM2S_SR(k)            (`MM2S_CHANNEL_BASE(k) + 32'h004)
`define MM2S_CURDESC_LSB(k)   (`MM2S_CHANNEL_BASE(k) + 32'h008)
`define MM2S_CURDESC_MSB(k)   (`MM2S_CHANNEL_BASE(k) + 32'h00C)
`define MM2S_TAILDESC_LSB(k)  (`MM2S_CHANNEL_BASE(k) + 32'h010)
`define MM2S_TAILDESC_MSB(k)  (`MM2S_CHANNEL_BASE(k) + 32'h014)
`define MM2S_PKTCOUNT_STAT(k) (`MM2S_CHANNEL_BASE(k) + 32'h018)



`define S2MM_CHANNEL_BASE(k)   (`MCDMA_BASE + 32'h500 + 32'h040 * k)
`define S2MM_CCR               (`S2MM_CHANNEL_BASE(0) + 32'h00)
`define S2MM_CSR               (`S2MM_CHANNEL_BASE(0) + 32'h04)
`define S2MM_CHEN              (`S2MM_CHANNEL_BASE(0) + 32'h08)
`define S2MM_CHSER             (`S2MM_CHANNEL_BASE(0) + 32'h0C)
`define S2MM_ERR               (`S2MM_CHANNEL_BASE(0) + 32'h10)
`define S2MM_PKTDROP           (`S2MM_CHANNEL_BASE(0) + 32'h14)
`define S2MM_CHANNELS_SERVICED (`S2MM_CHANNEL_BASE(0) + 32'h18)
`define S2MM_AWCACHE_AWUSER    (`S2MM_CHANNEL_BASE(0) + 32'h1C)
`define S2MM_INTR_STATUS       (`S2MM_CHANNEL_BASE(0) + 32'h20)

`define S2MM_CR(k)            (`S2MM_CHANNEL_BASE(k) + 32'h000)
`define S2MM_SR(k)            (`S2MM_CHANNEL_BASE(k) + 32'h004)
`define S2MM_CURDESC_LSB(k)   (`S2MM_CHANNEL_BASE(k) + 32'h008)
`define S2MM_CURDESC_MSB(k)   (`S2MM_CHANNEL_BASE(k) + 32'h00C)
`define S2MM_TAILDESC_LSB(k)  (`S2MM_CHANNEL_BASE(k) + 32'h010)
`define S2MM_TAILDESC_MSB(k)  (`S2MM_CHANNEL_BASE(k) + 32'h014)
`define S2MM_PKTDROP_STAT(k)  (`S2MM_CHANNEL_BASE(k) + 32'h018)
`define S2MM_PKTCOUNT_STAT(k) (`S2MM_CHANNEL_BASE(k) + 32'h01C)

program automatic test_case(); // interface to the design
  bit[1:0] rsp;
  bit[127:0] data;
  integer i;
  bit[63:0] z3exp_d2[0:16];
  bit[63:0] z3act_d2[0:16];
  initial begin
    $display("Test case");
    // Load golden output from systemc
    $readmemh("../../../data/z3true.mem", z3exp_d2);
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
    `zynq.pre_load_mem(2'b11, 32'h0, 32'h8_0000);
    `zynq.pre_load_mem_from_file("../../../data/test_zynq_blank.mem", 32'h0001_0000, 8192);

    repeat(128) begin
      @(posedge `clkps);
    end


    `zynq.write_data(`GPIO, 8'h4, 32'b1, rsp);

    repeat(128) begin
      @(posedge `clkps);
    end

    
    $display("Resetting MM2S CCR");
    `zynq.write_data(`MM2S_CCR, 8'h4, 32'b100, rsp);
    `zynq.wait_reg_update(`MM2S_CCR, 32'b000, ~32'b100, 20, 500, data); // wait until the reset is done
    // enable s2mm first
    setup_s2mm_descriptor(
      64'h0004_0000,

      64'h0004_0000,
      64'h0001_0200,
      {1'b1, 1'b1, 4'h0, 26'(128)});
    `zynq.write_data(`S2MM_CHEN, 8'h4, 32'b011, rsp); // Channel 0 and 1 and 2
    // Program Current Descriptor
    `zynq.write_data(`S2MM_CURDESC_LSB(1), 8'h4, 32'h0004_0000, rsp);
    `zynq.write_data(`S2MM_CURDESC_MSB(1), 8'h4, 32'h0000_0000, rsp);

    setup_mm2s_descriptor(
      64'h0004_0040,

      64'h0004_0040,
      64'h0001_0000,
      {1'b1, 1'b1, 4'h0, 26'(128)},
      {8'h4, 8'h0, 16'h6});
    $display("Starting Transfer");
    // enable transfer
    // Enable required channels
    `zynq.write_data(`MM2S_CHEN, 8'h4, 32'b001, rsp); // Channel 0 and 1 and 2
    // Program Current Descriptor
    `zynq.write_data(`MM2S_CURDESC_LSB(1), 8'h4, 32'h0004_0040, rsp);
    `zynq.write_data(`MM2S_CURDESC_MSB(1), 8'h4, 32'h0000_0000, rsp);

    // CHANNEL.Fetch bit of channel control registers
    `zynq.write_data(`MM2S_CR(1), 8'h4, 32'b11101001,rsp);
    // Start MCDMA.RS
    `zynq.write_data(`MM2S_CCR, 8'h4, 32'b1,rsp);
    // Interrupt thershold
    // Program the TD register (this triggers the fetching of the BD
    `zynq.write_data(`MM2S_TAILDESC_LSB(1), 8'h4, 32'h0004_0040, rsp);
    `zynq.write_data(`MM2S_TAILDESC_MSB(1), 8'h4, 32'h0000_0000, rsp);

    `zynq.wait_reg_update(`MM2S_CSR, 32'b010, ~32'b010, 20, 16384, data);

    `zynq.peek_mem_to_file("../../../data/dump.txt", 32'h0001_0000, 1024);

    repeat(100) @(posedge `clk50mhz);

    for(int i = 0; i < 16; i++) begin
      if(z3exp_d2[i] != z3act_d2[i]) begin
        $fatal("Mismatched datum");
      end
    end
    $display("Test passed");

    $finish;
  end
  initial begin
    integer i = 0;
    while(1) begin
      @(posedge `clk50mhz);
        if($root.tb.dut.inst_rp.dut.c_dut_enc_valid && $root.tb.dut.inst_rp.dut.c_dut_enc_ready) begin
          z3act_d2[i] = $root.tb.dut.inst_rp.dut.c_dut_enc_data_tdata;
          if($root.tb.dut.inst_rp.dut.c_dut_enc_data_tlast) begin
            break;
          end
          i = i + 1;
        end
    end
  end
  task automatic setup_mm2s_descriptor(
    input logic[63:0] base_addr,

    input logic[63:0] next_descriptor,
    input logic[63:0] buffer,
    input logic[31:0] control,
    input logic[31:0] control_sideband,

    input logic[31:0] app0 = 0,
    input logic[31:0] app1 = 0,
    input logic[31:0] app2 = 0,
    input logic[31:0] app3 = 0,
    input logic[31:0] app4 = 0
  );
  begin
    bit[1:0] rsp;
    `zynq.write_mem(next_descriptor[31:0],  base_addr + 32'h00, 32'h04);
    `zynq.write_mem(next_descriptor[63:32], base_addr + 32'h04, 32'h04);
    `zynq.write_mem(buffer[31:0],           base_addr + 32'h08, 32'h04);
    `zynq.write_mem(buffer[63:32],          base_addr + 32'h0C, 32'h04);

    `zynq.write_mem(control,                base_addr + 32'h14, 32'h04);
    `zynq.write_mem(control_sideband,       base_addr + 32'h18, 32'h04);
    `zynq.write_mem(0,                      base_addr + 32'h1C, 32'h04);

    `zynq.write_mem(app0,                   base_addr + 32'h20, 32'h04);
    `zynq.write_mem(app1,                   base_addr + 32'h24, 32'h04);
    `zynq.write_mem(app2,                   base_addr + 32'h28, 32'h04);
    `zynq.write_mem(app3,                   base_addr + 32'h2C, 32'h04);
    `zynq.write_mem(app4,                   base_addr + 32'h30, 32'h04);
  end
  endtask
  task automatic setup_s2mm_descriptor(
    input logic[63:0] base_addr,

    input logic[63:0] next_descriptor,
    input logic[63:0] buffer,
    input logic[31:0] control,

    input logic[31:0] app0 = 0,
    input logic[31:0] app1 = 0,
    input logic[31:0] app2 = 0,
    input logic[31:0] app3 = 0,
    input logic[31:0] app4 = 0
  );
  begin
    bit[1:0] rsp;
    `zynq.write_mem(next_descriptor[31:0],  base_addr + 32'h00, 32'h04);
    `zynq.write_mem(next_descriptor[63:32], base_addr + 32'h04, 32'h04);
    `zynq.write_mem(buffer[31:0],           base_addr + 32'h08, 32'h04);
    `zynq.write_mem(buffer[63:32],          base_addr + 32'h0C, 32'h04);

    `zynq.write_mem(control,                base_addr + 32'h14, 32'h04);
    `zynq.write_mem(0,                      base_addr + 32'h18, 32'h04);
    `zynq.write_mem(0,                      base_addr + 32'h1C, 32'h04);

    `zynq.write_mem(app0,                   base_addr + 32'h20, 32'h04);
    `zynq.write_mem(app1,                   base_addr + 32'h24, 32'h04);
    `zynq.write_mem(app2,                   base_addr + 32'h28, 32'h04);
    `zynq.write_mem(app3,                   base_addr + 32'h2C, 32'h04);
    `zynq.write_mem(app4,                   base_addr + 32'h30, 32'h04);
  end
  endtask
endprogram


