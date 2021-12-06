#include "systemc.h"
//#define wait(...) if (true) {wait(__VA_ARGS__);}

SC_MODULE(test) {
  // input ports
  sc_in_clk clk;
  sc_in<int> in1;
  // output ports
  sc_out<int> out1;

  // others
  int x;
  int k;

  void break0() {
    k = 0;
    while (true) {
      k = 1;
      for (int i = 0; i < 2; i++) {
        k = 2;
        break;
        k++;
      }
      k = 3;
      wait();
      k = 4;
    }
  }

  void break1() {
    k = 0;
    while (true) {
      k = 1;
      for (int i = 0; i < 2; i++) {
        k = 2;
        if (i == 1) {
          k = 3;
          break;
        k++;
        }
      }
      k = 4;
      wait();
      k = 5;
    }
  }



  void break2() {
    k = 0;
    while (true) {
      k = 1;
      wait();

      for (int i = 0; i < 2; i++) {
        k = 2;
        break;
      }
      k = 3;
    }
  }

  void break3() {
    k = 0;
    while (true) {
      k = 1;
      wait();

      for (int i = 0; i < 2; i++) {
        k = 2;
        if (i == 1) { 
          break;
        }
        wait();
      }
      k = 3;
    }
  }

  void break4() {
    k = 0;
    while (true) {
      k = 1;
      wait();

      for (int i = 0; i < 2; i++) {
        k = 2;
        wait();
        k = 3;
        if (i == 1) { 
          break;
        }
        wait();
        k = 4;
      }
      k = 5;
    }
  }


  void break_with_wait() {
    k = 0;
    while (true) {
      k = 1;
      wait();

      for (int i = 0; i < 2; i++) {
        k = 2;
        wait();
        break;
      }
      k = 3;
    }
  }


  void break_with_if_wait0() {
    k = 0;
    while (true) {
      k = 1;
      wait();

      for (int i = 0; i < 2; i++) {
        k = 2;
        wait();
        k = 3;

        if (i == 1) {
          k = 4;
          break;
        }
      }
      k = 5;
    }
  }


  void break_with_if_wait1() {
    k = 0;
    while (true) {
      k = 1;
      wait();

      for (int i = 0; i < 2; i++) {
        k = 2;
        wait();
        k = 3;

        if (i == 1) {
          k = 4;
          wait();
          k = 5;
          break;
        }
      }
      k = 6;
    }
  }


  SC_CTOR(test) {
    int x{2};
    /*
    SC_THREAD(break0);
    sensitive << clk.pos();

    SC_THREAD(break1);
    sensitive << clk.pos();
    */

    SC_THREAD(break2);
    sensitive << clk.pos();
    /*

    SC_THREAD(break3);
    sensitive << clk.pos();

    SC_THREAD(break4);
    sensitive << clk.pos();

    SC_THREAD(break_with_wait);
    sensitive << clk.pos();

    SC_THREAD(break_with_if_wait0);
    sensitive << clk.pos();

    SC_THREAD(break_with_if_wait1);
    sensitive << clk.pos();
    */
  }
};

SC_MODULE(DUT) {
  sc_signal<int> sig1;
  sc_signal<double> double_sig;

  test test_instance;

  int others;
  SC_CTOR(DUT) : test_instance("testing") {
    test_instance.in1(sig1);
    test_instance.out1(sig1);
  }
};

int sc_main(int argc, char *argv[]) {
  DUT d("d");
  return 0;
}
