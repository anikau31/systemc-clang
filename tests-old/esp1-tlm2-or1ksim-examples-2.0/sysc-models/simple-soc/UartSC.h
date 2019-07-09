// UART base module definition

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


#ifndef UART_SC__H
#define UART_SC__H

#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"


#define UART_ADDR_MASK      7	//!< Mask for addresses (3 bit bus)

// Offsets for the 16450 UART registers
#define UART_BUF  0		//!< R/W: Rx/Tx buffer, DLAB=0
#define UART_IER  1 		//!< R/W: Interrupt Enable Register, DLAB=0
#define UART_IIR  2 		//!< R: Interrupt ID Register
#define UART_LCR  3 		//!< R/W: Line Control Register
#define UART_MCR  4 		//!< W: Modem Control Register
#define UART_LSR  5 		//!< R: Line Status Register
#define UART_MSR  6 		//!< R: Modem Status Register
#define UART_SCR  7 		//!< R/W: Scratch Register

// Interrupt Enable register bits of interest
#define UART_IER_VALID  0x0f	//!< Mask for the valid bits

#define UART_IER_MSI    0x08    //!< Enable Modem status interrupt
#define UART_IER_RLSI   0x04    //!< Enable receiver line status interrupt
#define UART_IER_TBEI   0x02  	//!< Enable transmitter holding register int.
#define UART_IER_RBFI   0x01  	//!< Enable receiver data interrupt

// Interrupt Identification register bits and interrupt masks of interest
#define UART_IIR_IPEND  0x01  	//!< Interrupt pending (active low)

#define UART_IIR_MASK   0x06  	//!< the IIR status bits
#define UART_IIR_RLS    0x06  	//!< Receiver line status
#define UART_IIR_RDA    0x04  	//!< Receiver data available
#define UART_IIR_THRE   0x02  	//!< Transmitter holding reg empty
#define UART_IIR_MOD    0x00  	//!< Modem status

// Line Control register bits of interest and data word length mask
#define UART_LCR_DLAB   0x80	//!< Divisor latch access bit
#define UART_LCR_PEN    0x08  	//!< Parity Enable
#define UART_LCR_STB    0x04  	//!< Stop bits: 0=1 stop bit, 1= 2 stop bits
#define UART_LCR_MASK   0x03	//!< 2-bit mask for word length

// Line Status register bits of interest
#define UART_LSR_VALID  0x7f    //!< Valid bits for LSR

#define UART_LSR_TEMT   0x40  	//!< Transmitter serial register empty
#define UART_LSR_THRE   0x20  	//!< Transmitter holding register empty
#define UART_LSR_BI     0x10    //!< Break interrupt indicator
#define UART_LSR_FE     0x08    //!< Frame error indicator
#define UART_LSR_PE     0x04    //!< Parity error indicator
#define UART_LSR_OE     0x02    //!< Overrun error indicator
#define UART_LSR_DR     0x01  	//!< Receiver data ready

// Modem Control register bits of interest
#define UART_MCR_LOOP   0x10	//!< Enable loopback mode
#define UART_MCR_OUT2   0x08    //!< Auxilary output 2
#define UART_MCR_OUT1   0x04    //!< Auxilary output 1
#define UART_MCR_RTS    0x02    //!< Force RTS
#define UART_MCR_DTR    0x01    //!< Force DTR

// Modem Status register bits of interest
#define UART_MSR_DCD    0x80    //!< Data Carrier Detect
#define UART_MSR_RI     0x40    //!< Ring Indicator
#define UART_MSR_DSR    0x20    //!< Data Set Ready
#define UART_MSR_CTS    0x10    //!< Clear to Send
#define UART_MSR_DDCD   0x08    //!< Delta DCD
#define UART_MSR_TERI   0x04    //!< Trailing edge ring indicator
#define UART_MSR_DDSR   0x02    //!< Delta DSR
#define UART_MSR_DCTS   0x01    //!< Delta CTS


// ----------------------------------------------------------------------------
//! SystemC module class for a 16450 UART.

