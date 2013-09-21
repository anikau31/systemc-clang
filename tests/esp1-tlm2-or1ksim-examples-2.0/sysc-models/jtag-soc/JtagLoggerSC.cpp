// JTAG logger module : implementation

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

#include "JtagLoggerSC.h"


using sc_core::sc_module_name;
using sc_core::sc_time;
using sc_core::SC_MS;
using sc_core::SC_ZERO_TIME;

using std::cerr;
using std::cout;
using std::dec;
using std::endl;
using std::hex;


SC_HAS_PROCESS (JtagLoggerSC);

//-----------------------------------------------------------------------------
//! Constructor for the JTAG logger

//! Associate the payload extension with the generic payload.

//! Declare the SystemC thread which will generate JTAG traffic.

//! @param[in] name  Name of this SystemC module
//-----------------------------------------------------------------------------
JtagLoggerSC::JtagLoggerSC (sc_module_name  name) :
  sc_module (name)
{
  // Hook the extension to the payload
  payload.set_extension (&ext);

  // The main thread
  SC_THREAD (runJtag);

}	// JtagLoggerSC ()


//-----------------------------------------------------------------------------
//! Main JTAG logging thread

//! Makes a series of JTAG transactions:
//! - a reset
//! - set the instruction to DEBUG
//! - read the value of NPR

//! We wait () between each transaction to allow the world to catch up. The
//! mutex of the wrapper will ensure the transaction is only processed between
//! calls to run ().
//-----------------------------------------------------------------------------
void
JtagLoggerSC::runJtag ()
{
  static const unsigned char DEBUG_INST = 0x8;	// JTAG DEBUG instruction
  static const unsigned char CPU0_MOD   = 0x1;  // JTAG CPU0 module ID
  static const unsigned char READ32     = 0x6;  // JTAG 32-bit read cmd
  static const unsigned int  SPR_NPC    = 0x10; // OR1200 NPC SPR
  sc_time  delay = SC_ZERO_TIME;

  // Reset, set the instruction and select the CPU0 module
  jtagReset (delay);
  cout << "Reset after " << delay << "." << endl;
  wait (SC_ZERO_TIME);

  delay = SC_ZERO_TIME;
  jtagInstruction (DEBUG_INST, delay);
  cout << "Instruction shifted after " << delay << "." << endl;
  wait (SC_ZERO_TIME);

  delay = SC_ZERO_TIME;
  jtagSelectModule (CPU0_MOD, delay);
  cout << "Module selected after " << delay << "." << endl;
  wait (SC_ZERO_TIME);

  // Loop forever reading the NPC SPR
  while (true)
    {
      // Specify the WRITE_COMMAND to read the NPC SPR
      delay = SC_ZERO_TIME;
      jtagWriteCommand (READ32, SPR_NPC, 4, delay);
      cout << "WRITE_COMMAND after " << delay << "." << endl;
      wait (SC_ZERO_TIME);

      // Read the data, remembering that OR1200 is big endian
      unsigned char  res[4];		// For the data read back

      delay = SC_ZERO_TIME;
      jtagGoCommandRead (res, 4, delay);
      cout << "GO_COMMAND after " << delay << "." << endl;
      cout << "- NPC = 0x" << hex << (int) res[3] << (int) res[2]
	   << (int) res[1] << (int) res[0] << dec << "." << endl;
      wait (sc_time (1000.0, SC_MS));
    }
}	// runJtag ()


//-----------------------------------------------------------------------------
//! Request a JTAG reset

//! Just mark the extension type as reset. All other fields in the payload and
//! extension are irrelevant.

//! @param[in,out] delay  The delay before and after the reset.
//-----------------------------------------------------------------------------
void
JtagLoggerSC::jtagReset (sc_time &delay)
{
  // Set up field in the extension and shift it
  ext.setType (JtagExtensionSC::RESET);

  jtag->b_transport (payload, delay);

  // Warn if response is not good
  if (!payload.is_response_ok ())
    {
      cerr << "Warning: JTAG reset failed" << endl;
    }
}	// jtagReset ()


