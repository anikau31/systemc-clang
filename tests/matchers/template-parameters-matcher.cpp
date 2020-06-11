#include "SystemCClang.h"
#include "catch.hpp"
#include "clang/Tooling/Tooling.h"

// This is automatically generated from cmake.
#include "ClangArgs.h"
#include "Testing.h"

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchersInternal.h"
#include "clang/ASTMatchers/ASTMatchersMacros.h"

using namespace clang::ast_matchers;
using namespace scpar;
using namespace sc_ast_matchers;

class TemplateParametersMatcher : public MatchFinder::MatchCallback {
 public:
  void registerMatchers(MatchFinder &finder) {
    // Overview of the matcher
    // Terminology: template parameter refers to the template typename name, and
    // template arguments refers to the substituted type. For example, template
    // <typename T> X{}; X<int> x; has the parameter as T and the argument as
    // int.
    //
    // 1. Start matching within a ClassTemplateDecl. Note that the match() call
    // has to be updated to ensure that the appropriate ClassTemplateDecl is
    // provided.
    // 2. The isExpansionInMainFile() makes the matcher focus on the provided
    // file (input), and not the underlying C++ files.
    //
    // 3. We check if the ClassTemplateDecl has a CXXRecordDecl within it (note
    // that it will have a CXXRecordDecl and a ClassTemplateSpecializationDecl,
    // but we want the former to extract the template parameters).
    // 4. For every fieldDecl that is a descendant, it is going to check if the
    // type for the FieldDecl's desugared type is a TemplateSpecialization or a
    // TemplateTypeParm.  This should cover typedefs as well. An alternative way
    // would be to just match with Type, and then in the callback one could cast
    // it to respective types that one cares about.
    //
    /* clang-format off */
    auto match_template_decl =
        classTemplateDecl(
            isExpansionInMainFile(),
            has(cxxRecordDecl(forEachDescendant(
                fieldDecl(anyOf(
                    hasType(hasUnqualifiedDesugaredType(
                        templateSpecializationType().bind("template_special")
                        )//hasUnqualifiedDesugaredType
                      ) //hasType
                    , hasType(hasUnqualifiedDesugaredType( 
                        templateTypeParmType().bind( "parm_type") 
                        ) //hasUnqualifiedDesugaredType
                      )// hasType
                  )  // anyOf
                ).bind("fd"))
              )
            )
          ).bind("template_decl");
    /* clang-format on */

    finder.addMatcher(match_template_decl, this);
  }

  virtual void run(const MatchFinder::MatchResult &result) {
    auto decl{result.Nodes.getNodeAs<Decl>("decl")};
    auto template_decl{
        result.Nodes.getNodeAs<ClassTemplateDecl>("template_decl")};
    auto fd{result.Nodes.getNodeAs<FieldDecl>("fd")};
    auto typedef_type{result.Nodes.getNodeAs<TypedefType>("type_deftype")};
    auto parm_type{result.Nodes.getNodeAs<TemplateTypeParmType>("parm_type")};
    auto template_special{
        result.Nodes.getNodeAs<TemplateSpecializationType>("template_special")};
    llvm::outs() << "=============== TEST Template Parm Matcher ====== \n";
    
    if (template_decl) {
    template_decl->dump();
    }
    if (template_special && fd) {
      llvm::outs() << "########  TEmplateSpecial\n";
      fd->dump();
      template_special->dump();
      const TemplateArgument &targ{template_special->getArg(0)};
      //    llvm::outs() << "## get the 0 arg: " << targ.getKind() << "\n";
      //     targ.dump(llvm::outs());

      switch (targ.getKind()) {
        case TemplateArgument::ArgKind::Integral: {
          auto q{targ.getAsIntegral()};
          llvm::outs() << "@@ Integral: " << q << "\n";
        }; break;
        case TemplateArgument::ArgKind::Type: {
          auto q{targ.getAsType()};
          auto name{q.getAsString()};
          llvm::outs() << "@@ arg: " << name << "\n";
        }; break;
        case TemplateArgument::ArgKind::Expression: {
          Expr *expr{targ.getAsExpr()};
          llvm::outs() << "\n@@ Expression arg: ";
          DeclRefExpr *dexpr{dyn_cast<DeclRefExpr>(expr)};
          //       expr->dump();
          if (dexpr) {
            //        dexpr->dump();
            llvm::outs() << "NameInfor: " << dexpr->getNameInfo().getAsString()
                         << "\n";
          }
        }
        default: {
        }
      };
    }
  }
  void dump() {}
};

// This test works
TEST_CASE("Read SystemC model from file for testing", "[parsing]") {
  std::string code = R"(
#include "systemc.h"

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


  template <int T,typename U> 
  class TemplateTest {
  public:
  sc_uint<T> t;
  U u;
  fp_t<11,23> fpt;


  };

int main() {
TemplateTest<32, double> tt{};

return 0;
}
     )";

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, systemc_clang::catch_test_args)
          .release();

  TemplateParametersMatcher template_matcher{};
  MatchFinder matchRegistry{};
  template_matcher.registerMatchers(matchRegistry);
  // Run all the matchers
  // This next call will not work on the subtree only.
  matchRegistry.matchAST(from_ast->getASTContext());
  // To run just on a subtree.
  // 1. ct_decl is the ClassTemplateDecl
  // 2. result.Context (is the ASTContext).
  // Examples for the invocation is available in InstanceMatcher.
  //
  // matchRegistry.match(*ct_decl, *result.Context);
  //
  template_matcher.dump();

  REQUIRE(true);
}
