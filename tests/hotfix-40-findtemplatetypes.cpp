#include "catch.hpp"

#include "clang/AST/ASTImporter.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Parse/ParseAST.h"
#include "clang/Tooling/Tooling.h"

#include "PluginAction.h"
#include "SystemCClang.h"

/// This is automatically generated from cmake.
#include "ClangArgs.h"
#include "Testing.h"

#include <iostream>

using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;
using namespace scpar;

TEST_CASE("Basic parsing checks", "[parsing]") {
  std::string code = R"(
#include "systemc.h"
//#include "sreg.h"
//#include "sc_stream.h"

template< int E, int F>
struct fp_t {
	static constexpr int ebits = E;
	static constexpr int fbits = F;
	static constexpr int bits = 1+E+F;

	// exponent bias
	// When E (bits) =  8, ebias =  127
	// When E (bits) = 11, ebias = 1023
	static constexpr int ebias = (1 << (E-1))-1;


	typedef sc_int <bits> si_t;
	typedef sc_uint<bits> ui_t;


	typedef sc_uint<F> frac_t;
	typedef sc_uint<E> expo_t;
	typedef sc_uint<1> sign_t;

	frac_t frac;
	expo_t expo; // biased by ebias
	sign_t sign;

	fp_t(ui_t ui = 0)
	{
		(sign,expo,frac) = ui;
	}

	fp_t& operator=(ui_t ui)
	{
		(sign,expo,frac) = ui;
		return *this;
	}

	operator ui_t() const
	{
		return (sign,expo,frac);
	}

	bool operator==(const fp_t& fp)
	{
		return
			this->frac == fp.frac &&
			this->expo == fp.expo &&
			this->sign == fp.sign;
	}

};
  
template<int E, int F>
inline std::ostream& operator<<(std::ostream& os, const fp_t<E,F>& fp)
{
	return os << hex << fp.sign << ':'  << fp.expo << ':' << fp.frac;
}

template<int E, int F>
void sc_trace(sc_trace_file* tf, const fp_t<E,F>& ob, const std::string& nm)
{
	sc_trace(tf, ob.frac, nm+".frac");
	sc_trace(tf, ob.expo, nm+".expo");
	sc_trace(tf, ob.sign, nm+".sign");
}



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
      os << v.info << std::boolalpha << v.flag ; 
  return os;
    }
};

// https://www.doulos.com/knowhow/systemc/faq/

template <typename T>
SC_MODULE(ram) {

  sc_signal<T> buggy_signal;

  void ram_proc(){};

  SC_HAS_PROCESS(ram);

  ram(sc_module_name name_ = "AA"):
    sc_module(name_) {
    SC_THREAD(ram_proc);

  }
};


SC_MODULE( test ){

	typedef sc_uint<16> expo_t;

  ram<fp_t<11,3>> specialized_template;
  
  // input ports
  sc_uint<32> uint;
  sc_in_clk clk;
  sc_in<bool> bool_clk;
	sc_signal<expo_t> emax;

	//sc_stream_in<int> s_port;
	//sc_stream_out<double> m_port;

  sc_in<MyType> in_mytype;
  sc_out<MyType> out_mytype;

  void entry_function_1() {
    while(true) {
    }
  }
  SC_CTOR( test ) {
    SC_METHOD(entry_function_1);
    sensitive << clk.pos();
  }
};


int sc_main(int argc, char *argv[]) {
  test test_instance("testing");

  return 0;
}
     )";

  cout << "TEST: " << systemc_clang::test_data_dir << "\n";
  INFO(systemc_clang::test_data_dir);
  auto catch_test_args = systemc_clang::catch_test_args;
  catch_test_args.push_back("-I" + systemc_clang::test_data_dir +
                            "/llnl-examples/");

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, catch_test_args).release();
  // ASTUnit *from_ast =
  //   tooling::buildASTFromCodeWithArgs(code, systemc_clang::catch_test_args)
  //      .release();

  SystemCConsumer sc{from_ast};
  sc.HandleTranslationUnit(from_ast->getASTContext());

  auto model{sc.getSystemCModel()};

  // This provides the module declarations.
  auto module_decl{model->getModuleDecl()};
  auto module_instance_map{model->getModuleInstanceMap()};

  // Want to find an instance named "testing".

  ModuleDecl *test_module{model->getInstance("testing")};
  ;

  SECTION("Found sc_module instances", "[instances]") {
    // There should be 2 modules identified.
    INFO("Checking number of sc_module declarations found: "
         << module_decl.size());

    REQUIRE(module_decl.size() == 1);

    REQUIRE(test_module != nullptr);

    INFO("Checking clock port parsing.");
    // These checks should be performed on the declarations.

    ModuleDecl *test_module_inst{test_module};

    // Check if the proper number of ports are found.
    //
    // There is only one input port seen as sc_in<bool> clk;
    auto input_ports{test_module_inst->getIPorts()};
    REQUIRE(input_ports.size() == 3);

    // Try to access each of the ports
    // // Iterate over all ports and their arguments.
    for (const auto &port : input_ports) {
      auto name = get<0>(port);
      PortDecl *pd = get<1>(port);
      llvm::outs() << "name: " << name << "\n";
      auto template_type = pd->getTemplateType();
      auto template_args{template_type->getTemplateArgumentsType()};

      if (name == "uint") {
        REQUIRE((template_args[0].getTypeName() == "sc_uint"));
        REQUIRE((template_args[1].getTypeName() == "32"));
      }

      if (name == "bool_clk") {
        REQUIRE((template_args[0].getTypeName() == "sc_in"));
        REQUIRE((template_args[1].getTypeName() == "_Bool"));
      }

      if (name == "clk") {
        REQUIRE((template_args[0].getTypeName() == "sc_in"));
        REQUIRE((template_args[1].getTypeName() == "_Bool"));
      }

      if (name == "s_port") {
        REQUIRE((template_args[0].getTypeName() == "sc_stream_in"));
        REQUIRE((template_args[1].getTypeName() == "int"));
      }

      if (name == "m_port") {
        REQUIRE((template_args[0].getTypeName() == "sc_stream_out"));
        REQUIRE((template_args[1].getTypeName() == "double"));
      }

      if (name == "in_mytype") {
        REQUIRE((template_args[0].getTypeName() == "sc_in"));
        REQUIRE((template_args[1].getTypeName() == "MyType"));
      }

      if (name == "out_mytype") {
        REQUIRE((template_args[0].getTypeName() == "sc_out"));
        REQUIRE((template_args[1].getTypeName() == "MyType"));
      }
    }

    REQUIRE(test_module_inst->getOPorts().size() == 1);
    REQUIRE(test_module_inst->getIOPorts().size() == 0);
    REQUIRE(test_module_inst->getSignals().size() == 1);
    REQUIRE(test_module_inst->getOtherVars().size() == 1);
    REQUIRE(test_module_inst->getInputStreamPorts().size() == 1);
    REQUIRE(test_module_inst->getOutputStreamPorts().size() == 1);
  }
}
