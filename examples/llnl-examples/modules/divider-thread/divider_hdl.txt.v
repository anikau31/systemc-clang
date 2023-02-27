module mymodule_sc_module_0 (
);
  logic [0:0] areset_scclang_global_0;
  logic [0:0] clk_scclang_global_1;
  logic [63:0] dividend_scclang_global_2;
  logic [63:0] divisor_scclang_global_3;
  logic [63:0] quotient_scclang_global_4;
  logic [0:0] valid_scclang_global_5;
  logic [0:0] vld_scclang_global_6;
  divider_sc_module_1 u_dut(
    .clk(clk_scclang_global_1),
    .arst(areset_scclang_global_0),
    .dividend(dividend_scclang_global_2),
    .divisor(divisor_scclang_global_3),
    .valid(valid_scclang_global_5),
    .quotient(quotient_scclang_global_4),
    .vld(vld_scclang_global_6)
  );
  always @(*) begin
  end

endmodule
module divider_sc_module_1 (
  input logic [0:0] clk,
  input logic [0:0] arst,
  input logic [63:0] dividend,
  input logic [63:0] divisor,
  input logic [0:0] valid,
  output logic [63:0] quotient,
  output logic [0:0] vld,
  output logic [0:0] busy
);
  logic [63:0] _op1_scclang_global_0;
  logic [127:0] _op2_scclang_global_1;
  logic [63:0] _quotient_scclang_global_2;
  logic [63:0] _res_scclang_global_3;
  logic [63:0] _temp_scclang_global_4;
  logic [0:0] _vld_scclang_global_5;
  logic [63:0] _main__op1_scclang_global_0;
  logic [127:0] _main__op2_scclang_global_1;
  logic [63:0] _main__quotient_scclang_global_2;
  logic [63:0] _main__temp_scclang_global_4;
  logic signed[31:0] i_thread_proc_1_thread_0;
  logic signed[31:0] state_thread_proc_1;
  logic signed[31:0] _next_state_thread_proc_1;
  logic signed[31:0] wait_counter_thread_proc_1;
  logic signed[31:0] _next_wait_counter_thread_proc_1;
  logic signed[31:0] wait_next_state_thread_proc_1;
  logic signed[31:0] _next_wait_next_state_thread_proc_1;
  logic signed[31:0] _main_i_thread_proc_1_thread_0;
  // Thread: thread_proc_1
  always @(negedge arst) begin: thread_proc_1_state_update
    if ((arst) == (0)) begin
      state_thread_proc_1 <= (0);
      wait_next_state_thread_proc_1 <= (0);
      wait_counter_thread_proc_1 <= (0);
      _main__op1_scclang_global_0 <= (0);
      _main__op2_scclang_global_1 <= (0);
      _main__quotient_scclang_global_2 <= (0);
      _main__temp_scclang_global_4 <= (0);
      _main_i_thread_proc_1_thread_0 <= (0);
    end else begin
      state_thread_proc_1 <= (_next_state_thread_proc_1);
      wait_counter_thread_proc_1 <= (_next_wait_counter_thread_proc_1);
      wait_next_state_thread_proc_1 <= (_next_wait_next_state_thread_proc_1);
      _main__op1_scclang_global_0 <= (_op1_scclang_global_0);
      _main__op2_scclang_global_1 <= (_op2_scclang_global_1);
      _main__quotient_scclang_global_2 <= (_quotient_scclang_global_2);
      _main__temp_scclang_global_4 <= (_temp_scclang_global_4);
      _main_i_thread_proc_1_thread_0 <= (i_thread_proc_1_thread_0);
    end

  end
  function automatic void thread_proc_1_func ();
  begin

    case(state_thread_proc_1)
      0: begin
        vld <= 0;
        busy <= 0;
        if (1) begin

        end
        if (valid) begin
          _quotient_scclang_global_2 <= 64'd0;
          _temp_scclang_global_4 <= 64'd0;
          _op1_scclang_global_0 <= dividend;
          _op2_scclang_global_1 <= divisor;
          vld <= 0;
          _next_state_thread_proc_1 = 1;
          return;
        end else begin
          _next_state_thread_proc_1 = 2;
          return;
        end

      end
      1: begin
        i_thread_proc_1_thread_0 = (64) - (1);
        if ((i_thread_proc_1_thread_0) >= (0)) begin
          if (((_temp_scclang_global_4) + ((_op2_scclang_global_1) << (i_thread_proc_1_thread_0))) <= (_op1_scclang_global_0)) begin
            _temp_scclang_global_4 <= (_temp_scclang_global_4) + ((_op2_scclang_global_1) << (1));
            _quotient_scclang_global_2 <= (_quotient_scclang_global_2) | ((1) << (i_thread_proc_1_thread_0));
            _next_state_thread_proc_1 = 3;
            return;
          end else begin
            _next_state_thread_proc_1 = 3;
            return;
          end

        end else begin
          vld <= 1;
          busy <= 0;
          quotient <= _quotient_scclang_global_2;
          _next_state_thread_proc_1 = 2;
          return;
        end

      end
      2: begin
        if (1) begin

        end
        if (valid) begin
          _quotient_scclang_global_2 <= 64'd0;
          _temp_scclang_global_4 <= 64'd0;
          _op1_scclang_global_0 <= dividend;
          _op2_scclang_global_1 <= divisor;
          vld <= 0;
          _next_state_thread_proc_1 = 1;
          return;
        end else begin
          _next_state_thread_proc_1 = 2;
          return;
        end

      end
      3: begin
        i_thread_proc_1_thread_0--;
        if ((i_thread_proc_1_thread_0) >= (0)) begin
          if (((_temp_scclang_global_4) + ((_op2_scclang_global_1) << (i_thread_proc_1_thread_0))) <= (_op1_scclang_global_0)) begin
            _temp_scclang_global_4 <= (_temp_scclang_global_4) + ((_op2_scclang_global_1) << (1));
            _quotient_scclang_global_2 <= (_quotient_scclang_global_2) | ((1) << (i_thread_proc_1_thread_0));
            _next_state_thread_proc_1 = 3;
            return;
          end else begin
            _next_state_thread_proc_1 = 3;
            return;
          end

        end else begin
          vld <= 1;
          busy <= 0;
          quotient <= _quotient_scclang_global_2;
          _next_state_thread_proc_1 = 2;
          return;
        end

      end
    endcase
  end
  endfunction
  always @(*) begin: thread_proc_1
    _next_state_thread_proc_1 = state_thread_proc_1;
    _next_wait_counter_thread_proc_1 = wait_counter_thread_proc_1;
    _next_wait_next_state_thread_proc_1 = wait_next_state_thread_proc_1;
    _op1_scclang_global_0 <= _main__op1_scclang_global_0;
    _op2_scclang_global_1 <= _main__op2_scclang_global_1;
    _quotient_scclang_global_2 <= _main__quotient_scclang_global_2;
    _temp_scclang_global_4 <= _main__temp_scclang_global_4;
    i_thread_proc_1_thread_0 = _main_i_thread_proc_1_thread_0;
    thread_proc_1_func();
  end
endmodule