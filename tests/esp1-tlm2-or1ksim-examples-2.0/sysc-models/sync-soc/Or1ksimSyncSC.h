// Wrapper for synchronized loosely timed Or1ksim module definition

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

#ifndef OR1KSIM_SYNC_SC__H
#define OR1KSIM_SYNC_SC__H

#include "Or1ksimExtSC.h"


// ----------------------------------------------------------------------------
//! SystemC module class wrapping Or1ksim ISS with synchronized timing

//! Derived from the earlier Or1ksimExtSC class. Reimplements
//! Or1ksimExtSC::doTrans() to provide synchronized timing.
// ----------------------------------------------------------------------------
class Or1ksimSyncSC
  : public Or1ksimExtSC
{
public:

  Or1ksimSyncSC( sc_core::sc_module_name  name,
		 const char              *configFile,
		 const char              *imageFile );

  // Public utility to return the clock rate
  unsigned long int  getClockRate();


protected:

  // The common thread to make the transport calls. This has synchronized
  // timing. It will be further reimplemented in later calls to add termporal
  // decoupling.
  virtual void  doTrans( tlm::tlm_generic_payload &trans );

};	/* Or1ksimSyncSC() */


#endif	// OR1KSIM_SYNC_SC__H

// EOF
