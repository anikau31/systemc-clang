module mem_write
#
(
    parameter   D_W = 8,
    parameter   N = 3,
    parameter   M = 6
)
(
    input   wire                            clk,
    input   wire                            rst,
    input   wire    [N-1:0]                 in_valid,
    input   wire    [D_W-1:0]               in_data [0:N-1],
    output  reg     [$clog2((M*M)/N)-1:0]   wr_addr_bram    [N-1:0],
    output  wire    [D_W-1:0]               wr_data_bram    [0:N-1],
    output  wire    [N-1:0]                 wr_en_bram
);

assign  wr_data_bram    = in_data;
assign  wr_en_bram      = (rst == 1) ? 0 : in_valid ;
genvar x;
for(x=0;x<N;x=x+1)
begin
    always@(posedge clk)
    begin
        if (rst)
        begin
            wr_addr_bram[x]  <= 0;
        end
        else if (in_valid[x]==1'b1)
        begin
            //wr_addr_bram[x] <= wr_addr_bram[x] + 1;
            wr_addr_bram[x] <= wr_addr_bram[x] < ((M*M)/N-1) ? wr_addr_bram[x] + 1 : 0;
        end
    end
end
endmodule
