// Wrapper for decoupled loosely timed Or1ksim module definition

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

#ifndef OR1KSIM_DECOUP_SC__H
#define OR1KSIM_DECOUP_SC__H

#include "Or1ksimSyncSC.h"
#include "tlm_utils/tlm_quantumkeeper.h"


// ----------------------------------------------------------------------------
//! SystemC module class wrapping Or1ksim ISS with temporal decoupling

//! Derived from the earlier Or1ksimSyncSC class. Reimplements the
//! Or1ksimSyncSC::run() thread to support temporal decoupling. Reimplements
//! the Or1ksimSynSC::doTrans() method to support temporal decoupling.
// ----------------------------------------------------------------------------
class Or1ksimDecoupSC
  : public Or1ksimSyncSC
{
public:

  Or1ksimDecoupSC( sc_core::sc_module_name  name,
		   const char              *configFile,
		   const char              *imageFile );


protected:

  //! TLM 2.0 system global quantum. A singleton. May be reused in derived
  //! classes.
  tlm::tlm_global_quantum *tgq;

  //! TLM 2.0 Quantum keeper for the ISS model thread. May be reused in
  //! derived classes.
  tlm_utils::tlm_quantumkeeper  issQk;

  // The common thread to make the transport calls. This has temporal
  // decoupling. Will be reimplemented in later calls
  virtual void  doTrans( tlm::tlm_generic_payload &trans );

  // Thread which will run the model, with temporal decoupling.
  virtual void  run();


private:

};	/* Or1ksimDecoupSC() */


#endif	// OR1KSIM_DECOUP_SC__H

// EOF
