// UART base module implementation

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

#include "UartSC.h"


using sc_core::sc_time_stamp;

using std::cout;
using std::endl;


SC_HAS_PROCESS( UartSC );

// ----------------------------------------------------------------------------
//! Custom constructor for the UART module

//! Passes the name to the parent constructor. Sets the model endianism
//! (UartSC::isLittleEndian).

//! Sets up processes listening to the bus (UartSC::busThread()) and the Rx pin
//! (UartSC::rxMethod()).

//! Registers UartSC::busReadWrite() as the callback for blocking transport on
//! the UartSC::bus socket.

//! Zeros the registers, but leaves the UartSC::regs.dl unset - it is
//! undefined until used.

//! @param name             The SystemC module name, passed to the parent
//!                         constructor
// ----------------------------------------------------------------------------
UartSC::UartSC( sc_core::sc_module_name  name) :
  sc_module( name ),
  intrPending( 0 )
{
  // Set up the thread for the bus side
  SC_THREAD( busThread );

  // Set up the method for the terminal side (statically sensitive to Rx)
  SC_METHOD( rxMethod );
  sensitive << rx;
  dont_initialize();

  // Register the blocking transport method
  bus.register_b_transport( this, &UartSC::busReadWrite );

  // Clear UART regs
  bzero( (void *)&regs, sizeof( regs ));

}	/* UartSC() */


// ----------------------------------------------------------------------------
//! SystemC thread listening for transmit traffic on the bus

//! Sits in a loop. Initially sets the line status register to indicate the
//! buffer is empty (reset will clear these bits) and sends an interrupt (if
//! enabled) to indicate the buffer is empty.

//! Then waits for the UartSC::txReceived event to be triggered (this happens
//! when new data is written into the transmit buffer by a bus write).

//! @note This could be a SC_METHOD in this implementation, but when we add
//!       timing, we will want to use wait (), so it must then be a SC_THREAD.

//! On receipt of a character writes the char onto the Tx FIFO.
// ----------------------------------------------------------------------------
void
UartSC::busThread()
{
  // Loop listening for changes on the Tx buffer, waiting for a baud rate
  // delay then sending to the terminal
  while( true ) {
    set( regs.lsr, UART_LSR_THRE );	// Indicate buffer empty
    set( regs.lsr, UART_LSR_TEMT );
    genIntr( UART_IER_TBEI );		// Interrupt if enabled

    wait( txReceived );			// Wait for a Tx request
    tx.write( regs.thr );		// Send char to terminal
  }
}	// busThread()


// ----------------------------------------------------------------------------
//! SystemC method sensitive to data on the Rx buffer

//! Copies the character received into the read buffer.

//! Sets the data ready flag of the line status register and sends an
//! interrupt (if enabled) to indicate the data is ready.

//! @note The terminal attached to the FIFO is responsible for modeling any
//!       wire delay on the Rx.
// ----------------------------------------------------------------------------
void
UartSC::rxMethod()
{
  regs.rbr  = rx.read();

  sc_core::sc_time  now = sc_core::sc_time_stamp();
  // cout << "Char " << (char)(regs.rbr) << " read at  " << sc_time_stamp ()
  //      << endl;

  set( regs.lsr, UART_LSR_DR );		// Mark data ready
  genIntr( UART_IER_RBFI );		// Interrupt if enabled

}	// rxMethod()


// ----------------------------------------------------------------------------
//! TLM2.0 blocking transport routine for the UART bus socket

//! Receives transport requests on the target socket.

//! Break out the address, data and byte enable mask. All data is host endian,
//! so we do not need to allow for Orksim model endianism in calculating this
//! (see UartSC::isLittleEndian).

//! Switches on the command and calls UartSC::busRead() or UartSC::buswrite()
//! routines to do the behavior

//! Increases the delay as appropriate and sets a success response.

