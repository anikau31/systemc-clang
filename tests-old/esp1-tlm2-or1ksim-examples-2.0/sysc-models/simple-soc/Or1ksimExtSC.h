// Or1ksim wrapper module with wait definition

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


#ifndef OR1KSIM_EXT_SC__H
#define OR1KSIM_EXT_SC__H

#include "Or1ksimSC.h"


// ----------------------------------------------------------------------------
//! Extended SystemC module class wrapping Or1ksim ISS

//! Derived from Or1ksimSC:: to reimplement Or1ksimSC::doTrans(). Everything
//! else comes from the parent module.
// ----------------------------------------------------------------------------
class Or1ksimExtSC
  : public Or1ksimSC
{
public:

  Or1ksimExtSC( sc_core::sc_module_name  name,
		const char              *configFile,
		const char              *imageFile );


protected:

  // Reimplmented version of the common thread to make the transport
  // calls. This will be further reimplemented in later derived classes to
  // deal with timing.
  virtual void  doTrans( tlm::tlm_generic_payload &trans );

};	/* Or1ksimExtSC() */


#endif	// OR1KSIM_EXT_SC__H

// EOF
