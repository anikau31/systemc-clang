`ifdef ultra96v2
`define zynq $root.tb.dut.sw.bd_i.zynq_ultra_ps_e_0.inst
`define clkps   $root.tb.dut.sw.bd_i.zynq_ultra_ps_e_0.pl_clk0
`elsif pynqz1
`define zynq $root.tb.dut.sw.bd_i.processing_system7_0.inst
`define clkps   $root.tb.dut.sw.bd_i.processing_system7_0.FCLK_CLK0
`define ps_por $root.tb.dut.FIXED_IO_ps_porb
`define ps_rst $root.tb.dut.FIXED_IO_ps_srstb
`endif

`define clk25mhz $root.tb.dut.sw.clk25mhz_0
`define clk50mhz $root.tb.dut.sw.clk50mhz_0
`define clk100mhz $root.tb.dut.sw.clk100mhz_0
`define NFLIT 107
`define FLIT_PER_TRANSFER 4

// we will need to access the MCDMA IP

`ifdef ultra96v2
`define GPIO       32'hA000_0000
`define MCDMA_BASE 32'hA001_0000
`define FIFO       32'hA002_0000
`elsif pynqz1
`define GPIO       32'h4000_0000
`define MCDMA_BASE 32'h4001_0000
`define FIFO       32'h4002_0000
`endif

`define MM2S_CHANNEL_BASE(k)  (`MCDMA_BASE + 32'h040 * k)
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



`define S2MM_CHANNEL_BASE(k)  (`MCDMA_BASE + 32'h500 + 32'h040 * k)
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

`define SG_USER_CACHE         (`MCDMA_BASE + 32'h4B0)

`ifdef ultra96v2
`define DDR_BASE 32'h0000_0000
`elsif pynqz1
`define DDR_BASE 32'h0008_0000
`endif

`define HDATA_BUFFER  32'h0000_1000
`define HTAP_BUFFER   32'h0000_2000
`define RES_BUFFER    32'h0001_0000
`define DESC_BUFFER   32'h0001_8000

program automatic test_case(); // interface to the design
  bit[1:0] rsp;
  bit[255:0] data;
  bit[255:0] htapfromshash[0:`NFLIT - 1];
  bit ps_por;
  bit ps_rst;
  assign `ps_por = ps_por;
  assign `ps_rst = ps_rst;
  integer i, j;
  initial begin
    $readmemh("../../../data/htapfromshash.mem", htapfromshash);

    $display("Test case");
    `ifdef ultra96v2
    `zynq.por_srstb_reset(1'b1); // must be called
    repeat (17) begin
      @(posedge `clkps);
    end
    `zynq.por_srstb_reset(1'b0);
    `elsif pynqz1
    ps_por = 1;
    ps_rst = 1;
    repeat(200) begin
      @(posedge `clkps);
    end
    ps_por = 0;
    ps_rst = 0;
    `endif
    `zynq.fpga_soft_reset(4'hF);
    repeat (17 * 4) begin
      @(posedge `clkps);
    end

    // minimum 16 clock pulse width delay
    `ifdef ultra96v2
    `zynq.por_srstb_reset(1'b1);
    `elsif pynqz1
    ps_por = 1;
    ps_rst = 1;
    `endif
    `zynq.fpga_soft_reset(4'h0);

    repeat(100) @(posedge `clkps);

    `zynq.pre_load_mem(2'b11, `DDR_BASE + 32'h0, 32'h2_0000);
    
    `zynq.pre_load_mem_from_file("../../../data/hdata.mem", `DDR_BASE + `HDATA_BUFFER, 455);
    `zynq.pre_load_mem_from_file("../../../data/htap.mem", `DDR_BASE +  `HTAP_BUFFER, 856);

    repeat(128) begin
      @(posedge `clkps);
    end

    // deassert resets
    $display("De-asserting resets");
    `zynq.write_data(`GPIO, 8'h4, 32'b1, rsp);

    repeat(128) begin
      @(posedge `clkps);
    end

    $display("Resetting MM2S CCR");
    `zynq.write_data(`MM2S_CCR, 8'h4, 32'b100, rsp);
    `zynq.wait_reg_update(`MM2S_CCR, 32'b000, ~32'b100, 20, 500, data); // wait until the reset is done

    // enable s2mm first
    setup_s2mm_descriptor(
      `DDR_BASE + `DESC_BUFFER,

      `DDR_BASE + `DESC_BUFFER,
      `DDR_BASE + `RES_BUFFER,
      // {1'b1, 1'b1, 4'h0, 25'd872 * 4},
      {1'b1, 1'b1, 4'h0, 26'(32 * `NFLIT)});
    `zynq.write_data(`S2MM_CHEN, 8'h4, 32'b011, rsp); // Channel 0 and 1 and 2
    // Program Current Descriptor
    `zynq.write_data(`S2MM_CURDESC_LSB(1), 8'h4, `DDR_BASE + `DESC_BUFFER, rsp);
    `zynq.write_data(`S2MM_CURDESC_MSB(1), 8'h4, 32'h0000_0000, rsp);

    // CHANNEL.Fetch bit of channel control registers
    `zynq.write_data(`S2MM_CR(1), 8'h4, 32'b11101001,rsp);
    // Start MCDMA.RS
    `zynq.write_data(`S2MM_CCR, 8'h4, 32'b1,rsp);
    // Interrupt thershold
    // Program the TD register (this triggers the fetching of the BD
    `zynq.write_data(`S2MM_TAILDESC_LSB(1), 8'h4, `DDR_BASE + `DESC_BUFFER, rsp);
    `ifdef ultra96v2
    `zynq.write_data(`S2MM_TAILDESC_MSB(1), 8'h4, 32'h0000_0000, rsp);
    `endif

    repeat(1024) begin
      @(posedge `clkps);
    end

    $display("MM2S Descriptor");
    for(j = 0; j < `NFLIT; j = j + `FLIT_PER_TRANSFER) begin
      logic[25:0] to_transfer = (`NFLIT - j) <  `FLIT_PER_TRANSFER ? (`NFLIT - j) : `FLIT_PER_TRANSFER;
      // Channel 1 Desc
      setup_mm2s_descriptor(
        `DDR_BASE + `DESC_BUFFER + 64'h40,

        `DDR_BASE + `DESC_BUFFER + 64'h40,
        `DDR_BASE + `HDATA_BUFFER + 32'd17 * j,
        // {1'b1, 1'b1, 4'h0, 25'd464 * 4},
        {1'b1, 1'b1, 4'h0, 26'(17 * to_transfer)},
        {8'h4, 8'h0, 16'h6});
      // Channel 2 Desc
      setup_mm2s_descriptor(
        `DDR_BASE + `DESC_BUFFER + 64'h80,

        `DDR_BASE + `DESC_BUFFER + 64'h80,
        `DDR_BASE + `HTAP_BUFFER + 32'd32 * j,
        // {1'b1, 1'b1, 4'h0, 25'd872 * 4},
        {1'b1, 1'b1, 4'h0, 26'(32 * to_transfer)},
        {8'h2, 8'h0, 16'h7});

      $display("Starting Transfer");
      // enable transfer
      // Enable required channels
      `zynq.write_data(`MM2S_CHEN, 8'h4, 32'b111, rsp); // Channel 0 and 1 and 2
      // Program Current Descriptor
      `zynq.write_data(`MM2S_CURDESC_LSB(1), 8'h4, `DDR_BASE + `DESC_BUFFER + 32'h0000_0040, rsp);
      `zynq.write_data(`MM2S_CURDESC_MSB(1), 8'h4, 32'h0000_0000, rsp);

      `zynq.write_data(`MM2S_CURDESC_LSB(2), 8'h4, `DDR_BASE + `DESC_BUFFER + 32'h0000_0080, rsp);
      `zynq.write_data(`MM2S_CURDESC_MSB(2), 8'h4, 32'h0000_0000, rsp);

      // CHANNEL.Fetch bit of channel control registers
      `zynq.write_data(`MM2S_CR(1), 8'h4, 32'b11101001,rsp);
      `zynq.write_data(`MM2S_CR(2), 8'h4, 32'b11101001,rsp);
      // Start MCDMA.RS
      `zynq.write_data(`MM2S_CCR, 8'h4, 32'b1,rsp);
      // Interrupt thershold
      // Program the TD register (this triggers the fetching of the BD
      `zynq.write_data(`MM2S_TAILDESC_LSB(1), 8'h4, `DDR_BASE + `DESC_BUFFER + 32'h0000_0040, rsp);
      `zynq.write_data(`MM2S_TAILDESC_MSB(1), 8'h4, 32'h0000_0000, rsp);

      `zynq.write_data(`MM2S_TAILDESC_LSB(2), 8'h4, `DDR_BASE + `DESC_BUFFER + 32'h0000_0080, rsp);
      `zynq.write_data(`MM2S_TAILDESC_MSB(2), 8'h4, 32'h0000_0000, rsp);

      `zynq.wait_reg_update(`MM2S_CSR, 32'b010, ~32'b010, 20, 16384, data);

      // `zynq.read_data(`MM2S_CHANNELS_SERVICED, 8'h4, data, rsp);
      // $display("Serviced Channel: %b", data[31:0]);
      // for(i = 0; i < 4; i = i + 1) begin
      //   `zynq.read_data(`MM2S_PKTCOUNT_STAT(i + 1), 8'h4, data, rsp);
      //   $display("MM2S Pkt Count: %d", data[15:0]);
      // end
    end

    // `zynq.read_data(`S2MM_CHANNELS_SERVICED, 8'h4, data, rsp);
    // $display("Serviced Channel: %b", data[31:0]);
    // `zynq.read_data(`S2MM_INTR_STATUS, 8'h4, data, rsp);
    // $display("Result: %b", data[31:0]);
    // $finish;

    `zynq.wait_reg_update(`S2MM_CSR, 32'b010, ~32'b010, 200, 16384, data);
    `zynq.read_data(`S2MM_CHANNELS_SERVICED, 8'h4, data, rsp);
    $display("Serviced Channel: %b", data[31:0]);
    for(i = 0; i < 4; i = i + 1) begin
      `zynq.read_data(`S2MM_PKTCOUNT_STAT(i + 1), 8'h4, data, rsp);
      $display("S2MM Pkt Count: %d", data[15:0]);
    end
    repeat(2048) begin
      @(posedge `clkps);
    end

    for(i = 0; i < `NFLIT; i = i + 1) begin
      // check for the resultant data
      data = 0;
      `zynq.read_mem(`DDR_BASE + `RES_BUFFER+ i * 32, 32, data[255:0]);
      $display("htapfromshash %x", data);

      if(data[255:0] != htapfromshash[i]) begin
        $error("htapfromshash %d does not match, expect %x, got %x", i, htapfromshash[i], data[255:0]);
        $finish;
      end

    end
    $display("Test passed.");

    $finish;
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

  // `zynq.write_data(base_addr + 32'h00, 8'h04, next_descriptor[31:0],  rsp);
  // `zynq.write_data(base_addr + 32'h04, 8'h04, next_descriptor[63:32], rsp);
  // `zynq.write_data(base_addr + 32'h08, 8'h04, buffer[31:0],           rsp);
  // `zynq.write_data(base_addr + 32'h0C, 8'h04, buffer[63:32],          rsp);
     
  // `zynq.write_data(base_addr + 32'h14, 8'h04, control,                rsp);
  // `zynq.write_data(base_addr + 32'h18, 8'h04, control_sideband,       rsp);
  // `zynq.write_data(base_addr + 32'h1C, 8'h04, 0,                      rsp);
     
  // `zynq.write_data(base_addr + 32'h20, 8'h04, app0,                   rsp);
  // `zynq.write_data(base_addr + 32'h24, 8'h04, app1,                   rsp);
  // `zynq.write_data(base_addr + 32'h28, 8'h04, app2,                   rsp);
  // `zynq.write_data(base_addr + 32'h2C, 8'h04, app3,                   rsp);
  // `zynq.write_data(base_addr + 32'h30, 8'h04, app4,                   rsp);
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

  // `zynq.write_data(base_addr + 32'h00, 8'h04, next_descriptor[31:0],  rsp);
  // `zynq.write_data(base_addr + 32'h04, 8'h04, next_descriptor[63:32], rsp);
  // `zynq.write_data(base_addr + 32'h08, 8'h04, buffer[31:0],           rsp);
  // `zynq.write_data(base_addr + 32'h0C, 8'h04, buffer[63:32],          rsp);
     
  // `zynq.write_data(base_addr + 32'h14, 8'h04, control,                rsp);
  // `zynq.write_data(base_addr + 32'h18, 8'h04, 0,                      rsp);
  // `zynq.write_data(base_addr + 32'h1C, 8'h04, 0,                      rsp);
     
  // `zynq.write_data(base_addr + 32'h20, 8'h04, app0,                   rsp);
  // `zynq.write_data(base_addr + 32'h24, 8'h04, app1,                   rsp);
  // `zynq.write_data(base_addr + 32'h28, 8'h04, app2,                   rsp);
  // `zynq.write_data(base_addr + 32'h2C, 8'h04, app3,                   rsp);
  // `zynq.write_data(base_addr + 32'h30, 8'h04, app4,                   rsp);

  end
  endtask
endprogram