//! @param payload  The transaction payload
//! @param delay    How far the initiator is beyond baseline SystemC time. For
//!                 use with temporal decoupling.
// ----------------------------------------------------------------------------
void
UartSC::busReadWrite( tlm::tlm_generic_payload &payload,
		      sc_core::sc_time         &delay )
{
  // Break out the address, mask and data pointer. This should be only a
  // single byte access.
  sc_dt::uint64      addr    = payload.get_address();
  unsigned char     *maskPtr = payload.get_byte_enable_ptr();
  unsigned char     *dataPtr = payload.get_data_ptr();

  int                offset;		// Data byte offset in word
  unsigned char      uaddr;		// UART address

  // Deduce the byte address (endianness independent)
  if (0xff == maskPtr[0])
    {
      offset = 0;
    }
  else if (0xff == maskPtr[1])
    {
      offset = 1;
    }
  else if (0xff == maskPtr[2])
    {
      offset = 2;
    }
  else if (0xff == maskPtr[3])
    {
      offset = 3;
    }
  else
    {
      payload.set_response_status( tlm::TLM_GENERIC_ERROR_RESPONSE );
      return;
    }

  // Mask off the address to its range. This ought to have been done already
  // by an arbiter/decoder.
  uaddr = (unsigned char)((addr + offset) & UART_ADDR_MASK);

  // Which command?
  switch( payload.get_command() ) {
  case tlm::TLM_READ_COMMAND:  dataPtr[offset] = busRead( uaddr ); break;
  case tlm::TLM_WRITE_COMMAND: busWrite( uaddr, dataPtr[offset] ); break;
  case tlm::TLM_IGNORE_COMMAND:
    payload.set_response_status( tlm::TLM_GENERIC_ERROR_RESPONSE );
    return;
  }

  // Single byte accesses always work
  payload.set_response_status( tlm::TLM_OK_RESPONSE );

}	// busReadWrite()


// ----------------------------------------------------------------------------
//! Process a read on the UART bus

//! Switch on the address to determine behavior
//! - UART_BUF
//!   - if UART_LSR_DLAB is set, read the low byte of the clock divisor
//!   - otherwise get the value from the read buffer and
//!     clear the data ready flag in the line status register
//! - UART_IER
//!   - if UART_LSR_DLAB is set, read the high byte of the clock divisor
//!   - otherwise get the instruction enable register
//! - UART_IIR Get the interrupt indicator register and
//!   clear the most important pending interrupt (UartSC::clrIntr())
//! - UART_LCR Get the line control register
//! - UART_MCR Ignored - write only register
//! - UART_LSR Get the line status register
//! - UART_MSR Get the modem status registe
//! - UART_SCR Get the scratch register

//! @param uaddr  The address of the register being accessed

//! @return  The value read
// ----------------------------------------------------------------------------
unsigned char
UartSC::busRead( unsigned char  uaddr )
{
  unsigned char  res = 0;		// The result to return

  // State machine lookup on the register
  switch( uaddr ) {
  case UART_BUF:
    if( isSet(regs.lcr, UART_LCR_DLAB ) ) {	// DLL byte
      res = (unsigned char)(regs.dl & 0x00ff);
    }
    else {
      res = regs.rbr;				// Get the read data
      clr( regs.lsr, UART_LSR_DR );		// Clear the data ready bit
      clrIntr( UART_IER_RBFI );
    }
    break;

  case UART_IER:
    if( isSet( regs.lcr, UART_LCR_DLAB ) ) {	// DLH byte
      res = (unsigned char)((regs.dl & 0xff00) >> 8);
    }
    else {
      res = regs.ier;
    }
    break;

  case UART_IIR:
    res = regs.iir;
    clrIntr( UART_IER_TBEI );
    break;

  case UART_LCR: res = regs.lcr; break;
  case UART_MCR: res = 0;        break;		// Write only
  case UART_LSR:
    res = regs.lsr;
    clr( regs.lsr, UART_LSR_BI );
    clr( regs.lsr, UART_LSR_FE );
    clr( regs.lsr, UART_LSR_PE );
    clr( regs.lsr, UART_LSR_OE );
    clrIntr( UART_IER_RLSI );
    break;

  case UART_MSR:
    res      = regs.msr;
    regs.msr = 0;
    clrIntr( UART_IER_MSI );
    modemLoopback();				// May need resetting
    break;

  case UART_SCR: res = regs.scr; break;
  }

  return res;

}	// busRead()


// ----------------------------------------------------------------------------
//! Process a write on the UART bus

//! Switch on the address to determine behavior
//! - UART_BUF
//!   - if UART_LSR_DLAB is set, write the low byte of the clock divisor and
//!     recalculate the character delay (UartSC::resetCharDelay())
//!   - otherwise write the data to the transmit buffer, clear the buffer
//!     empty flags and notify the UartSC::busThread() using the
//!     UartSC::txReceived SystemC event.
//! - UART_IER
//!   - if UART_LSR_DLAB is set, write the high byte of the clock divisor and
//!     recalculate the character delay (UartSC::resetCharDelay())
//!   - otherwise set the instruction enable register
//! - UART_IIR Ignored - read only
//! - UART_LCR Set the line control register
//! - UART_MCR Set the modem control regsiter
//!   - if loopback is set, set the MSR registers to correspond
//! - UART_LSR Ignored - read only
//! - UART_MSR Ignored - read only
//! - UART_SCR Set the scratch register

