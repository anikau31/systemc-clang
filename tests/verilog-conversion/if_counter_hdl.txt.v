module Counter_0(
  input wire [0:0] pi_bClk,
  input wire [0:0] pi_bReset,
  input wire [0:0] pi_bUpDown,
  output reg [7:0] po_Count 
);
reg [7:0] m_Count;
always @(pi_bClk) begin: main

  if((pi_bReset) == (1)) begin
    m_Count <= 0;
  end else begin
    if((pi_bUpDown) == (0)) begin
      m_Count <= (m_Count) + (1);
    end else begin
      m_Count <= (m_Count) - (1);
    end
  end
end // main

always @(pi_bClk) begin: updateOutput

  po_Count <= m_Count;
end // updateOutput
endmodule // Counter_0
module Stimulus_0(
  output reg [0:0] po_bClk,
  output reg [0:0] po_bReset,
  output reg [0:0] po_bUpDown 
);


endmodule // Stimulus_0
