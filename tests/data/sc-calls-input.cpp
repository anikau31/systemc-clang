#include "systemc.h"


/*
template<class T>
struct B : public sc_module
{
    sc_in_clk   clk{"clk"};
    sc_in<bool> rst{"rst"};

    B(const sc_module_name& name) : sc_module(name) {
    }
};

template<class T>
struct A : public B<T>
{
    T  m;
    sc_signal<bool> dummy{"dummy"};

    SC_HAS_PROCESS(A);

    A(const sc_module_name& name) : B<T>(name) {

        //SC_METHOD(virt_call); sensitive << dummy;

        SC_CTHREAD(simple_wait, this->clk.pos()); 
        this->async_reset_signal_is(this->rst, false);
    }

    virtual void d() { int x{2}; x++; };
    virtual void f(int i) =0;

    void simple_wait() {
        d();
        f(12);
        wait();

        while(1) wait();
    }
};


template<class T, unsigned>
class C 
{};

template<class T>
class C<T, 1> : public A<T>
{
public:

    C(const sc_module_name& name) : A<T>(name) 
    {}

    virtual void d() {
        this->m = 1;
    }
    virtual void f(int i) {
        this->m = i;
    }
};


class B_top : public sc_module 
{
public:
    sc_signal<bool>   clk{"clk"};
    sc_signal<bool>   rst{"rst"};

    C<int, 1> c_mod{"c_mod"};

    B_top(const sc_module_name& name) : sc_module(name) {
        c_mod.clk(clk);
        c_mod.rst(rst);
    }
};

int sc_main(int argc, char *argv[]) {
    B_top d{"d"};
    return 0;
}
*/

SC_MODULE(test) {
  sc_in_clk clk;
  sc_in<int> in1;
  sc_in<bool> reset;
  // output ports
  sc_out<int> out1;

  int x;
  int y;

  void simple_wait() {
    x = 4;
    y = 2;
    wait();
    while (true) {
      // supported waits
      wait();
      wait(4);

      // min,max,abs
      y = sc_min(x, y);
      x = sc_max(x, y);
      y = sc_abs(4.2);

      sc_int<1> bit_size = 0;
      sc_int<4> nibble_size = 1;
      sc_int<8> byte_size = 2;
      sc_int<32> dword_size = 3;
      // sc_int<128> addr; sc_int can not be more then 64
      //  Perform auto increment
      dword_size++;
      // Terse method addition
      byte_size += nibble_size;
      // Bit selection
      bit_size = dword_size[2];
      // Range selection
      nibble_size = dword_size.range(4, 1);  // Can not assign out of range
      // Concatenated
      dword_size = (byte_size, byte_size, byte_size, byte_size);

      sc_bv<8> data_bus;
      sc_bv<16> addr_bus;
      sc_bit parity;
      // Assign value to sc_bv
      data_bus = "00001011";
      // Use range operator
      addr_bus.range(7, 0) = data_bus;
      // Assign reverse to addr bus using range operator
      addr_bus.range(0, 7) = data_bus;
      // Use bit select to set the value
      addr_bus[10] = "1";
      // Use reduction operator
      parity = data_bus.or_reduce();
      parity = data_bus.xor_reduce();
      parity = data_bus.nor_reduce();
      parity = data_bus.xnor_reduce();
      // and_reduce
      parity = data_bus.and_reduce();
      parity = data_bus.nand_reduce();
    }
  }

  SC_CTOR(test) {
    SC_THREAD(simple_wait);  //{
    sensitive << clk.pos();
    async_reset_signal_is(reset, false);  // active low reset async
  }
};

SC_MODULE(DUT) {
  sc_signal<int> sig1;
  sc_signal<sc_int<64>> double_sig;

  test test_instance;

  SC_CTOR(DUT) : test_instance("testing") {
    test_instance.in1(sig1);
    test_instance.out1(sig1);
  }
};

int sc_main(int argc, char *argv[]) {
  DUT d("d");
  return 0;
}
