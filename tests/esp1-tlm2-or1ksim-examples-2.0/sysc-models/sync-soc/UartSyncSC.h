// UART with synchronized timing module definition

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

#ifndef UART_SYNC_SC__H
#define UART_SYNC_SC__H

#include "UartSC.h"

#define UART_READ_NS       60	//!< Time to access the UART for read
#define UART_WRITE_NS      60	//!< Time to access the UART for write


// ----------------------------------------------------------------------------
//! SystemC module class for a 16450 UART with synchronized timing

//! Derived from the UartSC:: class. Updates the UartSC::busThread() thread and
//! UartSC::busReadWrite() blocking TLM callback to provide synchronous
//! timing.

//! The UartSC::busWrite() function is extended to update the baud rate timing
//! based on changes to the UART registers.
// ----------------------------------------------------------------------------
class UartSyncSC
  : public UartSC
{
public:

  UartSyncSC (sc_core::sc_module_name  name,
	      unsigned long int        _clockRate);


protected:

  // Reimplemetned blocking transport function, which adds timing, further
  // reimplmented in derived classes.
  virtual void  busReadWrite( tlm::tlm_generic_payload &payload,
			      sc_core::sc_time         &delay );

  // New version of the busWrite function, to update the character delay if
  // relevant registers are update will not be reimplemented further.
  void          busWrite( unsigned char  uaddr,
			  unsigned char  wdata );


private:

  // Reimplemented bus thread, which adds a timing delay before passing on hte
  // character read. Not reimplemented again.
  void          busThread();

  // Function to work out the baud rate character delay. Won't be reused in
  // derived classes.
  void  resetCharDelay();

  // Additional status info for this class only

  unsigned long int   clockRate;	//!< External clock into UART
  sc_core::sc_time    charDelay;	//!< Total time to Tx a char

};	// UartSyncSC()


#endif	// UART_SYNC_SC__H

// EOF