//! @param uaddr  The address of the register being accessed
//! @param wdata  The value to be written
// ----------------------------------------------------------------------------
void
UartSC::busWrite( unsigned char  uaddr,
		  unsigned char  wdata )
{
  // State machine lookup on the register
  switch( uaddr ) {
  case UART_BUF:
    if( isSet( regs.lcr, UART_LCR_DLAB ) ) {	// DLL
      regs.dl = (regs.dl & 0xff00) | (unsigned short int)wdata;
    }
    else {
      regs.thr = wdata;

      clr( regs.lsr, UART_LSR_TEMT );		// Tx buffer now full
      clr( regs.lsr, UART_LSR_THRE );
      clrIntr( UART_IER_TBEI );

      txReceived.notify();			// Tell the bus thread
    }
    break;

  case UART_IER:
    if( isSet( regs.lcr, UART_LCR_DLAB ) ) {	// DLH
      regs.dl = (regs.dl & 0x00ff) | ((unsigned short int)wdata << 8);
    }
    else {
      regs.ier = wdata;
    }
    break;

  case UART_IIR:                   break;	// Read only
  case UART_LCR: regs.lcr = wdata; break;
  case UART_MCR: 
    regs.mcr = wdata;
    modemLoopback();
    break;

  case UART_LSR:                   break;	// Read only
  case UART_MSR:                   break;	// Read only
  case UART_SCR: regs.scr = wdata; break;
  }

}	// busWrite()


// ----------------------------------------------------------------------------
//! Generate modem loopback signals

//! Software relies on this to detect the UART type. Set the modem status bits
//! as defined for modem loopback
// ----------------------------------------------------------------------------
void
UartSC::modemLoopback()
{
  // Only if we are in loopback state
  if( isClr( regs.mcr, UART_MCR_LOOP )) {
    return;
  }

  // Delta status bits for what is about to change.

  if( (isSet( regs.mcr, UART_MCR_RTS ) && isClr( regs.msr, UART_MSR_CTS )) ||
      (isClr( regs.mcr, UART_MCR_RTS ) && isSet( regs.msr, UART_MSR_CTS )) ) {
    set( regs.msr, UART_MSR_DCTS );
  }
  else {
    clr( regs.msr, UART_MSR_DCTS );
  }

  if( (isSet( regs.mcr, UART_MCR_DTR ) && isClr( regs.msr, UART_MSR_DSR )) ||
      (isClr( regs.mcr, UART_MCR_DTR ) && isSet( regs.msr, UART_MSR_DSR )) ) {
    set( regs.msr, UART_MSR_DDSR );
  }
  else {
    clr( regs.msr, UART_MSR_DDSR );
  }

  if( (isSet( regs.mcr, UART_MCR_OUT1 ) && isClr( regs.msr, UART_MSR_RI )) ||
      (isClr( regs.mcr, UART_MCR_OUT1 ) && isSet( regs.msr, UART_MSR_RI )) ) {
    set( regs.msr, UART_MSR_TERI );
  }
  else {
    clr( regs.msr, UART_MSR_TERI );
  }

  if( (isSet( regs.mcr, UART_MCR_OUT2 ) && isClr( regs.msr, UART_MSR_DCD )) ||
      (isClr( regs.mcr, UART_MCR_OUT2 ) && isSet( regs.msr, UART_MSR_DCD )) ) {
    set( regs.msr, UART_MSR_DDCD );
  }
  else {
    clr( regs.msr, UART_MSR_DDCD );
  }

  // Loopback status bits

  if( isSet( regs.mcr, UART_MCR_RTS )) {	// CTS = RTS
    set( regs.msr, UART_MSR_CTS );
  }
  else {
    clr( regs.msr, UART_MSR_CTS );
  }

  if( isSet( regs.mcr, UART_MCR_DTR )) {	// DSR = DTR
    set( regs.msr, UART_MSR_DSR );
  }
  else {
    clr( regs.msr, UART_MSR_DSR );
  }

  if( isSet( regs.mcr, UART_MCR_OUT1 )) {	// RI = OUT1
    set( regs.msr, UART_MSR_RI );
  }
  else {
    clr( regs.msr, UART_MSR_RI );
  }

  if( isSet( regs.mcr, UART_MCR_OUT2 )) {	// DSR = DTR
    set( regs.msr, UART_MSR_DCD );
  }
  else {
    clr( regs.msr, UART_MSR_DCD );
  }

  if( isSet( regs.msr, UART_MSR_DCTS ) |
      isSet( regs.msr, UART_MSR_DDSR ) |
      isSet( regs.msr, UART_MSR_TERI ) |
      isSet( regs.msr, UART_MSR_DDCD ) ) {
    genIntr( UART_IER_MSI );
  }

}	// modemLoopback()


