// Simple SoC SystemC main program

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
#include "Or1ksimExtSC.h"
#include "UartSC.h"
#include "TermSC.h"


using std::cerr;
using std::endl;


// ----------------------------------------------------------------------------
//! Main program building a simple SoC model

//! Parses arguments, instantiates the modules and connects up the ports. Then
//! runs forever.
// ----------------------------------------------------------------------------
int  sc_main( int   argc,
	      char *argv[] )
{
  if( argc != 3 ) {
    cerr << "Usage: simple-soc <config_file> <image_file>" << endl;
    exit( 1 );
  }

  // Instantiate the modules
  Or1ksimExtSC  iss ("or1ksim", argv[1], argv[2]);
  UartSC        uart ("uart");
  TermSC        term ("terminal");

  // Connect up the TLM ports
  iss.dataBus( uart.bus );

  // Connect up the UART and terminal Tx and Rx
  uart.tx( term.rx );
  term.tx( uart.rx );

  // Run it forever
  sc_core::sc_start();
  return 0;			// Should never happen!

}	// sc_main()


// EOF
