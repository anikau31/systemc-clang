#include "systemc.h"

SC_MODULE(FIR) {
  sc_in_clk clk;
  sc_in<double> sample;       
  static const int ORDER = 5;
  static const double single;
  static const double c[ORDER+1];
  int x;
  /*
  sc_out<double> result;
  sc_int<32> myint;
  sc_signal<int> mysignal;
  */
  SC_CTOR(FIR) {      
    SC_METHOD(behavior);
    sensitive << clk.neg();
  }
  private:

  sc_buffer<double> i[ORDER];

  void behavior() {
    double sum = c[0] * sample.read();
    for (int j=1; j<=ORDER; ++j)
      sum = sum + c[j] * i[j-1].read();
    //result.write(sum);

    i[0].write(sample.read());
    for (int j=1; j<ORDER; ++j)
      i[j].write(i[j-1].read());

    x = sum;
  }
};

int sc_main(int argc, char *argv[]) {
    sc_signal<double> sample;
    FIR  fir("test_fir");
    fir.sample(sample);

    return 0;
}
