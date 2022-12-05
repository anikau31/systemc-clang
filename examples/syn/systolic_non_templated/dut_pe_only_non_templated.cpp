#include "systolic_array_non_templated.h"


const int N1 = 4;
const int N2 = 4;
const int M = 8;
SC_MODULE(mymodule) {
  typedef sc_uint<8> T;
  typedef sc_uint<16> Tmult;
  typedef sc_uint<16> Tacc;

  pe_non_templated pe_inst[N1][N2];

  sc_clock clk;
  sc_signal<bool> rst;
  sc_signal<bool> enable_row_count_A;


  SC_CTOR(mymodule) {
    for(int i = 0; i < N1; i++) {
        for(int j = 0; j < N2; j++) {
            pe_inst[i][j].clk(clk);
            pe_inst[i][j].rst(rst);
        }
    }

  }

};
int sc_main(int argc, char** argvj) {
    mymodule mymod { "mymod" };
}