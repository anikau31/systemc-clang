// Or1ksim base wrapper module implementation

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


#include "Or1ksimSC.h"


SC_HAS_PROCESS( Or1ksimSC );

// ----------------------------------------------------------------------------
//! Custom constructor for the Or1ksimSC SystemC module

//! Initializes the underlying Or1ksim ISS, passing in the configuration and
//! image files, a pointer to this class instance and pointers to the static
//! upcall routines (@see ::staticReadUpcall() and ::staticWriteUpcall()).

//! Declares the main SystemC thread, ::run().

//! @param name        SystemC module name
//! @param configFile  Config file for the underlying ISS
//! @param imageFile   Binary image to run on the ISS
// ----------------------------------------------------------------------------
Or1ksimSC::Or1ksimSC (sc_core::sc_module_name  name,
		      const char              *configFile,
		      const char              *imageFile) :
  sc_module (name),
  dataBus ("data_initiator" )
{
  or1ksim_init (configFile, imageFile, this, staticReadUpcall,
		staticWriteUpcall);

  SC_THREAD (run);		  // Thread to run the ISS

}	// Or1ksimSC()


// ----------------------------------------------------------------------------
//! The SystemC thread running the underlying ISS

//! The thread calls the ISS to run forever.

//! There are upcalls for read and write to peripherals (@see
//! ::staticReadUpcall(), ::staticWriteUpcall, ::readUpcall(),
//! ::writeUpcall()), which provide opportunities for the thread to yield, and
//! so not block the simulation.
// ----------------------------------------------------------------------------
void
Or1ksimSC::run ()
{
  (void)or1ksim_run (-1.0);

}	// Or1ksimSC()


// ----------------------------------------------------------------------------
//! Static upcall for read from the underlying Or1ksim ISS

//! Static utility routine is used (for easy C linkage!) that will call the
//! class callback routine. Takes standard C types (the underlying ISS is in
//! C) as arguments, but calls the member routine, ::readUpcall(), with fixed
//! width types (from stdint.h).

//! @note In theory this ought to be possible by using member pointers to
//! functions. However given the external linkage is to C and not C++, this way
//! is much safer!

//! @param[in]  instancePtr  The pointer to the class member associated with
//!                          this upcall (originally passed to or1ksim_init in
//!                          the constructor, ::Or1ksimSC()).
//! @param[in]  addr         The address for the read
//! @param[in]  mask         The byte enable mask for the read
//! @param[out] rdata        Vector for the read data
//! @param[in]  dataLen      The number of bytes to read

//! @return  Zero on success. A return code otherwise.
// ----------------------------------------------------------------------------
int
Or1ksimSC::staticReadUpcall (void              *instancePtr,
			     unsigned long int  addr,
			     unsigned char      mask[],
			     unsigned char      rdata[],
			     int                dataLen)
{
  Or1ksimSC *classPtr = (Or1ksimSC *) instancePtr;
  return  classPtr->readUpcall (addr, mask, rdata, dataLen);

}	// staticReadUpcall()


// ----------------------------------------------------------------------------
//! Static upcall for write to the underlying Or1ksim ISS

//! Static utility routine is used (for easy C linkage!) that will call the
//! class callback routine. Takes standard C types (the underlying ISS is in
//! C) as arguments, but calls the member routine, ::writeUpcall(), with fixed
//! width types (from stdint.h).

//! @note In theory this ought to be possible by using member pointers to
//! functions. However given the external linkage is to C and not C++, this way
//! is much safer!

//! @param[in] instancePtr  The pointer to the class member associated with
//!                         this upcall (originally passed to or1ksim_init in
//!                         the constructor, ::Or1ksimSC()).
//! @param[in] addr         The address for the write
//! @param[in] mask         The byte enable mask for the write
//! @param[in] wdata        Vector of data to write
//! @param[in] dataLen      The number of bytes to write

//! @return  Zero on success. A return code otherwise.
// ----------------------------------------------------------------------------
int
Or1ksimSC::staticWriteUpcall (void              *instancePtr,
			      unsigned long int  addr,
			      unsigned char      mask[],
			      unsigned char      wdata[],
			      int                dataLen)
{
  Or1ksimSC *classPtr = (Or1ksimSC *) instancePtr;
  return  classPtr->writeUpcall (addr, mask, wdata, dataLen);

}	// staticWriteUpcall()


// ----------------------------------------------------------------------------
//! Member function to handle read upcall from the underlying Or1ksim ISS

//! This is the entry point used by ::staticReadUpcall(). Constructs a Generic
//! transactional payload for the read, then passes it to ::doTrans() (also
//! used by ::writeUpcall()) for transport to the target.

//! @param[in]  addr         The address for the read
//! @param[in]  mask         The byte enable mask for the read
//! @param[out] rdata        Vector for the read data
//! @param[in]  dataLen      The number of bytes to read

//! @return  Zero on success. A return code otherwise.
// ----------------------------------------------------------------------------
int
Or1ksimSC::readUpcall (unsigned long int  addr,
		       unsigned char      mask[],
		       unsigned char      rdata[],
		       int                dataLen)
{
  // Set up the payload fields. Assume everything is 4 bytes.
  trans.set_read ();
  trans.set_address ((sc_dt::uint64) addr);

  trans.set_byte_enable_length ((const unsigned int) dataLen);
  trans.set_byte_enable_ptr ((unsigned char *) mask);

  trans.set_data_length ((const unsigned int) dataLen);
  trans.set_data_ptr ((unsigned char *) rdata);

  // Transport.
  doTrans (trans);

  /* For now just simple non-zero return code on error */
  return  trans.is_response_ok () ? 0 : -1;

}	// readUpcall()


// ----------------------------------------------------------------------------
//! Member function to handle write upcall from the underlying Or1ksim ISS

//! This is the entry point used by ::staticWriteUpcall(). Constructs a
//! Generic transactional payload for the write, then passes it to ::doTrans()
//! (also used by ::readUpcall()) for transport to the target.

//! @param[in] addr         The address for the write
//! @param[in] mask         The byte enable mask for the write
//! @param[in] wdata        Vector of data to write
//! @param[in] dataLen      The number of bytes to write

//! @return  Zero on success. A return code otherwise.
// ----------------------------------------------------------------------------
int
Or1ksimSC::writeUpcall (unsigned long int  addr,
			unsigned char      mask[],
			unsigned char      wdata[],
			int                dataLen)
{
  // Set up the payload fields. Assume everything is 4 bytes.
  trans.set_write ();
  trans.set_address ((sc_dt::uint64) addr);

  trans.set_byte_enable_length ((const unsigned int) dataLen);
  trans.set_byte_enable_ptr ((unsigned char *) mask);

  trans.set_data_length ((const unsigned int) dataLen);
  trans.set_data_ptr ((unsigned char *) wdata);

  // Transport.
  doTrans( trans );

  /* For now just simple non-zero return code on error */
  return  trans.is_response_ok () ? 0 : -1;

}	// writeUpcall()


// ----------------------------------------------------------------------------
//! TLM transport to the target

//! Calls the blocking transport routine for the initiator socket (@see
//! ::dataBus). Passes in a dummy time delay of zero.

//! @param trans  The transaction payload
// ----------------------------------------------------------------------------
void
Or1ksimSC::doTrans( tlm::tlm_generic_payload &trans )
{
  sc_core::sc_time  dummyDelay = sc_core::SC_ZERO_TIME;
  dataBus->b_transport( trans, dummyDelay );

}	// doTrans()


// EOF
