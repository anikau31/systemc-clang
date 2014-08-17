// Terminal with synchronized timing module definition

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

#ifndef TERM_SYNC_SC__H
#define TERM_SYNC_SC__H

#include "TermSC.h"


// ----------------------------------------------------------------------------
//! SystemC module class for the Terminal with synchronized timing

//! Talks to the outside world via two systemC FIFOs. Any data coming in has
//! already been delayed (to represent the baud rate wire delay) via the
//! UART. Any data we send out is similarly delayed.
// ----------------------------------------------------------------------------
class TermSyncSC
  : public TermSC
{
public:

  TermSyncSC( sc_core::sc_module_name  name,
	      unsigned long int        baudRate );


private:

  // Reimplemented thread with timing to model the baud rate delay. Will not
  // be modified further in derived classes.
  void  xtermThread();

  //!< Baud rate delay to send a character
  sc_core::sc_time  charDelay;

};	/* TermSyncSC() */


#endif	// TERM_SYNC_SC__H

// EOF
