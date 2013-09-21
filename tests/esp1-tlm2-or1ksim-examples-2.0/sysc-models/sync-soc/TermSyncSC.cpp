// Terminal with synchronized timing module implementation

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

#include "TermSyncSC.h"


// ----------------------------------------------------------------------------
//! Custom constructor for the terminal module

//! Passes the name to the parent constructor.

//! Sets up threads listening to the rx port from the UART
//! (TermSyncSC::rxThread) and waiting for characters from the xterm
//! (TermSyncSC::xtermThread).

//! Calculates the character delay appropriate for the baud rate. Fixed
//! behavior of 1 start bit, 8 data bits and and 1 stop bit.

//! Initializes the xterm in a separate process, setting up a file descriptor
//! that can read and write from/to it.

//! @param name      Name of this module - passed to the parent constructor
//! @param baudRate  The baud rate of the terminal connection to the UART
// ----------------------------------------------------------------------------
TermSyncSC::TermSyncSC( sc_core::sc_module_name  name,
			unsigned long int        baudRate ) :
  TermSC( name )  
{
  // Calculate the delay. No configurability here - 1 start, 8 data and 1 stop
  // = total 10 bits.
  charDelay = sc_core::sc_time( 10.0 / (double)baudRate, sc_core::SC_SEC );

}	/* TermSyncSC() */


// ----------------------------------------------------------------------------
//! Thread listening for characters from the xterm with synchronization

//! Wait to be notified via the SystemC event TermSC::ioEvent that there
//! is a character available.

//! Read the character, wait to model the delay on the wire, then send it out
//! to the UART
// ----------------------------------------------------------------------------
void
TermSyncSC::xtermThread()
{
  while( true ) {
    wait( *ioEvent );			// Wait for some I/O on the terminal

    int ch = xtermRead();		// Should not block

    wait( charDelay );			// Model baud rate delay
    tx.write( (unsigned char)ch );	// Send it
  }
}	// xtermThread()


// EOF