//! Provides a TLM 2.0 simple target convenience socket for access to the UART
//! regsters, unsigned char SystemC FIFO ports (to a 1 byte FIFO) for the Rx
//! and Tx pins and a bool SystemC signal for the interrupt pin.

//! A thread (UartSC::busThread()) is provided to wait for transmit requests
//! from the bus. A method (UartSC::rxMethod) is provided to wait for data on
//! the Rx pin.
// ----------------------------------------------------------------------------
class UartSC
  : public sc_core::sc_module
{
public:

  UartSC (sc_core::sc_module_name  name);

  //! Simple target convenience socket for UART bus access to registers
  tlm_utils::simple_target_socket<UartSC>  bus;

  // Buffer for input to the UART and port to connect to the terminal buffer
  // for output.

  sc_core::sc_buffer<unsigned char>        rx;	 //!< Buffer for Rx in
  sc_core::sc_out<unsigned char>           tx;	 //!< Port to terminal for Tx


protected:

  // A thread to run interaction with the bus side of the UART. This will be
  // reimplemented later in derived classes, so is declared virtual.
  virtual void  busThread();

  // Blocking transport function. Split out separate read and write
  // functions. The busReadWrite() and busWrite() functions will be
  // reimplemented in derived classes, so are declared virtual. The
  // busRead() function is only used here, so declared private (below).
  virtual void   busReadWrite( tlm::tlm_generic_payload &payload,
			       sc_core::sc_time         &delay );

  virtual void   busWrite( unsigned char  uaddr,
			   unsigned char  wdata );

  // Utility routines for interrupt handling. The setIntrFlags function is
  // needed in derived class. Generate and clear functions will be
  // reimplemented in derived classes.

  void          setIntrFlags();
  virtual void  genIntr( unsigned char  ierFlag );
  virtual void  clrIntr( unsigned char  ierFlag );

  // Flag handling utilities. Also reused in later derived classes.

  void  set( unsigned char &reg,
	     unsigned char  flag );
  void  clr( unsigned char &reg,
	     unsigned char  flag );
  bool  isSet( unsigned char  reg,
	       unsigned char  flag );
  bool  isClr( unsigned char  reg,
	       unsigned char  flag );

  //! UART event triggered by the CPU writing into the Tx buffer. This is
  //! reused by derived classes.
  sc_core::sc_event  txReceived;

  //! UART registers. These will be reused in later derived classes
  //! - rbr: R: Rx buffer,		      
  //! - thr: R: Tx hold reg,		      
  //! - ier: R/W: Interrupt Enable Register   
  //! - iir: R: Interrupt ID Register	      
  //! - lcr: R/W: Line Control Register	      
  //! - mcr: W: Modem Control Register	      
  //! - lsr: R: Line Status Register	      
  //! - msr: R: Modem Status Register	      
  //! - scr: R/W: Scratch Register            
  //! - dl:  R/W: Divisor Latch

  struct {
    unsigned char       rbr;		// R: Rx buffer,
    unsigned char       thr;		// R: Tx hold reg,
    unsigned char       ier;		// R/W: Interrupt Enable Register
    unsigned char       iir;		// R: Interrupt ID Register
    unsigned char       lcr;		// R/W: Line Control Register
    unsigned char       mcr;		// W: Modem Control Register
    unsigned char       lsr;		// R: Line Status Register
    unsigned char       msr;		// R: Modem Status Register
    unsigned char       scr;		// R/W: Scratch Register            
    unsigned short int  dl;		// R/W: Divisor Latch
  } regs;

  //! Which interrupts are currently pending
  unsigned char intrPending;


private:

  // The method to listen to the terminal side is only used in this class and
  // never reimplemented.
  void  rxMethod();

  //The busRead() function supports busReadWrite, and is only used in this
  //class, so declared private
  unsigned char  busRead( unsigned char  uaddr );

  // Modem loopback utility. Only used in this class.
  void  modemLoopback();

};	// UartSC()


#endif	// UART_SC__H

// EOF
