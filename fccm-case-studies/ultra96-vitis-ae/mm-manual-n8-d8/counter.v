module counter
#(
  parameter   WIDTH   = 32,
  parameter   HEIGHT  = 32

)
(
  input   wire clk,
  input   wire rst,
  input   wire enable_row_count,
  output  reg  [($clog2(WIDTH)?$clog2(WIDTH):1)-1:0]     pixel_cntr,
  output  reg  [($clog2(HEIGHT)?$clog2(HEIGHT):1)-1:0]    slice_cntr

);

always@(posedge clk)
begin
  if (rst) begin
    pixel_cntr  <= 0;
  end else begin
    if (pixel_cntr == WIDTH-1) begin
      pixel_cntr  <= 0;
    end else begin
      pixel_cntr  <= (pixel_cntr + 1);
    end
  end
end

always@(posedge clk)
begin
  if (rst) begin
    slice_cntr <= 0;
  end else begin
    if (enable_row_count && pixel_cntr == WIDTH-1) begin
      if (slice_cntr == HEIGHT-1) begin
        slice_cntr <= 0;
      end else begin
        slice_cntr <= slice_cntr + 1;
      end
    end
  end
end

endmodule
