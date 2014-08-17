// Logger module definition

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

#ifndef LOGGER_SC__H
#define LOGGER_SC__H

#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"


// ----------------------------------------------------------------------------
//! SystemC module class providing a simple TLM 2.0 logger

//! Provides a TLM 2.0 simple target convenience socket, and records the
//! command, address, data, mask and delay in any packets sent to that socket.

//! Assumes all transactions are 32 bits long - this is intended for use with
//! the Or1ksimSC class.
// ----------------------------------------------------------------------------
class LoggerSC
  : public sc_core::sc_module
{
public:

  //! The TLM socket to receive bus traffic.
  tlm_utils::simple_target_socket<LoggerSC>  loggerSocket;

  // Constructor
  LoggerSC( sc_core::sc_module_name  name );


private:

  // The blocking transport routine for the port
  void  loggerReadWrite( tlm::tlm_generic_payload &payload,
			 sc_core::sc_time         &delay );

};	// LoggerSC()


#endif	// LOGGER_SC__H

// EOF
