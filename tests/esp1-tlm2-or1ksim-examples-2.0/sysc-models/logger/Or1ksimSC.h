// Or1ksim base wrapper module definition

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


#ifndef OR1KSIM_SC__H
#define OR1KSIM_SC__H

#include <stdint.h>

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "or1ksim.h"


// ----------------------------------------------------------------------------
//! SystemC module class wrapping Or1ksim ISS

//! Provides a single thread (::run) which runs the underlying Or1ksim ISS.
// ----------------------------------------------------------------------------
class Or1ksimSC
  : public sc_core::sc_module
{
public:

  //! Initiator port for data accesses
  tlm_utils::simple_initiator_socket<Or1ksimSC>  dataBus;

  Or1ksimSC (sc_core::sc_module_name  name,
	     const char              *configFile,
	     const char              *imageFile);


protected:

  // Thread which will run the model. This will be reimplemented in later
  // derived classes to deal with timing.
  virtual void  run ();

  // The common thread to make the transport calls. This will be reimplemented
  // in later derived classes to deal with timing.
  virtual void  doTrans (tlm::tlm_generic_payload &trans);


private:

  //! The transactional payload on the bus.

  //! The instinct is to declare this locally in the transaction method where
  //! it will be used. However the underlying SystemC data structures are
  //! apparently large with complex initialization. Although not documented in
  //! the LRM, it is far more efficient to declare the payload once here.

  //! Only one upcall is ever in use at a time, so there is no danger of a
  //! clash in usage.
  
  //! Since this is only used by the upcall methods, it can be private to this
  //! base class.
  tlm::tlm_generic_payload  trans;

  // I/O upcalls from Or1ksim, with a common synchronized transport
  // utility. These are not changed in later derived classes.
  static int  staticReadUpcall (void              *instancePtr,
				unsigned long int  addr,
				unsigned char      mask[],
				unsigned char      rdata[],
				int                dataLen);

  static int  staticWriteUpcall (void              *instancePtr,
				 unsigned long int  addr,
				 unsigned char      mask[],
				 unsigned char      wdata[],
				 int                dataLen);

  int         readUpcall (unsigned long int  addr,
			  unsigned char      mask[],
			  unsigned char      rdata[],
			  int                dataLen);

  int         writeUpcall (unsigned long int  addr,
			   unsigned char      mask[],
			   unsigned char      wdata[],
			   int                dataLen);

};	/* Or1ksimSC() */


#endif	// OR1KSIM_SC__H

// EOF
