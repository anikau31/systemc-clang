// Xterm terminal module definition

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


#ifndef TERM_SC__H
#define TERM_SC__H

#include <signal.h>

#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"
#include "or1ksim.h"


// ----------------------------------------------------------------------------
//! A convenience struct for lists of file descriptor -> instance mappings
// ----------------------------------------------------------------------------
struct Fd2Inst
{
  int           fd;		//!< The file descriptor
  class TermSC *inst;		//!< The instance (of class TermSC)
  Fd2Inst      *next;		//!< Next element in the list

};	// Fd2Inst


// ----------------------------------------------------------------------------
//! SystemC module class for the Terminal.

//! Talks to the outside world via a buffer (for Rx in) and a port to a buffer
//! in another module (Tx out).

//! The terminal is implemented as a separate process running an xterm. A 
//! thread listens for characters typed to the xterm, a method is sensitive to
//! characters sent from the UART.
// ----------------------------------------------------------------------------
class TermSC
  : public sc_core::sc_module
{
public:

  TermSC( sc_core::sc_module_name  name );
  ~TermSC();

  // Buffer for input to the terminal and port to connect to the UART buffer
  // for output

  sc_core::sc_buffer<unsigned char>  rx;	//!< Buffer for Rx in
  sc_core::sc_out<unsigned char>     tx;	//!< Port to UART for Tx


protected:

  // Thread to handle I/O for the xterm. Will be reimplemented in a derived
  // class.
  virtual void  xtermThread();

  // Utility function to read from the xterm. Will be reused in a derived
  // class.
  unsigned char  xtermRead();

  //! Pointer to the SystemC event raised when input is available. Reused in a
  //! derived class.

  //! @note This must be a pointer. If an event were declared here it would be
  //!       available at elaboration time and cause a crash. It is allocated
  //!       when the xterm is created.
  sc_core::sc_event *ioEvent;


private:

  // Method to handle I/O on the Rx buffer from the UART
  void  rxMethod();

  // Utility functions to control the xterm

  int   xtermInit();
  void  xtermKill( const char *mess );
  void  xtermLaunch( char *slaveName );
  int   xtermSetup();

  // Function to write to the xterm. Only used in this class.
  void  xtermWrite( unsigned char  ch );

  // Signal handling is tricky, since we have to use a static
  // function. Fortunately each instance has a 1:1 mapping to the FD used for
  // the xterm, so we can use that for lookup.
  static void  ioHandler( int        signum,
			  siginfo_t *si,
			  void      *p );

  //! The list of mappings of file descriptors to TermSC instances
  static Fd2Inst    *instList;

  // xterm state

  int  ptMaster;	//!< Master file descriptor
  int  ptSlave;		//!< Slave file descriptor (for talking to the xterm)
  int  xtermPid;	//!< Process ID of the child running the xterm

};	/* TermSC() */


#endif	// TERM_SC__H

// EOF
