#include "systemc.h"

SC_MODULE(submodule) {
  sc_in<int> input;
  sc_out<int> output;

  void entry() {}
  SC_CTOR(submodule) {
    SC_METHOD(entry) ;

  }
};

SC_MODULE(test) {
  sc_in_clk clk;

  sc_in<int> inS;
  sc_out<int> outS;
  sc_in<double> inT;
  sc_out<double> outT;

  sc_in<int> in_arr[4];

  submodule sub_module_member;

  submodule submod_1d[2];
  submodule submodules_2d[2][3];
  submodule submodules_3d[2][3][2];

  void entry() {
    while (true) {}
  }

  SC_CTOR(test) : 
    // single 
    // 0d
    sub_module_member("SUBMODULE"),
    //1d
    submod_1d{{"1d_submod_0"}, {"1d_submod_1"}},
      // 2d
        submodules_2d{
            {{{"submod_0_0"}}, {{"submod_0_1"}}, {{"submod_0_2"}}},
            {{{"submod_1_0"}}, {{"submod_1_1"}}, {{"submod_1_2"}}},
        },
//
 // 3d
 submodules_3d{
          { { {"submod_0_0_0"}, {"submod_0_0_1"} },
            { {"submod_0_1_0"}, {"submod_0_1_1"} },
            { {"submod_0_2_0"}, {"submod_0_2_1"} }
          },
          { { {"submod_1_0_0"}, {"submod_1_0_1"} },
            { {"submod_1_1_0"}, {"submod_1_1_1"} },
            { {"submod_1_2_0"}, {"submod_1_2_1"} }
          }
        } 

    {

// 0d
     sub_module_member.input(inS);
     sub_module_member.output(outS);

      // 1d
      submod_1d[0].output(outS);

      // 2d
      submodules_2d[1][2].input( inS );

      submodules_2d[0][1].output( submodules_2d[1][2].output );

      submodules_3d[1][2][1].input( inS );

    SC_METHOD(entry);
    sensitive << clk.pos();
  }
};

SC_MODULE(DUT) {
  sc_clock clk;
  sc_signal<int> sig1;

  test test_instance;
  SC_CTOR(DUT) : test_instance("testing") {
    test_instance.clk(clk);
    test_instance.inS(sig1);
    test_instance.outS(sig1);
  }
};

int sc_main(int argc, char *argv[]) {
  DUT d("dut");
  return 0;
}
