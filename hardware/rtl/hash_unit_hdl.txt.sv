module mymodule_sc_module_0 (
);
  logic [0:0] clk;
  logic [0:0] reset;
  logic [135:0] hdata_data;
  logic [0:0] hdata_valid;
  logic [0:0] hdata_ready;
  logic [255:0] htaptoshash_data;
  logic [0:0] htaptoshash_valid;
  logic [0:0] htaptoshash_ready;
  logic [255:0] htapfromshash_data;
  logic [0:0] htapfromshash_valid;
  logic [0:0] htapfromshash_ready;
  short_hash_sc_module_1 u_dut(
    .clk(clk),
    .reset(reset),
    .s_dat_data(hdata_data),
    .s_dat_valid(hdata_valid),
    .s_dat_ready(hdata_ready),
    .s_tap_data(htaptoshash_data),
    .s_tap_valid(htaptoshash_valid),
    .s_tap_ready(htaptoshash_ready),
    .m_tap_data(htapfromshash_data),
    .m_tap_valid(htapfromshash_valid),
    .m_tap_ready(htapfromshash_ready)
  );
endmodule
module short_hash_sc_module_1 (
  input logic [0:0] clk,
  input logic [0:0] reset,
  input logic [135:0] s_dat_data,
  input logic [0:0] s_dat_valid,
  output logic [0:0] s_dat_ready,
  input logic [255:0] s_tap_data,
  input logic [0:0] s_tap_valid,
  output logic [0:0] s_tap_ready,
  output logic [255:0] m_tap_data,
  output logic [0:0] m_tap_valid,
  input logic [0:0] m_tap_ready
);
  logic [63:0] a[0:11];
  logic [63:0] b[0:11];
  logic [63:0] c[0:11];
  logic [63:0] d[0:11];
  logic [11:0] v;
  initial begin


  end
  always @(s_dat_valid or s_tap_valid or m_tap_ready or a[(11)] or b[(11)] or c[(11)] or d[(11)] or v) begin: mc_mix
    
    
    s_dat_ready <= (m_tap_ready) && (s_tap_valid);
    s_tap_ready <= (m_tap_ready) && (s_dat_valid);
    m_tap_data <= {({({(d[(11)]), (c[(11)])}), (b[(11)])}), (a[(11)])};
    m_tap_valid <= v[11];
  end
  always @(posedge clk) begin: ms_mix
    logic [63:0] len_local_0 = 64'h0;
    logic [63:0] data_local_1[0:1] = {64'h0, 64'h0};
    logic [63:0] tapi_local_2[0:3] = {64'h0, 64'h0, 64'h0, 64'h0};
    logic signed[31:0] i_local_3;
    logic [63:0] a__ref_0[0:11];
    logic [63:0] c__ref_0[0:11];
    logic [63:0] d__ref_0[0:11];
    logic [63:0] b__ref_0[0:11];
    
    b__ref_0 = b;
    d__ref_0 = d;
    c__ref_0 = c;
    a__ref_0 = a;
    if ((reset) == (0)) begin
      for (i_local_3 = 0;(i_local_3) < (12);i_local_3 = i_local_3 + 1) begin
        a__ref_0[i_local_3] = 64'd0;
        b__ref_0[i_local_3] = 64'd0;
        c__ref_0[i_local_3] = 64'd0;
        d__ref_0[i_local_3] = 64'd0;
      end
      v <= 12'd0;
    end else begin
      if (m_tap_ready) begin
        { { len_local_0[(8) - (1):0], data_local_1[1] }, data_local_1[0] } = s_dat_data;
        { { { tapi_local_2[3], tapi_local_2[2] }, tapi_local_2[1] }, tapi_local_2[0] } = s_tap_data;
        a__ref_0[0] = (tapi_local_2[(0)]) ^ (len_local_0);
        b__ref_0[0] = (tapi_local_2[(1)]) ^ (~(len_local_0));
        c__ref_0[0] = (tapi_local_2[(2)]) + (data_local_1[(0)]);
        d__ref_0[0] = (tapi_local_2[(3)]) + (data_local_1[(1)]);
        short_hashemix(d,c,b,a,15,1,d__ref_0,c__ref_0,b__ref_0,a__ref_0);
        short_hashemix(a,d,c,b,52,2,a__ref_0,d__ref_0,c__ref_0,b__ref_0);
        short_hashemix(b,a,d,c,26,3,b__ref_0,a__ref_0,d__ref_0,c__ref_0);
        short_hashemix(c,b,a,d,51,4,c__ref_0,b__ref_0,a__ref_0,d__ref_0);
        short_hashemix(d,c,b,a,28,5,d__ref_0,c__ref_0,b__ref_0,a__ref_0);
        short_hashemix(a,d,c,b,9,6,a__ref_0,d__ref_0,c__ref_0,b__ref_0);
        short_hashemix(b,a,d,c,47,7,b__ref_0,a__ref_0,d__ref_0,c__ref_0);
        short_hashemix(c,b,a,d,54,8,c__ref_0,b__ref_0,a__ref_0,d__ref_0);
        short_hashemix(d,c,b,a,32,9,d__ref_0,c__ref_0,b__ref_0,a__ref_0);
        short_hashemix(a,d,c,b,25,10,a__ref_0,d__ref_0,c__ref_0,b__ref_0);
        short_hashemix(b,a,d,c,63,11,b__ref_0,a__ref_0,d__ref_0,c__ref_0);
        v <= ((v) << (1)) | ((s_dat_valid) && (s_tap_valid));
      end
    end

    a <= a__ref_0;
    c <= c__ref_0;
    d <= d__ref_0;
    b <= b__ref_0;
  end
  function automatic void short_hashemix (input logic [63:0] y[0:11], input logic [63:0] x[0:11], input logic [63:0] u[0:11], input logic [63:0] t[0:11], input logic [31:0] ro, input logic [31:0] ns, inout logic [63:0] y__ref_0[0:11], inout logic [63:0] x__ref_0[0:11], inout logic [63:0] u__ref_0[0:11], inout logic [63:0] t__ref_0[0:11]);
  begin
    logic [63:0] xt_local_0;
    logic [63:0] xr_local_1;
    xt_local_0 = x[(ns) - (1)];
    xr_local_1 = (xt_local_0 << ro) | ($unsigned(xt_local_0) >> ($bits(xt_local_0) - ro));
    t__ref_0[ns] = t[(ns) - (1)];
    u__ref_0[ns] = u[(ns) - (1)];
    x__ref_0[ns] = xr_local_1;
    y__ref_0[ns] = ((y[((ns) - (1))]) ^ (xt_local_0)) + (xr_local_1);
  end
  endfunction
endmodule
