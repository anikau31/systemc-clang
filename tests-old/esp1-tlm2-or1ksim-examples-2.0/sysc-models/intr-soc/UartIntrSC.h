// UART module with interrupt support definition

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

#ifndef UART_INTR_SC__H
#define UART_INTR_SC__H

#include "UartDecoupSC.h"


// ----------------------------------------------------------------------------
//! SystemC module class for a 16450 UART with temporal decoupling and
//! external interrupt.

//! Derived from UartDecoupSC:: to provide a thread which can drive a single
//! sc_signal port whether the interrupt is requested by either the existing
//! UartSyncSC::busThread() thread or UartSC::rxMethod() method.

//! @note Either of the other two processes may want to drive an interrupt,
//!       but only one process can drive a signal, so a separate thread is
//!       created and notified (via a one slot FIFO) by either of the other
//!       threads when it wishes to drive the interrupt.
// ----------------------------------------------------------------------------
class UartIntrSC
  : public UartDecoupSC
{
public:

  UartIntrSC (sc_core::sc_module_name  name,
	      unsigned long int        _clockRate);

  sc_core::sc_out<bool>  intr;		//!< Interrupt output port


private:

  // A single thread for driving interrupts. Only used in this class
  void  intrThread();

  // Reimplemented utility routines for interrupt handling, which drive the real
  // signal. Will not be reimplemented further.

  void  genIntr( unsigned char  ierFlag );
  void  clrIntr( unsigned char  ierFlag );

  //! A boolean fifo is used to communicate with the interrupt thread, to
  //! ensure that requests to set/clear are taken in the order they are sent.
  //! True to set, false to clear.
  sc_core::sc_fifo<bool>  intrQueue;

};	// UartIntrSC()


#endif	// UART_INTR_SC__H

// EOF