//-----------------------------------------------------------------------------
//! Shift a JTAG instruction

//! Construct an instruction register with the specified instruction and shift
//! it in.

//! Like all the JTAG register fields, it must be reversed, so it is shifted
//! MS bit first.

//! @param[in]     instr  The instruction to set.
//! @param[in,out] delay  The delay before and after the instruction.
//-----------------------------------------------------------------------------
void
JtagLoggerSC::jtagInstruction (unsigned char  inst,
			       sc_time       &delay)
{
  // Allocate space and then populate with the reversed bits of the
  // instruction.
  unsigned char *jreg = new unsigned char[1];

  jreg[0] = reverseBits (inst, 4);

  // Set up the fields in the payload and extension and shift it
  payload.set_data_ptr (jreg);
  ext.setType (JtagExtensionSC::SHIFT_IR);
  ext.setBitSize (4);

  jtag->b_transport (payload, delay);

  // Warn if response is not good
  if (!payload.is_response_ok ())
    {
      cerr << "Warning: JTAG instruction failed." << endl;
    }

  // Free up register
  delete [] jreg;

}	// jtagInstruction ()

  
//-----------------------------------------------------------------------------
//! Select a JTAG debug module

//! Construct a module selection data register and shift it in.

//! Like all the JTAG registers, each field, must be reversed, so it is shifted
//! MS bit first. It requires a 32-bit CRC and returns one.

//! @param[in]     module_id  The module to select.
//! @param[in,out] delay      The delay before and after the module selection.
//-----------------------------------------------------------------------------
void
JtagLoggerSC::jtagSelectModule (unsigned char  moduleId,
				sc_time       &delay)
{
  // Compute the CRC
  unsigned long int  crcIn;

  crcIn = crc32 (1,        1, 0xffffffff);
  crcIn = crc32 (moduleId, 4, crcIn);

  // Reverse the fields
  moduleId = reverseBits (moduleId, 4);
  crcIn    = reverseBits (crcIn, 32);

  // Allocate space and initialize the register
  // - 1 indicator bit
  // - 4 module bits in
  // - 32 bit CRC in
  // - 4 bits status out
  // - 32 bits CRC out

  // Total 73 bits = 10 bytes
  int            numBytes = 10;
  unsigned char *jreg      = new unsigned char[numBytes];

  memset (jreg, 0, numBytes);

  jreg[0]  = 0x01;
  jreg[0] |= moduleId << 1;
  jreg[0] |= crcIn    << 5;
  jreg[1]  = crcIn    >> 3;
  jreg[2]  = crcIn    >> 11;
  jreg[3]  = crcIn    >> 19;
  jreg[4]  = crcIn    >> 27;

  // Set up the fields in the payload and extension and shift it
  payload.set_data_ptr (jreg);
  ext.setType (JtagExtensionSC::SHIFT_DR);
  ext.setBitSize (73);

  jtag->b_transport (payload, delay);

  // If we got a good response, decode the response register
  if (payload.is_response_ok ())
    {
      // Break out fields
      unsigned char      status;
      unsigned long int  crcOut;

      status = ((jreg[4] >> 5) | (jreg[5] << 3)) & 0xf ;

      crcOut = ((unsigned long int) jreg[5] >>  1) |
	       ((unsigned long int) jreg[6] <<  7) |
	       ((unsigned long int) jreg[7] << 15) |
	       ((unsigned long int) jreg[8] << 23) |
	       ((unsigned long int) jreg[9] << 31);

      // Reverse the fields and compute our own CRC
      status  = reverseBits (status,  4);
      crcOut = reverseBits (crcOut, 32);

      unsigned long int  crcComputed = crc32 (status, 4, 0xffffffff);

      // Check the results
      if (crcOut != crcComputed)
	{
	  cerr << "Warning: JTAG module selection CRC mismatch." << endl;
	}

      if (0 != status)
	{
	  cerr << "Warning: JTAG module selection non-zero status: 0x" << hex
	       << (unsigned int) status << dec << "." << endl;
	}
    }
  else
    {
      cerr << "Warning: JTAG module selection failed" << endl;
    }

  delete [] jreg;

}	// jtagSelectModule ()

  
//-----------------------------------------------------------------------------
//! Shift a JTAG debug WRITE_COMMAND data register

