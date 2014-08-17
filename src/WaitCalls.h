//===-- src/WaitCalls.h - systec-clang class definition -------*- C++ -*-===//
//
//                     systemc-clang: SystemC Parser
//
// This file is distributed under the University of Illinois License.
// See LICENSE.mkd for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Stores the data members found in a class.
///
//===----------------------------------------------------------------------===//
#ifndef _WAIT_CALLS_H_
#define _WAIT_CALLS_H_

#include "clang/AST/DeclCXX.h"
#include <string>
#include <map>
#include "Utility.h"
#include "FindWait.h"

namespace scpar {
	using namespace clang;
	using namespace std;

	class WaitCalls {
	public:

		// Constructors.
		WaitCalls ();
		WaitCalls (const string &, FindWait::waitListType);

		// Copy constructor.
		  WaitCalls (const WaitCalls &);

		// Destructor.
		 ~WaitCalls ();

		/// Get parameters
		string getName ();
		  FindWait::waitListType getWaitList ();


		unsigned int getTotalWaits ();
		// Print
		void dump (raw_ostream &, int);

	private:
		  string _name;
		  FindWait::waitListType _waitList;
	};
}
#endif
