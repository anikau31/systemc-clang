#include <systemc.h>

static bool isThisAGlobalVar;

SC_MODULE(mytop)
{
public:
	int res;
	sc_event e;
//	SC_HAS_PROCESS(mytop);

	SC_CTOR(mytop) {
		SC_THREAD(myFctP);
		sensitive << e;
		SC_THREAD(myFctQ);
	}
  
	void myFctP() {
		//cout <<"mfuncp" << endl;
		wait(e);
    
		//    cout <<"p" << endl;
		wait(10,SC_NS);
		if (isThisAGlobalVar)
			res = 0;
		//      cout << "mfunc Ok" << endl;
		else
			//      cout << "mfunc Ko" << endl;
			res = 1;
		//    cout <<"mfunc end p" << endl;
	}
  
	void myFctQ() {
		int moniterateur;
		//    cout <<"mfunc q" << endl;
		e.notify();
		isThisAGlobalVar = false;
		wait(10,SC_NS);
		isThisAGlobalVar = true;
		//    cout <<"mfunc end q" << endl;

		for (moniterateur = 0; moniterateur < 10; moniterateur++) {
			isThisAGlobalVar = !isThisAGlobalVar;
		}
	}
};
 

int sc_main( int argc, char* argv[] )
{
	int i, j;
	mytop MYTOP("MYTOP");
//	mytop OTHERTOP("OTHERTOP");

	::std::cout << "Entering sc_main...\n";
//   MYTOP.myFctP();
//   MYTOP.myFctQ();
	//    i = 5;
	sc_start(-1);
	//    j = handleI(i);
}


// int handleI(int i)
// {
//   char* mymsg = "handling i";
//   int myres = i + 2;

//   return myres;
// }

// void execute()
// {
//   int i, j;
// //   top TOP("TOP");
// //   sc_start(-1);
//   i = 5;
  
//   j = handleI(i);
// }
