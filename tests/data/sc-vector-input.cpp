#include "systemc.h"

// vector with a size of 4.
template <class T>
class sc_vector4 : public sc_vector<T> {
	public:
		sc_vector4() : sc_vector<T>(4) {}
		explicit sc_vector4(const char* name_) : sc_vector<T>(name_, 4) {}
};

SC_MODULE( simple_module ){

  sc_in_clk clk;

  void entry_function_1() {
    while(true) {
    }
  }

  SC_CTOR( simple_module ) {
    SC_METHOD(entry_function_1);
    sensitive << clk.pos();
  }
};


SC_MODULE( test ){

  // input ports
  sc_in_clk clk;

  sc_vector4<simple_module> v4;
  sc_vector4<sc_vector4<simple_module>> v4v4;

   void run_this() {
      while(true) {
      }
  }
  SC_CTOR( test ) : v4("v4_once"), v4v4("v4_twice") {
    SC_METHOD(run_this);
    sensitive << clk.pos();

    v4[0].clk(clk);
    v4[1].clk(clk);
    v4[2].clk(clk);
    v4[3].clk(clk);

    // Just one of the sc_vector4
    v4v4[0][0].clk(clk);
    v4v4[0][1].clk(clk);
    v4v4[0][2].clk(clk);
    v4v4[0][3].clk(clk);
  }
};

SC_MODULE(DUT) {

sc_clock clock;
  // sc_signal<int> sig1;
  // sc_signal<double> double_sig;
//
  test test_instance;

//  simple_module simple;

  SC_CTOR(DUT) : test_instance("testing") {
  test_instance.clk(clock);

  // int others;
  // SC_CTOR(DUT) : test_instance("testing"), simple("simple_module_instance") {
  // test_instance.in1(sig1);
  // test_instance.in_out(double_sig);
  // test_instance.out1(sig1);
//
  // simple.one(sig1);
  }


};

int sc_main(int argc, char *argv[]) {
  DUT d("d");
   return 0;
}

