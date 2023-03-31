module mymodule_sc_module_0 (
  input logic [63:0] dividend_data,
  input logic [0:0] dividend_valid,
  output logic [0:0] dividend_ready,
  input logic [63:0] divisor_data,
  input logic [0:0] divisor_valid,
  output logic [0:0] divisor_ready,
  output logic [63:0] quotient_data,
  output logic [0:0] quotient_valid,
  input logic [0:0] quotient_ready
);
  logic [0:0] areset_scclang_global_0;
  logic [0:0] clk_scclang_global_1;
  divider_sc_module_1 u_dut(
    .clk(clk_scclang_global_1),
    .arst(areset_scclang_global_0),
    .dividend_data(dividend_data),
    .dividend_valid(dividend_valid),
    .dividend_ready(dividend_ready),
    .divisor_data(divisor_data),
    .divisor_valid(divisor_valid),
    .divisor_ready(divisor_ready),
    .quotient_data(quotient_data),
    .quotient_valid(quotient_valid),
    .quotient_ready(quotient_ready)
  );
  always @(*) begin
  end

endmodule
module divider_sc_module_1 (
  input logic [0:0] clk,
  input logic [0:0] arst,
  input logic [63:0] dividend_data,
  input logic [0:0] dividend_valid,
  output logic [0:0] dividend_ready,
  input logic [63:0] divisor_data,
  input logic [0:0] divisor_valid,
  output logic [0:0] divisor_ready,
  output logic [63:0] quotient_data,
  output logic [0:0] quotient_valid,
  input logic [0:0] quotient_ready
);
  logic [0:0] _busy_scclang_global_0;
  logic [63:0] _op1_scclang_global_1;
  logic [0:0] _op1_vld_scclang_global_2;
  logic [127:0] _op2_scclang_global_3;
  logic [0:0] _op2_vld_scclang_global_4;
  logic [63:0] _quotient_scclang_global_5;
  logic [63:0] _res_scclang_global_6;
  logic [0:0] _sync_scclang_global_7;
  logic [0:0] _sync_out_scclang_global_8;
  logic [63:0] _temp_scclang_global_9;
  logic [0:0] _vld_scclang_global_10;
  logic [63:0] _main__op1_scclang_global_1;
  logic [0:0] _main__op1_vld_scclang_global_2;
  logic [127:0] _main__op2_scclang_global_3;
  logic [0:0] _main__op2_vld_scclang_global_4;
  logic [63:0] _main__quotient_scclang_global_5;
  logic [63:0] _main__temp_scclang_global_9;
  logic [0:0] _main__vld_scclang_global_10;
  logic [0:0] _main__sync_scclang_global_7;
  logic [0:0] _main__sync_out_scclang_global_8;
  logic signed[31:0] i_thread_proc_1_thread_0;
  logic signed[31:0] state_thread_proc_1;
  logic signed[31:0] _next_state_thread_proc_1;
  logic signed[31:0] wait_counter_thread_proc_1;
  logic signed[31:0] _next_wait_counter_thread_proc_1;
  logic signed[31:0] wait_next_state_thread_proc_1;
  logic signed[31:0] _next_wait_next_state_thread_proc_1;
  logic signed[31:0] _main_i_thread_proc_1_thread_0;
  always @(_vld_scclang_global_10 or _op1_vld_scclang_global_2 or _op2_vld_scclang_global_4) begin: mc_proc
    
    
    _sync_scclang_global_7 <= (dividend_valid) && (divisor_valid);
    dividend_ready <= _op1_vld_scclang_global_2;
    divisor_ready <= _op2_vld_scclang_global_4;
    quotient_valid <= _vld_scclang_global_10;
    quotient_data <= _quotient_scclang_global_5;
    _sync_out_scclang_global_8 <= quotient_ready;
  end
  // Thread: thread_proc_1
  always @(posedge clk or posedge arst) begin: thread_proc_1_state_update
    if ((arst) == (1)) begin
      state_thread_proc_1 <= (0);
      wait_next_state_thread_proc_1 <= (0);
      wait_counter_thread_proc_1 <= (0);
      _main__op1_scclang_global_1 <= (0);
      _main__op1_vld_scclang_global_2 <= (0);
      _main__op2_scclang_global_3 <= (0);
      _main__op2_vld_scclang_global_4 <= (0);
      _main__quotient_scclang_global_5 <= (0);
      _main__temp_scclang_global_9 <= (0);
      _main__vld_scclang_global_10 <= (0);
      _main__sync_scclang_global_7 <= (0);
      _main__sync_out_scclang_global_8 <= (0);
      _main_i_thread_proc_1_thread_0 <= (0);
    end else begin
      state_thread_proc_1 <= (_next_state_thread_proc_1);
      wait_counter_thread_proc_1 <= (_next_wait_counter_thread_proc_1);
      wait_next_state_thread_proc_1 <= (_next_wait_next_state_thread_proc_1);
      _main__op1_scclang_global_1 <= (_op1_scclang_global_1);
      _main__op1_vld_scclang_global_2 <= (_op1_vld_scclang_global_2);
      _main__op2_scclang_global_3 <= (_op2_scclang_global_3);
      _main__op2_vld_scclang_global_4 <= (_op2_vld_scclang_global_4);
      _main__quotient_scclang_global_5 <= (_quotient_scclang_global_5);
      _main__temp_scclang_global_9 <= (_temp_scclang_global_9);
      _main__vld_scclang_global_10 <= (_vld_scclang_global_10);
      _main__sync_scclang_global_7 <= (_sync_scclang_global_7);
      _main__sync_out_scclang_global_8 <= (_sync_out_scclang_global_8);
      _main_i_thread_proc_1_thread_0 <= (i_thread_proc_1_thread_0);
    end

  end
  function automatic void thread_proc_1_func ();
  begin

    case(state_thread_proc_1)
      0: begin
        _vld_scclang_global_10 = 0;
        _op1_vld_scclang_global_2 = 0;
        _op2_vld_scclang_global_4 = 0;
        _next_state_thread_proc_1 = 1;
        return;
      end
      1: begin
        if (1) begin

        end
        if (_sync_scclang_global_7) begin
          _op1_scclang_global_1 = dividend_data;
          _op2_scclang_global_3 = divisor_data;
          _op1_vld_scclang_global_2 = 1;
          _op2_vld_scclang_global_4 = 1;
          _next_state_thread_proc_1 = 2;
          return;
        end else begin
          if (_sync_out_scclang_global_8) begin
            _vld_scclang_global_10 = 0;
            _next_state_thread_proc_1 = 3;
            return;
          end else begin
            _next_state_thread_proc_1 = 3;
            return;
          end

        end

      end
      2: begin
        _op1_vld_scclang_global_2 = 0;
        _op2_vld_scclang_global_4 = 0;
        _quotient_scclang_global_5 = 64'd0;
        _temp_scclang_global_9 = 64'd0;
        _vld_scclang_global_10 = 0;
        _next_state_thread_proc_1 = 4;
        return;
      end
      3: begin
        if (1) begin

        end
        if (_sync_scclang_global_7) begin
          _op1_scclang_global_1 = dividend_data;
          _op2_scclang_global_3 = divisor_data;
          _op1_vld_scclang_global_2 = 1;
          _op2_vld_scclang_global_4 = 1;
          _next_state_thread_proc_1 = 2;
          return;
        end else begin
          if (_sync_out_scclang_global_8) begin
            _vld_scclang_global_10 = 0;
            _next_state_thread_proc_1 = 3;
            return;
          end else begin
            _next_state_thread_proc_1 = 3;
            return;
          end

        end

      end
      4: begin
        i_thread_proc_1_thread_0 = (64) - (1);
        if ((i_thread_proc_1_thread_0) >= (0)) begin
          if (((_temp_scclang_global_9) + ((_op2_scclang_global_3) << (i_thread_proc_1_thread_0))) <= (_op1_scclang_global_1)) begin
            _temp_scclang_global_9 = (_temp_scclang_global_9) + ((_op2_scclang_global_3) << (i_thread_proc_1_thread_0));
            _quotient_scclang_global_5 = (_quotient_scclang_global_5) | ((1) << (i_thread_proc_1_thread_0));
            _next_state_thread_proc_1 = 5;
            return;
          end else begin
            _next_state_thread_proc_1 = 5;
            return;
          end

        end else begin
          _vld_scclang_global_10 = 1;
          _next_state_thread_proc_1 = 6;
          return;
        end

      end
      5: begin
        i_thread_proc_1_thread_0--;
        if ((i_thread_proc_1_thread_0) >= (0)) begin
          if (((_temp_scclang_global_9) + ((_op2_scclang_global_3) << (i_thread_proc_1_thread_0))) <= (_op1_scclang_global_1)) begin
            _temp_scclang_global_9 = (_temp_scclang_global_9) + ((_op2_scclang_global_3) << (i_thread_proc_1_thread_0));
            _quotient_scclang_global_5 = (_quotient_scclang_global_5) | ((1) << (i_thread_proc_1_thread_0));
            _next_state_thread_proc_1 = 5;
            return;
          end else begin
            _next_state_thread_proc_1 = 5;
            return;
          end

        end else begin
          _vld_scclang_global_10 = 1;
          _next_state_thread_proc_1 = 6;
          return;
        end

      end
      6: begin
        if (_sync_out_scclang_global_8) begin
          _vld_scclang_global_10 = 0;
          _next_state_thread_proc_1 = 3;
          return;
        end else begin
          _next_state_thread_proc_1 = 3;
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
    _op1_scclang_global_1 = _main__op1_scclang_global_1;
    _op1_vld_scclang_global_2 = _main__op1_vld_scclang_global_2;
    _op2_scclang_global_3 = _main__op2_scclang_global_3;
    _op2_vld_scclang_global_4 = _main__op2_vld_scclang_global_4;
    _quotient_scclang_global_5 = _main__quotient_scclang_global_5;
    _temp_scclang_global_9 = _main__temp_scclang_global_9;
    _vld_scclang_global_10 = _main__vld_scclang_global_10;
    ;
    ;
    i_thread_proc_1_thread_0 = _main_i_thread_proc_1_thread_0;
    thread_proc_1_func();
  end
endmodule