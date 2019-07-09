#include <systemc.h>

SC_MODULE(example_fifo) {
  void m_drain_packets(void);
  void t_source1(void);
  void t_source2(void);
  // Constructor
  SC_CTOR(example_fifo) {
    SC_METHOD(m_drain_packets);
    SC_THREAD(t_source1);
    SC_THREAD(t_source2);
    // Size the packet_fifo to 5 ints.
    sc_fifo<int> packet_fifo (5);
  }
  
  // Declare the FIFO
  sc_fifo<int> packet_fifo;
  sc_fifo_in<int> inFifo;
};

void example_fifo::m_drain_packets(void) {
  int val;
  if (packet_fifo.nb_read(val)) {
    std::cout << sc_time_stamp() << ": m_drain_packets(): Received " << val <<
      std::endl;
  } else {
    std::cout << sc_time_stamp() << ": m_drain_packets(): FIFO empty." << std::endl;
  }
  // Check back in 2ns
  next_trigger(2, SC_NS);
}

void example_fifo::t_source1(void) {
  int val = 1000;
  for (;;) {
    wait(3, SC_NS);
    val++;
    packet_fifo.write(val);
    std::cout << sc_time_stamp() << ": t_thread1(): Wrote " << val << std::endl;
  }
}

void example_fifo::t_source2(void) {
  int val = 2000;
  for (;;) {
    wait(5, SC_NS);
    val++;
    packet_fifo.write(val);
    std::cout << sc_time_stamp() << ": t_thread2(): Wrote " << val << std::endl;
  }
}

int sc_main(int argc, char* argv[]) {
  sc_fifo<int> siFifo;

  example_fifo ex_fifo ("ex_fifo0");
  ex_fifo.inFifo(siFifo);

  sc_start(30, SC_NS);
  return 0;
}
