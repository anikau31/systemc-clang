// Taken directly from: http://www.asic-world.com/code/systemc/ports_arrays.cpp

#include <systemc.h>

template <typename T>
SC_MODULE(lift) {
  sc_in<bool> clk;

  T x;
  void lift_body() {}

  SC_CTOR(lift) {
    SC_METHOD(lift_body) { sensitive << clk.pos(); }
  }
};

SC_MODULE(ports_arrays) {
  // lift<int> submodule;
  //  lift<int> submodules[2];
  //
  //
  //  2d
  lift<int> submodules_2d[2][3];

  // 3d
  //lift<int> submodules_3d[2][3][2];

  sc_in<sc_uint<2>> a[4];
  sc_in<sc_uint<2>> b[4];
  sc_out<sc_uint<3>> o[4];

  sc_in<bool> test;

  int my_int_array[4];
  double my_double_array[8];
  const int x = 5;

  sc_out<sc_uint<2>> two_d[4][4];
  sc_out<sc_uint<2>> three_d[2][3][4];

  void body() {
    int i;
    for (i = 0; i < 4; i++) {
      o[i].write(a[i].read() + b[i].read());
    }
  }

  SC_CTOR(ports_arrays)
      :  //        submodule{"submodule_single"}
         //
         // //         1d
                // submodules{{"first_submod"}, {"second_submod"}}

        //,
        // 2d
        submodules_2d{
            {{{"submod_0_0"}}, {{"submod_0_1"}}, {{"submod_0_2"}}},
            {{{"submod_1_0"}}, {{"submod_1_1"}}, {{"submod_1_2"}}},
        }
//
        // 3d
        // 2,3,2
        // submodules_3d{
          // { { {"submod_0_0_0"}, {"submod_0_0_1"} },
            // { {"submod_0_1_0"}, {"submod_0_1_1"} },
            // { {"submod_0_2_0"}, {"submod_0_2_1"} }
          // },
          // { { {"submod_1_0_0"}, {"submod_1_0_1"} },
            // { {"submod_1_1_0"}, {"submod_1_1_1"} },
            // { {"submod_1_2_0"}, {"submod_1_2_1"} }
          // }
        // }  // end 3d
  {
    //
    //  submodule.clk(test);
    //
    // // 1d
    // submodules[0].clk(test);
    // submodules[1].clk(test);
//
    //
    // 2d
    submodules_2d[0][0].clk(test);

    //
    // 3d
    // submodules_3d[0][0][0].clk(test);

    int j;
    SC_METHOD(body);
    for (j = 0; j < 4; j++) {
      sensitive << a[j] << b[j];
    }
  }
};

SC_MODULE(DUT) {
  sc_signal<sc_uint<2>> as[4];
  sc_signal<sc_uint<2>> b[4];
  sc_signal<sc_uint<3>> o[4];
  ports_arrays par;

  sc_signal<bool> ts;

  SC_CTOR(DUT) : par("port_array_instance") {
    for (int z = 0; z < 4; z++) {
      int x = z;
      par.a[x](as[z]);
      par.test(ts);
      //      par.b[z](b[z]);
      //     par.o[z](o[z]);
    }
  }
};

// Testbench to generate test vectors
int sc_main(int argc, char* argv[]) {
  DUT design("design_under_test");

  // sc_signal<sc_uint<2> > a[4];
  // sc_signal<sc_uint<2> > b[4];
  // sc_signal<sc_uint<3> > o[4];
  //
  // int z;
  //
  // ports_arrays prt_ar("PORT_ARRAY");
  // for (z=0; z<4; z++) {
  // prt_ar.a[z](a[z]);
  // prt_ar.b[z](b[z]);
  // prt_ar.o[z](o[z]);
  // }

  /*
sc_start(0);
// Open VCD file
sc_trace_file *wf = sc_create_vcd_trace_file("ports_arrays");
((vcd_trace_file*)wf)->sc_set_vcd_time_unit(-5);
  for (z=0; z<4; z++) {
    char str[3];
    sprintf(str, "(%0d)",z);
    sc_trace(wf,a[z],"a" + string(str));
    sc_trace(wf,b[z],"b" + string(str));
    sc_trace(wf,o[z],"o" + string(str));
  }
// Start the testing here
sc_start(1);
for (z=0; z<4; z++) {
  a[z] = rand();
  b[z] = rand();
  sc_start(1);
  cout << "@" << sc_time_stamp() <<" a : " << a[z]
    << " ,b : " << b[z] << " ,o : " << o[z] << endl;
}
sc_start(2);

sc_close_vcd_trace_file(wf);
*/
  return 0;  // Terminate simulation
}
