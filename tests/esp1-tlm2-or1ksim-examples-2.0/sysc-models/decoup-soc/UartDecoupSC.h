// UART module with decoupled timing module definition

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

#ifndef UART_DECOUP_SC__H
#define UART_DECOUP_SC__H

#include "UartSyncSC.h"


// ----------------------------------------------------------------------------
//! SystemC module class for a 16450 UART with temporal decoupling

//! Adds support for temporal decoupling to UartSyncSC::. Only the blocking
//! TLM callack, UartSyncSC::busReadWrite() is replaced, to support temporal
//! decoupling in the intitiator (Or1ksimDecoupSC::). The thread in this class
//! is not temporally decoupled, since it is not a TLM initiator.
// ----------------------------------------------------------------------------
class UartDecoupSC
  : public UartSyncSC
{
public:

  UartDecoupSC (sc_core::sc_module_name  name,
		unsigned long int        _clockRate);


private:

  // Reimplemented blocking transport function, which adds decoupled timing
  // delay. Will not be reimplemented further.
  virtual void  busReadWrite( tlm::tlm_generic_payload &payload,
			      sc_core::sc_time         &delay );

};	// UartDecoupSC()


#endif	// UART_DECOUP_SC__H

// EOF
