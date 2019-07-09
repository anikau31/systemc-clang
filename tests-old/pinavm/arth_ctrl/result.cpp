




//using namespace sc_core;





SC_MODULE(Component)
{
public:

  sc_out<bool> out;
  sc_in<bool> in;





 bool notified;
 bool isHead;

 SC_CTOR(Component):
  out("Out"),
  in("in")
  {
   notified = false;
   isHead = false;
   SC_THREAD(process);
  }

 void process() {
  if (isHead == false) {
   while (in.read() == false) {

    wait(5, SC_NS);



   }
  }
  out.write(true);
  notified = true;
 }
};

int sc_main (int argc , char *argv[])
{

  sc_signal<bool> s1;
  sc_signal<bool> s2;
  sc_signal<bool> s3;




 Component C1("C1");
 Component C2("C2");
 Component C3("C3");

 C1.out(s1);
 C2.out(s2);
 C3.out(s3);

 C1.in(s3);
 C2.in(s1);
 C3.in(s2);

 C1.isHead = true;

 //Run the Simulation for "200 nanosecnds"
 sc_start(200, SC_NS);

 return 0;
}




//using namespace sc_core;





class Component : public sc_module
{
public:

  sc_out<bool> out;
  sc_in<bool> in;





 bool notified;
 bool isHead;

 void process();

 SC_CTOR(Component):
  out("Out"),
  in("in")
  {
   notified = false;
   isHead = false;
   SC_THREAD(main);
  }

  void essaiwrite();

 void main() {
// 		if (isHead == false) {
// 			while (in.read() == false) {
// #ifdef KASCPAR
// 				wait(5);
// #else
// 				sc_core::wait(5, SC_NS);
// #endif
// 			}
// 		}
//		notified = true;
  out.write(true);
//		essaiwrite();
 }
};

void Component::essaiwrite()
{
 out.write(true);
}

int sc_main (int argc , char *argv[])
{

  sc_signal<bool> s1;
  sc_signal<bool> s2;
  sc_signal<bool> s3;




 Component C1("C1");
 Component C2("C2");
 Component C3("C3");

 C1.out(s1);
 C2.out(s2);
 C3.out(s3);

 C1.in(s3);
 C2.in(s1);
 C3.in(s2);

 C1.isHead = true;

 //Run the Simulation for "200 nanosecnds"
 sc_start(200, SC_NS);

 return 0;
}






class Component : public sc_module
{
public:
// 	sc_core::sc_out<bool> out;
// 	sc_core::sc_in<bool> in;

 sc_out<bool> out;
 sc_in<bool> in;

 bool notified;
 bool isHead;

 void process();

 SC_HAS_PROCESS(Component);

 Component(sc_module_name name) : sc_module(name) {
  SC_THREAD(main);
  notified = false;
  isHead = false;
 }

 void main();
};

void Component::main() {
 out->write(true);
 if (! isHead) {
  while (! in.read())
//				sc_core::wait(5, SC_NS);
   wait(5, SC_NS);
 }
 notified = true;
}
