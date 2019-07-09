// Decouped loosely timed SoC SystemC with interrupts main program

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

#include "tlm.h"
#include "Or1ksimIntrSC.h"
#include "UartIntrSC.h"
#include "TermSyncSC.h"


using std::cerr;
using std::endl;


#define BAUD_RATE   115200		//!< Baud rate of the Linux console
#define QUANTUM_US      10		//!< Enough time for approx one bit

#define INTR_UART        2		//!< Interrupt line used by the UART


// ----------------------------------------------------------------------------
//! Main program building a SoC model with temporal decoupling, interrupts and
//! a debug interface.

//! Parses arguments, sets the global time quantum, instantiates the modules
//! and connects up the ports. Then runs forever.
// ----------------------------------------------------------------------------
int  sc_main( int   argc,
	      char *argv[] )
{
  if( argc != 3 ) {
    cerr << "Usage: intr-soc <config_file> <image_file>" << endl;
    exit( 1 );
  }

  // Set the global time quantum
  tlm::tlm_global_quantum &refTgq = tlm::tlm_global_quantum::instance();
  refTgq.set( sc_core::sc_time( QUANTUM_US, sc_core::SC_US ));

  // Instantiate the modules
  Or1ksimIntrSC    iss ("or1ksim", argv[1], argv[2]);
  UartIntrSC       uart ("uart", iss.getClockRate());
  TermSyncSC       term ("terminal", BAUD_RATE);

  // Connect up the TLM ports
  iss.dataBus( uart.bus );

  // Connect up the UART and terminal Tx and Rx
  uart.tx( term.rx );
  term.tx( uart.rx );

  // Connect the Uart to #2 interrupt on the iss
  uart.intr( iss.intr[INTR_UART] );

  // Run it forever
  sc_core::sc_start();
  return 0;			// Shouldn't get here!

}	// sc_main()


// EOF
