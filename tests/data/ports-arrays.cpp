// Taken directly from: http://www.asic-world.com/code/systemc/ports_arrays.cpp

#include <systemc.h>

SC_MODULE(ports_arrays) {
  sc_in<sc_uint<2> > a[4];
  sc_in<sc_uint<2> > b[4];
  sc_out<sc_uint<3> > o[4];

  int my_int_array[4];
  double my_double_array[8];
  const int x = 5;

  void body () {
    int i;
    for (i=0; i < 4; i ++) { 
      o[i].write(a[i].read() + b[i].read());
    }
  }

  SC_CTOR(ports_arrays) {
    int j;
    SC_METHOD(body);
      for (j=0; j<4; j++) {
        sensitive << a[j] << b[j];
      }
  }
};

// Testbench to generate test vectors
int sc_main (int argc, char* argv[]) {
  sc_signal<sc_uint<2> > a[4];
  sc_signal<sc_uint<2> > b[4];
  sc_signal<sc_uint<3> > o[4];

  int z;

  ports_arrays prt_ar("PORT_ARRAY");
    for (z=0; z<4; z++) {
      prt_ar.a[z](a[z]);
      prt_ar.b[z](b[z]);
      prt_ar.o[z](o[z]);
    }

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
  return 0;// Terminate simulation
}
