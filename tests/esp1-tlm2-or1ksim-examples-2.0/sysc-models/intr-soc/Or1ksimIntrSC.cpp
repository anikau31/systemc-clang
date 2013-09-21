// Wrapper for loosely timed Or1ksim module with interrupts implementation

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

#include "Or1ksimIntrSC.h"


SC_HAS_PROCESS( Or1ksimIntrSC );

// ----------------------------------------------------------------------------
//! Custom constructor for the Or1ksimIntrSC SystemC module with interrupt
//! handling

//! Calls the constructor of the base class, Or1ksimSyncSC::, then sets up a
//! thread for the signal ports.

//! @param name           SystemC module name
//! @param configFile     Config file for the underlying ISS
//! @param imageFile      Binary image to run on the ISS
// ----------------------------------------------------------------------------
Or1ksimIntrSC::Or1ksimIntrSC ( sc_core::sc_module_name  name,
			       const char              *configFile,
			       const char              *imageFile ) :
  Or1ksimDecoupSC( name, configFile, imageFile )
{
  int  i;

  SC_METHOD( intrMethod );
  for( i = 0 ; i < NUM_INTR ; i++ ) {
    sensitive << intr[i].posedge_event();
  }
  dont_initialize();

}	/* Or1ksimIntrSC() */


// ----------------------------------------------------------------------------
//! Method to handle interrupt triggers.

//! Triggered statically on posedge write to any interrupt. Identifies the
//! generating trigger and signals the underlying Or1ksim ISS.
// ----------------------------------------------------------------------------
void
Or1ksimIntrSC::intrMethod()
{
  int  i;

  for( i = 0 ; i < NUM_INTR ; i++ ) {
    if( intr[i].event()) {
      or1ksim_interrupt( i );
    }
  }

}	// intrMethod()


// EOF