//! Construct a WRITE_COMMAND data register and shift it in.

//! Like all the JTAG registers, each field, must be reversed, so it is shifted
//! MS bit first. It requires a 32-bit CRC and returns one.

//! On return we get a status register and CRC.


//! @param[in]     accessType  The type of access (4 bits).
//! @param[in]     addr        32-bit address.
//! @param[in]     dataBytes   The number of bytes to shift (up to 2^16).
//! @param[in,out] delay       The delay before and after the register shift.
//-----------------------------------------------------------------------------
void
JtagLoggerSC::jtagWriteCommand (unsigned char      accessType,
				unsigned long int  addr,
				unsigned long int  dataBytes,
				sc_time           &delay)
{
  // Length field is one less than the number of bytes (since zero is not a
  // valid length)
  unsigned int  len = (unsigned int) (dataBytes - 1UL);

  // Compute the CRC
  unsigned long int  cmd = 2;		// WRITE_COMMAND
  unsigned long int  crcIn;

  crcIn = crc32 (0,           1, 0xffffffff);
  crcIn = crc32 (cmd,         4, crcIn);
  crcIn = crc32 (accessType,  4, crcIn);
  crcIn = crc32 (addr,       32, crcIn);
  crcIn = crc32 (len,        16, crcIn);

  // Reverse the fields
  cmd        = reverseBits (cmd,          4);
  accessType = reverseBits (accessType,  4);
  addr       = reverseBits (addr,        32);
  len        = reverseBits (len,         16);
  crcIn      = reverseBits (crcIn,      32);

  // Allocate space and initialize the register
  // -  1 indicator bit
  // -  4 bits command in
  // -  4 bits access type in
  // - 32 bits address in
  // - 16 bits length in
  // - 32 bits CRC in
  // -  4 bits status out
  // - 32 bits CRC out

  // Total 125 bits = 16 bytes
  int            numBytes = 16;
  unsigned char *jreg     = new unsigned char [numBytes];

  memset (jreg, 0, numBytes);

  jreg[ 0]  = 0x0;

  jreg[ 0] |= cmd        <<  1;

  jreg[ 0] |= accessType <<  5;
  jreg[ 1]  = accessType >>  3;

  jreg[ 1] |= addr       <<  1;
  jreg[ 2]  = addr       >>  7;
  jreg[ 3]  = addr       >> 15;
  jreg[ 4]  = addr       >> 23;
  jreg[ 5]  = addr       >> 31;

  jreg[ 5] |= len        <<  1;
  jreg[ 6]  = len        >>  7;
  jreg[ 7]  = len        >> 15;

  jreg[ 7] |= crcIn      <<  1;
  jreg[ 8]  = crcIn      >>  7;
  jreg[ 9]  = crcIn      >> 15;
  jreg[10]  = crcIn      >> 23;
  jreg[11]  = crcIn      >> 31;

  // Set up the fields in the payload and extension and shift it
  payload.set_data_ptr (jreg);
  ext.setType (JtagExtensionSC::SHIFT_DR);
  ext.setBitSize (125);

  jtag->b_transport (payload, delay);

  // If we got a good response, decode the response register
  if (payload.is_response_ok ())
    {
      // Break out fields
      unsigned char      status;
      unsigned long int  crcOut;

      status = (jreg[11] >> 1) & 0xf ;

      crcOut = ((unsigned long int) jreg[11] >>  5) |
	       ((unsigned long int) jreg[12] <<  3) |
	       ((unsigned long int) jreg[13] << 11) |
	       ((unsigned long int) jreg[14] << 19) |
	       ((unsigned long int) jreg[15] << 27);

      // Reverse the fields and compute our own CRC
      status  = reverseBits (status,  4);
      crcOut = reverseBits (crcOut, 32);

      unsigned long int  crcComputed = crc32 (status, 4, 0xffffffff);

      // Check the results
      if (crcOut != crcComputed)
	{
	  cerr << "Warning: JTAG WRITE_COMMAND CRC mismatch." << endl;
	}

      if (0 != status)
	{
	  cerr << "Warning: JTAG WRITE_COMMAND non-zero status: 0x" << hex
	       << (unsigned int) status << dec << "." << endl;
	}
    }
  else
    {
      cerr << "Warning: JTAG WRITE_COMMAND failed" << endl;
    }

  delete [] jreg;

}	// jtagWriteCommand ()

  
//-----------------------------------------------------------------------------
//! Shift a JTAG debug GO_COMMAND data register for reading

