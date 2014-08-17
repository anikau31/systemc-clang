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


#include <iostream>
#include <iomanip>
#include <stdint.h>

#include "LoggerSC.h"


// ----------------------------------------------------------------------------
//! Custom constructor for the OSCI TLM 2.0 logger

//! Registers the blocking callback procedure, ::loggerReadWrite with the
//! target socket.

//! @param name  The SystemC module name, passed to the parent constructor.
// ----------------------------------------------------------------------------
LoggerSC::LoggerSC( sc_core::sc_module_name  name ) :
  sc_module( name )
{
  loggerSocket.register_b_transport( this, &LoggerSC::loggerReadWrite );

}	// Or1ksimSC()


// ----------------------------------------------------------------------------
//! Blocking transport routine the target socket

//! Receives payloads for any transport on this socket. Assumes all payloads
//! are 32 bit only (this is intended for use with the Or1ksimSC class).

//! Break out the command, address, data (for write only) and mask and report
//! them.

//! Set a successful response (tlm::TLM_OK_RESPONSE) to the call.

//! @param payload  The generic TLM payload
//! @param delay    How far the initiator is beyond baseline SystemC time.
//!                 Ignored here.
// ----------------------------------------------------------------------------
void
LoggerSC::loggerReadWrite( tlm::tlm_generic_payload &payload,
			   sc_core::sc_time         &delay )
{
  // Break out the address, mask and data pointer.
  tlm::tlm_command   comm    = payload.get_command();
  sc_dt::uint64      addr    = payload.get_address();
  unsigned char     *maskPtr = payload.get_byte_enable_ptr();
  unsigned char     *dataPtr = payload.get_data_ptr();

  // Record the payload fields (data only if it's a write)
  const char *commStr;

  switch( comm ) {
  case tlm::TLM_READ_COMMAND:   commStr = "Read";            break;
  case tlm::TLM_WRITE_COMMAND:  commStr = "Write";           break;
  case tlm::TLM_IGNORE_COMMAND: commStr = "Ignore";          break;
  default:                      commStr = "Unknown command"; break;
  }

  std::cout << "Logging" << std::endl;
  std::cout << "  Command:      "   << commStr << std::endl;
  std::cout << "  Address:      0x" << std::setw( 8 ) << std::setfill( '0' )
	    <<std::hex << (uint64_t)addr << std::endl;
  std::cout << "  Byte enables: 0x" << std::setw( 8 ) << std::setfill( '0' )
	    <<std::hex << *((uint32_t *)maskPtr) << std::endl;

  if( tlm::TLM_WRITE_COMMAND == comm ) {
    std::cout << "  Data:         0x" << std::setw( 8 ) << std::setfill( '0' )
	      <<std::hex << *((uint32_t *)dataPtr) << std::endl;
  }

  std::cout << std::endl;

  payload.set_response_status( tlm::TLM_OK_RESPONSE );  // Always OK

}	// loggerReadWrite()


// EOF
