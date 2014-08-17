// Or1ksim wrapper module with wait implementation

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

#include "Or1ksimExtSC.h"

// ----------------------------------------------------------------------------
//! Custom constructor for the Or1ksimExtSC extended SystemC module

//! Just calls the parent contstructor.

//! @param name        SystemC module name
//! @param configFile  Config file for the underlying ISS
//! @param imageFile   Binary image to run on the ISS
// ----------------------------------------------------------------------------
Or1ksimExtSC::Or1ksimExtSC ( sc_core::sc_module_name  name,
			     const char              *configFile,
			     const char              *imageFile ) :
  Or1ksimSC( name, configFile, imageFile )
{
 }	// Or1ksimExtSC()


// ----------------------------------------------------------------------------
//! Extended TLM transport to the target

//! Calls the blocking transport routine for the initiator socket (@see
//! ::dataBus).

//! This version adds a zero time delay call to wait() so that the thread will
//! yield in an untimed model.

//! @param trans  The transaction payload
// ----------------------------------------------------------------------------
void
Or1ksimExtSC::doTrans( tlm::tlm_generic_payload &trans )
{
  sc_core::sc_time  dummyDelay = sc_core::SC_ZERO_TIME;

  // Call the transport and wait for no time, which allows the thread to yield
  // and others to get a look in!

  dataBus->b_transport( trans, dummyDelay );
  wait( sc_core::SC_ZERO_TIME );

}	// doTrans()


// EOF