//! Construct a GO_COMMAND data register for reading and shift it in.

//! Like all the JTAG registers, each field, must be reversed, so it is shifted
//! MS bit first. It requires a 32-bit CRC and returns one.

//! On return we get a status register and CRC.

//! On return we get a data vector, status register and CRC.

//! Like all JTAG fields, the CRC shifted in, the data read back, the status
//! and CRC shifted out, must be reversed, since they are shifted in MS bit
//! first and out LS bit first.

//! @param[out]    data       Vector to hold the data returned.
//! @param[in]     dataBytes  The number of bytes expected back.
//! @param[in,out] delay      The delay before and after the register shift.
//-----------------------------------------------------------------------------
void
JtagLoggerSC::jtagGoCommandRead (unsigned char      data[],
				 unsigned long int  dataBytes,
				 sc_time           &delay)
{
  // Compute the CRC
  unsigned long int  cmd = 0;		// GO_COMMAND
  unsigned long int  crcIn;

  crcIn = crc32 (0,   1, 0xffffffff);
  crcIn = crc32 (cmd, 4, crcIn);

  // Reverse the fields
  cmd    = reverseBits (cmd, 4);
  crcIn = reverseBits (crcIn,  32);

  // Allocate space and initialize the register
  // -              1 indicator bit
  // -              4 bits command in
  // -             32 bits CRC in
  // - dataBytes * 8 bits access type out
  // -              4 bits status out
  // -             32 bits CRC out

  // Total 73 + dataBytes * 8 bits = 10 + dataBytes bytes */
  int            numBytes = 10 + dataBytes;
  unsigned char *jreg      = new unsigned char[numBytes];

  memset (jreg, 0, numBytes);

  jreg[0]  = 0x0;
  jreg[0] |= cmd << 1;

  jreg[0] |= crcIn <<  5;
  jreg[1]  = crcIn >>  3;
  jreg[2]  = crcIn >> 11;
  jreg[3]  = crcIn >> 19;
  jreg[4]  = crcIn >> 27;

  // Set up the fields in the payload and extension and shift it
  payload.set_data_ptr (jreg);
  ext.setType (JtagExtensionSC::SHIFT_DR);
  ext.setBitSize (73 + dataBytes * 8);

  jtag->b_transport (payload, delay);

  // If we got a good response, decode the response register
  if (payload.is_response_ok ())
    {
      // Break out fields
      unsigned char      status;
      unsigned long int  crcOut;

      for (unsigned int  i = 0; i < dataBytes; i++)
	{
	  data[i] = ((jreg[i + 4] >> 5) | (jreg[i + 5] << 3)) & 0xff;
	}

      status = ((jreg[dataBytes + 4] >> 5) | (jreg[dataBytes + 5] << 3)) & 0xf ;

      crcOut = ((unsigned long int) jreg[dataBytes + 5] >>  1) |
	       ((unsigned long int) jreg[dataBytes + 6] <<  7) |
	       ((unsigned long int) jreg[dataBytes + 7] << 15) |
	       ((unsigned long int) jreg[dataBytes + 8] << 23) |
	       ((unsigned long int) jreg[dataBytes + 9] << 31);

      // Reverse the fields
      for (unsigned int  i = 0; i < dataBytes; i++)
	{
	  data[i] = reverseBits (data[i], 8);
	}

      status = reverseBits (status,  4);
      crcOut = reverseBits (crcOut, 32);

      // Compute our own CRC
      unsigned long int  crcComputed = 0xffffffff;

      for (unsigned int  i = 0; i < dataBytes; i++)
	{
	  crcComputed = crc32 (data[i], 8, crcComputed);
	}
  
      crcComputed = crc32 (status, 4, crcComputed);

      // Check the results
      if (crcOut != crcComputed)
	{
	  cerr << "Warning: JTAG GO_COMMAND (read) CRC mismatch." << endl;
	}

      if (0 != status)
	{
	  cerr << "Warning: JTAG GO_COMMAND (read) non-zero status: 0x" << hex
	       << (unsigned int) status << dec << "." << endl;
	}
    }
  else
    {
      cerr << "Warning: JTAG GO_COMMAND (read) failed" << endl;
    }

  delete [] jreg;

}	// jtagGoCommandRead ()

  
//-----------------------------------------------------------------------------
//! Compute a IEEE 802.3 CRC-32.

