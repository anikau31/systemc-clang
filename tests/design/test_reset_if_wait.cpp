/******************************************************************************
 * Copyright (c) 2020, Intel Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception.
 *
 *****************************************************************************/

//#include "//assert.h"
#include <systemc.h>

// for loop with and w/o wait() general cases
class top : sc_module {
 public:
  sc_in_clk clk;
  sc_signal<bool> arstn{"arstn", 1};
  sc_signal<int> in{"in"};
  sc_signal<int> out{"out"};

  int x;
  int k;

  SC_HAS_PROCESS(top);
  top(sc_module_name) {
    SC_CTHREAD(reset_if_wait, clk.pos());
    async_reset_signal_is(arstn, false);
  }

  void reset_if_wait() {
    x = 0;
    k = 0;

    while (true) {
      k = 1;

      if (x++ & k) {
        k = 2;
        wait();
      } else {
        k = 3;
      }
      k = 4;

      wait();
    }
  };
};

void state_machine() {
  switch (state) {
    case S0:
      x = 0;
      k = 0;
      // skip while
      if () {
        k = 1;
        k = 2;
        next_state_ = S1;
      } else {
        k = 3;
        k = 4;
        next_state = S2;
      }
      break;

    case S1:
      k = 2;
      next_state = S2;
      break;

    case S2:
      k = 4;
      if () {
        k = 1;
        k = 2;
        next_state = S1;
      } else {
        k = 3;
        k = 4;
        next_state = S2;
      }
      break;
  }
}

int sc_main(int argc, char *argv[]) {
  sc_clock clk{"clk", 10, SC_NS};
  top top_inst{"top_inst"};
  top_inst.clk(clk);

  sc_start(100, SC_NS);
  return 0;
}
