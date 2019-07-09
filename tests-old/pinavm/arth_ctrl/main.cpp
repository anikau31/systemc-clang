#include <systemc.h>

//using namespace sc_core;

//#define KASCPAR

#define NS * 1e-9

class superclass
{
	virtual void print() {}
};

class Component : public sc_module//, public superclass
{
public:
/*#ifdef KASCPAR
 	sc_out<bool> out;
 	sc_in<bool> in;
#else*/	
	sc_core::sc_out<bool> out;
	sc_core::sc_in<bool> in;
//#endif
	
	bool notified;
	bool isHead;
  SC_HAS_PROCESS(Component);

    Component(sc_module_name name) {
    //  SC_CTOR(Component)	{
                        int i=1;
			notified = false;
			isHead = false;
			SC_THREAD(process);
		}
	
 	void essaiwrite();

	void process() {
	  /*      if (isHead == false) {
 			while (in.read() == false) {
 #ifdef KASCPAR
 				wait(5, SC_NS);
 #else
 				sc_core::wait(5, SC_NS);
 #endif
 			}
 		}
 		notified = true;*/
	  int i = 5;
	  int j = 3;
	  //   std::cout<<j;
	  int res = i + j;
	  res++;
	  //	  std::cout<<res;
	  int res1 = i * j;
	  res++;
 	  int res2 = i / j;
	  res++;
 	  int res3 = i - j;
	  if (res>res1)
	    res3=res2;
	  else
	    res3=res;
	  res3++;
	  //out.write(true);
		}
};

/*void Component::essaiwrite()
{
	;
	}*/

int sc_main (int argc , char *argv[]) 
{
/*#ifdef KASCPAR
 	sc_signal<bool> s1;
 	sc_signal<bool> s2;
 	sc_signal<bool> s3;
#else*/
	sc_core::sc_signal<bool> s1("s1"), s2("s2");//, s3("s3");
//#endif

 	Component C1("C1");
 	Component C2("C2");
// 	Component C3("C3");
       
// 	//	superclass* C;
// 	//	C = dynamic_cast<superclass*>(&C3);
// 	//	superclass* S = C;
// 	//	Component* res = dynamic_cast<Component*>(S);

 	C1.out(s1);
 	C2.out(s2);
// 	//	(*res).out(s3);
// 	C3.out(s3);

 	C1.in(s2);
 	C2.in(s1);
// 	C3.in(s2);

// 	C1.isHead = true;

	//Run the Simulation for "200 nanosecnds"
	sc_start(200, SC_NS);

	return 0;
}