//! Print an error message if we get a duff argument, but we really should
//! not.

//! @param[in] value     The value to shift into the CRC
//! @param[in] num_bits  The number of bits in the value.
//! @param[in] crc_in    The existing CRC

//! @return  The computed CRC.                                                */
//-----------------------------------------------------------------------------
unsigned long int
JtagLoggerSC::crc32 (unsigned long long int  value,
		     int                     num_bits,
		     unsigned long int       crc_in)
{
  if ((1 > num_bits) || (num_bits > 64))
    {
      cerr << "ERROR: Max 64 bits of CRC can be computed. Ignored." << endl;
      return  crc_in;
    }

  static const unsigned long int  CRC32_POLY = 0x04c11db7;

  // Compute the CRC, MS bit first
  for (int  i = num_bits - 1; i >= 0; i--)
    {
      unsigned long int  d;
      unsigned long int  t;

      d = (1 == ((value >> i) & 1))   ? 0xfffffff : 0x0000000;
      t = (1 == ((crc_in >> 31) & 1)) ? 0xfffffff : 0x0000000;

      crc_in <<= 1;
      crc_in  ^= (d ^ t) & CRC32_POLY;
    }

  return  crc_in;

}	// crc32 ()


//-----------------------------------------------------------------------------
//! Reverse a value's bits

//! @param[in] val  The value to reverse (up to 64 bits).
//! @param[in] len  The number of bits to reverse.

//! @return  The reversed value                                               */
//-----------------------------------------------------------------------------
unsigned long long
JtagLoggerSC::reverseBits (unsigned long long  val,
			   int                 len)
{
  if ((1 > len) || (len > 64))
    {
      printf ("ERROR: Cannot reverse %d bits. Returning zero\n", len);
      return  0;
    }

  // Reverse the string
  val = (((val & 0xaaaaaaaaaaaaaaaaULL) >>  1) |
	 ((val & 0x5555555555555555ULL) <<  1));
  val = (((val & 0xccccccccccccccccULL) >>  2) |
	 ((val & 0x3333333333333333ULL) <<  2));
  val = (((val & 0xf0f0f0f0f0f0f0f0ULL) >>  4) |
	 ((val & 0x0f0f0f0f0f0f0f0fULL) <<  4));
  val = (((val & 0xff00ff00ff00ff00ULL) >>  8) |
	 ((val & 0x00ff00ff00ff00ffULL) <<  8));
  val = (((val & 0xffff0000ffff0000ULL) >> 16) |
	 ((val & 0x0000ffff0000ffffULL) << 16));

  return  ((val >> 32) | (val << 32)) >> (64 - len);

}	// reverseBits ()
