#include "systemc.h"
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
template< unsigned D1, unsigned D2>
SC_MODULE(producer){
  int global;
  sc_out <int> out;
  sc_in<MyType > user_in;
  sc_out<MyType> user_out;

  void f1() {
  }

  SC_CTOR(producer) {
    SC_THREAD(f1);
  }
};

int sc_main(int argc, char *argv[]) {

  sc_signal<int> s1;

  producer<222,333> p1("producer_instance");
  p1.out(s1);
  return 0;
}
