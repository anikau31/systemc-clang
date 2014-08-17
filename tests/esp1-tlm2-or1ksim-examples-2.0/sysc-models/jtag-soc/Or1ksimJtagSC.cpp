// Wrapper for Or1ksim with interrupts and JTAG module implementation

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

#include "Or1ksimJtagSC.h"


using sc_core::SC_SEC;
using sc_core::sc_module_name;
using sc_core::sc_time;

using std::cerr;
using std::endl;


// ----------------------------------------------------------------------------
//! Custom constructor for the Or1ksimJtagSC SystemC module

//! Although there is a new thread of control to handle JTAG, it is not
//! declared here. Since we are (for JTAG) a TLM target, it will be the thread
//! of the initiator socket.

//! The JTAG interface is implemented as an 8-bit wide TLM target port with an
//! extension to specify the number of bits.

//! We must initialize the Mutex to be unlocked.

//! @param name           SystemC module name
//! @param configFile     Config file for the underlying ISS
//! @param imageFile      Binary image to run on the ISS
// ----------------------------------------------------------------------------
Or1ksimJtagSC::Or1ksimJtagSC (sc_module_name  name,
			      const char     *configFile,
			      const char     *imageFile) :
  Or1ksimIntrSC (name, configFile, imageFile)
{
  // Bind the handler to the JTAG target port.
  jtag.register_b_transport( this, &Or1ksimJtagSC::jtagHandler );

  // Unlock the Mutex
  or1ksimMutex.unlock ();

}	/* Or1ksimJtagSC() */


// ----------------------------------------------------------------------------
//! The SystemC thread running the underlying ISS

//! This version uses temporal decoupling and is based on the version in the
//! base class, Or1ksimDecoupSC::run ().

//! This version adds a mutex to lock access to the underlying ISS. The JTAG
//! thread may not simultaneously access this.
// ----------------------------------------------------------------------------
void
Or1ksimJtagSC::run ()
{
  while( true ) {
    // Compute how much time left in this quantum
    sc_time  timeLeft =
      tgq->compute_local_quantum () - issQk.get_local_time ();

    // Reset the ISS time point.
    or1ksim_set_time_point ();

    // Run for (up to) the remainder of this quantum. Surround the call by a
    // mutex lock.
    or1ksimMutex.lock ();
    (void)or1ksim_run (timeLeft.to_seconds ());
    or1ksimMutex.unlock ();

    // Increment local time by the amount consumed.
    issQk.inc (sc_time (or1ksim_get_time_period(), SC_SEC));

    // If we halted before the end of the ISS period, that is presumably
    // because we have stalled. In which case we should immediately sync to
    // allow a potential JTAG thread to run. Otherwise we should only sync if
    // we have reached the end of the quantum. Except that must be the
    // case. So we don't need to call need_sync (), we can just sync here.
    issQk.sync ();
  }

}	// Or1ksimSC ()


//-----------------------------------------------------------------------------
//! Handler for a new JTAG transaction

//! The extension is ignorable, for greatest interoperability. If it is not
//! present, then the data length multiplied by 8 is used as the bit length
//! and the address is used to indicate the transaction required as follows:
//! - 0:               Shift through IR
//! - 1:               Shift through DR
//! - any other value: Reset

//! @param[in,out] payload  The generic payload (with mandatory JTAG extensions)
//! @param[in,out] delay    The incoming delay and on return the total delay
//!                         for this thread.
//-----------------------------------------------------------------------------
void
Or1ksimJtagSC::jtagHandler( tlm::tlm_generic_payload &payload,
			    sc_core::sc_time         &delay )
{
  // Retrieve the extension.
  JtagExtensionSC *ext;
  payload.get_extension (ext);

  // Check if the extension exists. Set up the access type and bit size as
  // appropriate.
  JtagExtensionSC::AccessType  type;
  int                          bitSize;

  if (NULL == ext)
    {
      unsigned int  addr = (unsigned int) payload.get_address ();

      type    = (ADDR_SHIFT_IR == addr) ? JtagExtensionSC::SHIFT_IR :
	        (ADDR_SHIFT_DR == addr) ? JtagExtensionSC::SHIFT_DR :
                                          JtagExtensionSC::RESET ;
      bitSize = 8 * (int) payload.get_data_length ();
    }
  else
    {
      type     = ext->getType ();
      bitSize = ext->getBitSize ();
    }

  // Behavior depends on the type
  switch (type)
    {
    case JtagExtensionSC::RESET:
      or1ksimMutex.lock ();
      delay += sc_time (or1ksim_jtag_reset (), SC_SEC);
      or1ksimMutex.unlock ();
      payload.set_response_status (tlm::TLM_OK_RESPONSE);
      return;

    case JtagExtensionSC::SHIFT_IR:
      or1ksimMutex.lock ();
      delay += sc_time (or1ksim_jtag_shift_ir (payload.get_data_ptr (),
					       bitSize), SC_SEC);
      or1ksimMutex.unlock ();
      payload.set_response_status (tlm::TLM_OK_RESPONSE);
      return;

    case JtagExtensionSC::SHIFT_DR:
      or1ksimMutex.lock ();
      delay += sc_time (or1ksim_jtag_shift_dr (payload.get_data_ptr (),
					       bitSize), SC_SEC);
      or1ksimMutex.unlock ();
      payload.set_response_status (tlm::TLM_OK_RESPONSE);
      return;

    default:
      cerr << "ERROR: Unrecognized JTAG transaction type." << endl;
      payload.set_response_status (tlm::TLM_GENERIC_ERROR_RESPONSE);
      return;
    }
}	/* jtagHandler () */
