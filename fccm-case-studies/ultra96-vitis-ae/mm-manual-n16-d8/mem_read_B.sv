module mem_read_B
#
(
    parameter   D_W = 8,
    parameter   N = 3,
    parameter   M = 6
)
(
    input   wire                           clk,
    input   wire                           rd_en,
    input   wire    [$clog2((M*M)/N)-1:0]  rd_addr,

    output  wire    [$clog2((M*M)/N)-1:0]  rd_addr_bram [N-1:0],
    output  wire    [N-1:0]                rd_en_bram
);

assign rd_addr_bram[0]  = rd_addr;
assign rd_en_bram[0]    = rd_en;  

reg     [$clog2((M*M)/N)-1:0]      rd_addr_bram_reg [N-1:0];
reg     [N-1:0]                    rd_en_bram_reg;


genvar x;
for (x=1;x<N;x=x+1)
begin
    always@(posedge clk)
    begin
        rd_addr_bram_reg[x] <= rd_addr_bram[x-1]; 
        rd_en_bram_reg[x]   <= rd_en_bram[x-1];   
    end
    assign  rd_addr_bram[x] = rd_addr_bram_reg[x];
    assign  rd_en_bram[x]   = rd_en_bram_reg[x];
end

endmodule
