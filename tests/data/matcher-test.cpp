#include "systemc.h"

// Taken from: https://www.doulos.com/knowhow/systemc/faq/#q1
class MyType {
  private:
    unsigned info;
    bool flag;
  public:

    // constructor
    MyType (unsigned info_ = 0, bool flag_ = false) {
      info = info_;
      flag = flag_;
    }

    inline bool operator == (const MyType & rhs) const {
      return (rhs.info == info && rhs.flag == flag );
    }

    inline MyType& operator = (const MyType& rhs) {
      info = rhs.info;
      flag = rhs.flag;
      return *this;
    }

    inline friend void sc_trace(sc_trace_file *tf, const MyType & v,
    const std::string & NAME ) {
      sc_trace(tf,v.info, NAME + ".info");
      sc_trace(tf,v.flag, NAME + ".flag");
    }

    inline friend ostream& operator << ( ostream& os,  MyType const & v ) {
      os << "(" << v.info << "," << std::boolalpha << v.flag << ")";
      return os;
    }

};

SC_MODULE( test ){

  // clocks
  sc_in_clk clk1;
  sc_in_clk clk2;

  // input ports
  sc_in<int> in1;
  sc_in<int> in2;
  sc_in<MyType> myin;

  // inout ports
  sc_inout<double> in_out1;
  sc_inout<double> in_out2;
  sc_inout<double> in_out3;
  sc_inout<MyType> in_out4;

  // output ports
  sc_out<int> out1;
  sc_out<int> out2;
  sc_out<MyType> out3;

  //signals
  sc_signal<int> internal_signal1;
  sc_signal<bool> internal_signal2;
  sc_signal<double> internal_signal3;
  sc_signal<float> internal_signal4;
  sc_signal<MyType> internal_signal5;

  // others
  int x;
  double y;
  float z;
  MyType my_t;

  void entry_function_1() {
    while(true) {
    }
  }
  SC_CTOR( test ) {
    SC_METHOD(entry_function_1);
    sensitive << clk1.pos() << clk2.neg() ;
  }
};

int sc_main(int argc, char *argv[]) {
  sc_signal<int> sig1;
  sc_signal<double> double_sig;
  test test_instance("testing");
  test_instance.in1(sig1);
  test_instance.in_out1(double_sig);
  test_instance.out1(sig1);

  /*simple_module simple("simple_module");
  simple.one(sig1);
  */

  return 0;
}
