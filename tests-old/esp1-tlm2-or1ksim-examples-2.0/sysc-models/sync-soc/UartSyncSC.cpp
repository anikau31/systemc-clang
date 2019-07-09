// UART with synchronized timing module implementation

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
#include <stdio.h>

#include "UartSyncSC.h"


// ----------------------------------------------------------------------------
//! Custom constructor for the UART module with timing synchronization

//! Calls the base class constructor to set up the module and records the
//! clock rate in the local state variable.

//! @param name             The SystemC module name, passed to the base class
//!                         constructor
//! @param _clockRate       The external clock rate
// ----------------------------------------------------------------------------
UartSyncSC::UartSyncSC( sc_core::sc_module_name  name,
			unsigned long int        _clockRate ) :
  UartSC (name),
  clockRate (_clockRate) 
{
}	/* UartSyncSC() */


// ----------------------------------------------------------------------------
//! SystemC thread listening for transmit traffic on the bus with timing

//! Sits in a loop. Initially sets the line status register to indicate the
//! buffer is empty (reset will clear these bits) and sends an interrupt (if
//! enabled) to indicate the buffer is empty.

//! Then waits for the UartSyncSC::txReceived event to be triggered (this
//! happens when new data is written into the transmit buffer by a bus write).

//! On receipt of a character, waits for UartSyncSC::charDelay to simulate the
//! time taken to put the bits on the wire (this represents an extension from
//! the functionality in the case class function, UartSC::busThread()). Then
//! writes the char onto the Tx FIFO.
// ----------------------------------------------------------------------------
void
UartSyncSC::busThread()
{
  // Loop listening for changes on the Tx buffer, waiting for a baud rate
  // delay then sending to the terminal
  while( true ) {
    set( regs.lsr, UART_LSR_THRE );	// Indicate buffer empty
    set( regs.lsr, UART_LSR_TEMT );
    genIntr( UART_IER_TBEI );		// Interrupt if enabled

    wait( txReceived );			// Wait for a Tx request
    wait( charDelay );			// Wait baud delay
    tx.write( regs.thr );		// Send char to terminal
  }
}	// busThread()


// ----------------------------------------------------------------------------
//! TLM2.0 blocking transport routine for the UART bus socket with timing

//! Reuses the base class function, UartSC::busReadWrite(), but synchronizes
//! to allow for the relevant read/write delay, which should make the delay 0.

//! @param payload  The transaction payload
//! @param delay    How far the initiator is beyond baseline SystemC time.
// ----------------------------------------------------------------------------
void
UartSyncSC::busReadWrite( tlm::tlm_generic_payload &payload,
			  sc_core::sc_time         &delay )
{
  UartSC::busReadWrite( payload, delay );	// base method

  // Delay as appropriate.
  switch( payload.get_command() ) {
  case tlm::TLM_READ_COMMAND:
    wait( sc_core::sc_time( UART_READ_NS, sc_core::SC_NS ));
    delay = sc_core::SC_ZERO_TIME;
    break;
  case tlm::TLM_WRITE_COMMAND:
    wait( sc_core::sc_time( UART_WRITE_NS, sc_core::SC_NS ));
    delay = sc_core::SC_ZERO_TIME;
    break;
  case tlm::TLM_IGNORE_COMMAND:
    wait( sc_core::sc_time( UART_WRITE_NS, sc_core::SC_NS ));
    delay = sc_core::SC_ZERO_TIME;
    break;
  }

}	// busReadWrite()


// ----------------------------------------------------------------------------
//! Process a write on the UART bus with update to timing

//! Reuses the method in the base class, UartSC::busWrite(). Then calls the
//! character delay update method if appropriate.x

//! @param uaddr  The address of the register being accessed
//! @param wdata  The value to be written
// ----------------------------------------------------------------------------
void
UartSyncSC::busWrite( unsigned char  uaddr,
		      unsigned char  wdata )
{
  UartSC::busWrite( uaddr, wdata );		// base function

  // Lookup on the register to see if update to delay is needed
  switch( uaddr ) {
  case UART_BUF:		// Only change if divisorLatch update (DLAB=1)
  case UART_IER:
    if( isSet( regs.lcr, UART_LCR_DLAB ) ) {
      resetCharDelay();
    }
    break;

  case UART_LCR:
    resetCharDelay();		// Could change baud delay
    break;

  default:
    break;			// Do nothing
  }

}	// busWrite()


// ----------------------------------------------------------------------------
//! Recalculate the character delay

//! Recalculate Uart::charDelay after a change to the divisor latch. The
//! divisor latch is for a 16x clock. Allow enough time for start (1), stop
//! (1, 1.5 or 2), data (5-7) and parity (0-1) bits.
// ----------------------------------------------------------------------------
void
UartSyncSC::resetCharDelay()
{
  if( clockRate > 0 ) {		// Avoid divide by zero!
    int  dataBits  = (regs.lcr & UART_LCR_MASK) + 5;
    int  parityBit = isSet( regs.lcr, UART_LCR_PEN ) ? 1 : 0;
    int  stopBits  = isSet( regs.lcr, UART_LCR_STB ) ? 2 : 1;

    // 5 bit data has max 1.5 stop bits
    double totBits = (double)(1 + dataBits + parityBit + stopBits) -
                     (((5 == dataBits) && (1 == stopBits)) ? 0.5 : 0.0);
    double baudRate = (double)clockRate / (double)regs.dl / 16.0;

    charDelay = sc_core::sc_time( totBits / baudRate, sc_core::SC_SEC );
  }
}	// resetCharDelay()


// EOF