// ----------------------------------------------------------------------------
//! Internal utility to set the IIR flags

//! The IIR bits are set for the highest priority outstanding interrupt.
// ----------------------------------------------------------------------------
void
UartSC::setIntrFlags()
{
  clr( regs.iir, UART_IIR_MASK );			// Clear current
  clr( regs.iir, UART_IIR_IPEND );			// 0 = pending

  if( isSet( intrPending, UART_IER_RLSI )) {		// Priority order
    set( regs.iir, UART_IIR_RLS );
  }
  else if( isSet( intrPending, UART_IER_RBFI )) {
    set( regs.iir, UART_IIR_RDA );
  }
  else if( isSet( intrPending, UART_IER_TBEI )) {
    set( regs.iir, UART_IIR_THRE );
  }
  else if( isSet( intrPending, UART_IER_MSI )) {
    set( regs.iir, UART_IIR_MOD );
  }
  else {
    set( regs.iir, UART_IIR_IPEND );			// 1 = not pending
  }
}	// setIntrFlags()


// ----------------------------------------------------------------------------
//! Generate an interrupt

//! If the particular interrupt is enabled, set the relevant interrupt
//! indicator flag, mark an interrupt as pending.

//! @note There is no actual interrupt port in this class. The interrupt
//!       signal driving functionality will be added in a derived class

//! @param ierFlag  Indicator of which interrupt is to be cleared (as IER bit).
// ----------------------------------------------------------------------------
void
UartSC::genIntr( unsigned char  ierFlag )
{
  if( isSet( regs.ier, ierFlag )) {
    set( intrPending, ierFlag );	// Mark this interrupt as pending.
    setIntrFlags();			// Show highest priority
  }
}	// genIntr()


// ----------------------------------------------------------------------------
//! Clear an interrupt

//! Clear the interrupts in priority order.

//! If no interrupts remain asserted, clear the interrupt pending flag

//! @note There is no actual interrupt port in this class. The interrupt
//!       signal driving functionality will be added in a derived class

//! @param ierFlag  Indicator of which interrupt is to be cleared (as IER bit).
// ----------------------------------------------------------------------------
void
UartSC::clrIntr( unsigned char ierFlag )
{
  clr( intrPending, ierFlag );		// Mark this interrupt as not pending
  setIntrFlags();

}	// clrIntr()


// ----------------------------------------------------------------------------
//! Set a bits in a register

//! @param reg    The register concerned
//! @param flags  The bits to set
// ----------------------------------------------------------------------------
void
UartSC::set( unsigned char &reg,
	     unsigned char  flags )
{
  reg |= flags;

}	// set()


// ----------------------------------------------------------------------------
//! Clear a bits in a register

//! @param reg    The register concerned
//! @param flags  The bits to set
// ----------------------------------------------------------------------------
void
UartSC::clr( unsigned char &reg,
	     unsigned char  flags )
{
  reg &= ~flags;

}	// clr()


// ----------------------------------------------------------------------------
//! Report if bits are set in a register

//! @param reg    The register concerned
//! @param flags  The bit to set

//! @return  True if the bit is set
// ----------------------------------------------------------------------------
bool
UartSC::isSet( unsigned char  reg,
	       unsigned char  flags )
{
  return  flags == (reg & flags);

}	// isSet()


// ----------------------------------------------------------------------------
//! Report if bits are clear in a register

//! @param reg    The register concerned
//! @param flags  The bit to set

//! @return  True if the bit is clear
// ----------------------------------------------------------------------------
bool
UartSC::isClr( unsigned char  reg,
	       unsigned char  flags )
{
  return  flags != (reg & flags);

}	// isClr()


// EOF
