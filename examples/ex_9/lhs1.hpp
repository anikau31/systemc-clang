#include "systemc.h"

extern sc_event pwait;
extern sc_event cwait;
extern int sharedVal;

SC_MODULE(consumer) {
 public:
  sc_in<int> value;
  sc_out<double> outResult;
  
  SC_CTOR(consumer) {
    SC_THREAD(consumer_entry);
    sensitive << value;
  }
  void consumer_entry() {
    while (true) {
      wait(cwait);
      int result;
      result = sharedVal * 10 + value.read();
      wait(1,SC_NS);
      outResult.write(result);
      pwait.notify();
    }
  }
};

