#ifndef SYNTHESIS
  #include <systemc.h>
  #define END_IF  
  #define END_WHILE
  #define CTOR_END
#else  
  #include "systemc_synth.h"
#endif


SC_MODULE(Counter)
{
  sc_in<bool>   	    pi_bClk;
  sc_in<bool>         pi_bReset;
  
  sc_in<bool>         pi_bUpDown;   // false : Up
  
  sc_out<sc_uint<8> > po_Count;
  
  sc_signal<sc_uint<8> > m_Count;

  void main();
  void updateOutput();

  SC_CTOR(Counter) :
    pi_bClk("pi_bClk"),
    pi_bReset("pi_bReset"),
    pi_bUpDown("pi_bUpDown"), 
    po_Count("po_Count"),
    m_Count("m_Count")
  {
    SC_METHOD(main);
    //sensitive_pos << pi_bClk;
    sensitive << pi_bClk.pos();
    
    SC_METHOD(updateOutput);
    sensitive << m_Count; CTOR_END

  }
};

void 
Counter::main()
{
  if (pi_bReset.read() == true)
  {
    m_Count.write(sc_uint<8>(0));
  } 
  else
  {
    if (pi_bUpDown.read() == false)
    {
      m_Count.write(m_Count.read() + sc_uint<8>(1));
    }
    else
    {
      m_Count.write(m_Count.read() - sc_uint<8>(1));
    } 
    END_IF
  } 
  END_IF
}

void
Counter::updateOutput()
{
  po_Count.write(m_Count.read());
}

SC_MODULE(Stimulus)
{
  sc_out<bool>       po_bClk;
  sc_out<bool>       po_bReset;
  
  sc_out<bool>       po_bUpDown;   // false : Up  
 
  void clockProcess()
  {
    while(true)
    {
      po_bClk.write(true);
      wait(sc_time(10, SC_NS));
      po_bClk.write(false);
      wait(sc_time(10, SC_NS));
    } END_WHILE
  }
  
  void resetProcess()
  { 
    po_bReset.write(true);
    wait(sc_time(50, SC_NS));
    po_bReset.write(false);   
    wait(sc_time(100000, SC_NS));
  }
  
  void upDown()
  {
    while(true)
    {
      po_bUpDown.write(false);
      wait(sc_time(100, SC_NS));
      po_bUpDown.write(true);
      wait(sc_time(80, SC_NS));
    } END_WHILE
  }
  
  SC_CTOR(Stimulus) : 
    po_bClk("po_bClk"),
    po_bReset("po_bReset"),
    po_bUpDown("po_bUpDown")
  {
    SC_THREAD(clockProcess);
    SC_THREAD(resetProcess);
    SC_THREAD(upDown); CTOR_END
  }

};

int sc_main(int, char**)
{

  sc_signal<bool> m_bClk("m_bClk");
  sc_signal<bool> m_bReset("m_bReset");
  sc_signal<bool> m_bUpDown("m_bUpDown");   // false : Up
  sc_signal<sc_uint<8> > m_Count("m_Count");

  Counter c("c");

  c.pi_bClk(m_bClk);
  c.pi_bReset(m_bReset);
  c.pi_bUpDown(m_bUpDown);   // false : Up
  c.po_Count(m_Count);

  Stimulus s("s");

  s.po_bClk(m_bClk);
  s.po_bReset(m_bReset);
  s.po_bUpDown(m_bUpDown);   // false : Up  
  
  sc_trace_file *tf = sc_create_vcd_trace_file("trace");
  sc_trace(tf, m_bClk, "m_bClk");
  sc_trace(tf, m_bReset, "m_bReset");
  sc_trace(tf, m_bUpDown, "m_bUpDown");
  sc_trace(tf, m_Count, "m_Count");

  sc_start(sc_time(100000, SC_NS));

  return 0;
}
