// Wrapper for Or1ksim with interrupts and JTAG module header

// Copyright (C) 2008, 2010 Embecosm Limited <info@embecosm.com>

// Contributor Jeremy Bennett <jeremy.bennett@embecosm.com>

// This file is part of the example programs for "Building a Loosely Timed SoC
// Model with OSCI TLM 2.0"

// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 3 of the License, or (at your option)
// any later version.

// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.

// You should have received a copy of the GNU General Public License along
// with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// This code is commented throughout for use with Doxygen.
// ----------------------------------------------------------------------------

#ifndef OR1KSIM_JTAG_SC__H
#define OR1KSIM_JTAG_SC__H

#include <tlm.h>
#include <tlm_utils/simple_target_socket.h>

#include "Or1ksimIntrSC.h"
#include "JtagExtensionSC.h"


// ----------------------------------------------------------------------------
//! SystemC module class wrapping Or1ksim ISS with temporal decoupling and
//! external interrupts.

//! Provides signals for the interrupts and additional threads sensitive to
//! the interrupt inputs. All other functionality comes from the base class,
//! Or1ksimDecoupSC::.
// ----------------------------------------------------------------------------
class Or1ksimJtagSC
  : public Or1ksimIntrSC
{
public:

  //! Target socket for JTAG transactions
  tlm_utils::simple_target_socket<Or1ksimJtagSC, 1>  jtag;

  // Constructor
  Or1ksimJtagSC( sc_core::sc_module_name  name,
		 const char              *configFile,
		 const char              *imageFile );


protected:

  // Thread which will run the model, which adds a mutex to prevent calling
  // the underlying ISS when the JTAG thread is active.
  virtual void  run();


private:

  static const unsigned int  ADDR_SHIFT_IR = 0;
  static const unsigned int  ADDR_SHIFT_DR = 1;

  //! SystemC mutex controlling access to the underlying ISS.
  sc_core::sc_mutex  or1ksimMutex;


  // The blocking transport routine for the JTAG port.
  void  jtagHandler( tlm::tlm_generic_payload &payload,
		     sc_core::sc_time         &delay );


};	/* Or1ksimJtagSC() */


#endif	// OR1KSIM_JTAG_SC__H

// EOF
