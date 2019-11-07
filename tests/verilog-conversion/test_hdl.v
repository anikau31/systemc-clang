module find_emax_0(
  input wire [0:0] clk,
  input wire [0:0] reset 
);
reg [0:0] c_sync;
reg [0:0] emax_v;
always @(*) begin: mc_proc

  reg c_sync;
  reg s_fp;
  reg c_sync;
  reg emax;
  reg emax_v;
  c_sync = ((s_fp) && (c_fp)) && ((!(emax_v)) || (c_ex));
end // mc_proc

always @(*) begin: ms_proc
  reg [0:0] last;
  if("NULL CHILD") begin
    count = (DIM) - (1);
    emax = 0;
    emax_v = 0;
  end else begin
    last = "# Unimplemented: CXXOperatorCallExpr";
    fp = s_fp;
    if(c_sync) begin
      if(last) begin
        count = (DIM) - (1);
      end else begin
        count = count;
      end
    end
    if((emax_v) && (c_ex)) begin
      if(s_fp) begin
        emax = fp;
      end else begin
        emax = 0;
      end
    end else begin
      if((s_fp) && ((fp) > (emax))) begin
        emax = fp;
      end
    end
    if((emax_v) && (c_ex)) begin
      emax_v = 0;
    end else begin
      if((c_sync) && (last)) begin
        emax_v = 1;
      end
    end
  end
end // ms_proc
endmodule // find_emax_0
