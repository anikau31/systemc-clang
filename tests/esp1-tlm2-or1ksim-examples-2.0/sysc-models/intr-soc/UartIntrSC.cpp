// UART module with interrupt support implementation

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

#include "UartIntrSC.h"


SC_HAS_PROCESS( UartIntrSC );


// ----------------------------------------------------------------------------
//! Custom constructor for the UART module

//! Call the parent constructor and mark the size of interrupt FIFO, then set
//! up the thread handling interrupt generation.

//! @param name             The SystemC module name, passed to the parent
//!                         constructor
//! @param _clockRate       The external clock rate, passed to the parent
//!                         constructor
// ----------------------------------------------------------------------------
UartIntrSC::UartIntrSC (sc_core::sc_module_name  name,
			unsigned long int        _clockRate) :
  UartDecoupSC (name, _clockRate),
  intrQueue (1)
{
  SC_THREAD( intrThread );

}	/* UartIntrSC() */


// ----------------------------------------------------------------------------
//! SystemC thread driving the interrupt signal

//! If true is read, assert the interrupt, otherwise deassert. Needed since a
//! single process must drive a signal.
// ----------------------------------------------------------------------------
void
UartIntrSC::intrThread()
{
  intr.write( false );		// Clear interrupt on startup

  while( true ) {
    intr.write( intrQueue.read() );
  }
 }	// intrThread()


// ----------------------------------------------------------------------------
//! Generate an interrupt

//! Updated version from the base class, UartSC::genIntr(), which triggers a
//! write to assert the interrupt signal port if required.

//! @param ierFlag  Indicator of which interrupt is to be cleared (as IER bit).
// ----------------------------------------------------------------------------
void
UartIntrSC::genIntr( unsigned char  ierFlag )
{
  if( isSet( regs.ier, ierFlag )) {
    set( intrPending, ierFlag );	// Mark this interrupt as pending.
    setIntrFlags();			// Show highest priority
    intrQueue.write( true );		// Request an interrupt signal
  }
}	// genIntr()


// ----------------------------------------------------------------------------
//! Clear an interrupt

//! Updated version from the base class, UartSC::clrIntr(), which triggers a
//! write to deassert the interrupt signal port if required.

//! @param ierFlag  Indicator of which interrupt is to be cleared (as IER bit).
// ----------------------------------------------------------------------------
void
UartIntrSC::clrIntr( unsigned char ierFlag )
{
  clr( intrPending, ierFlag );
  setIntrFlags();

  if( isSet( regs.iir, UART_IIR_IPEND )) {	// 1 = not pending
    intrQueue.write( false );			// Deassert if none left
  }
}	// clrIntr()


// EOF
