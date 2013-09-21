// JTAG logger module : definition

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

#ifndef JTAG_LOGGER_SC__H
#define JTAG_LOGGER_SC__H

#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>

#include "JtagExtensionSC.h"


// ----------------------------------------------------------------------------
//! SystemC module class generating JTAG traffic to log information.

//! Resets the JTAG, sets the JTAG DEBUG instruction, then reads the NPC
//! Special Purpose Register.
// ----------------------------------------------------------------------------
class JtagLoggerSC
: public sc_core::sc_module
{
public:

  //! Initiator socket for JTAG transactions
  tlm_utils::simple_initiator_socket<JtagLoggerSC, 1>  jtag;

  // Constructor
  JtagLoggerSC (sc_core::sc_module_name  name);


private:

  //! Generic payload which we'll use for communicating with the target JTAG
  //! port.
  tlm::tlm_generic_payload  payload;

  //! Payload extension for JTAG with default initialization
  JtagExtensionSC  ext;

  // Thread which will generate the JTAG transactions.
  virtual void  runJtag();

  // Support methods to handle different types of JTAG instructions recognized
  // by Or1ksim.
  void  jtagReset (sc_core::sc_time &delay);

  void  jtagInstruction (unsigned char     inst,
			 sc_core::sc_time &delay);

  void  jtagSelectModule (unsigned char     moduleId,
			  sc_core::sc_time &delay);

  void  jtagWriteCommand (unsigned char      accessType,
			  unsigned long int  addr,
			  unsigned long int  numBytes,
			  sc_core::sc_time  &delay);

  void  jtagGoCommandRead (unsigned char      data[],
			   unsigned long int  dataBytes,
			   sc_core::sc_time  &delay);

  // Utilities
  unsigned long int   crc32 (unsigned long long int  value,
			     int                     num_bits,
			     unsigned long int       crc_in);

  unsigned long long  reverseBits (unsigned long long  val,
				   int                 len);

};	/* JtagLoggerSC() */


#endif	// JTAG_LOGGER_SC__H

// EOF
