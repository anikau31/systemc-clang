// TLM 2.0 extension class for JTAG : definition

// Copyright (C) 2010 Embecosm Limited

// Contributor: Jeremy Bennett <jeremy.bennett@embecosm.com>

// This file is part of the example programs for "Building a Loosely Timed SoC
// Model with OSCI TLM 2.0".

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

// --------------------------------------------------------------------------
// This code is commented throughout for use with Doxygen.
// --------------------------------------------------------------------------

#ifndef JTAG_EXTENSION_SC__H
#define JTAG_EXTENSION_SC__H

#include <tlm.h>


// --------------------------------------------------------------------------
//! A user defined ignorable TLM extension class for JTAG

//! The existence of our own protocol traits class means this will be
//! mandatory.
// --------------------------------------------------------------------------
class JtagExtensionSC: public tlm::tlm_extension<JtagExtensionSC>
{
public:

  //! An enum to record the access required
  enum AccessType {
    RESET,
    SHIFT_IR,
    SHIFT_DR
  };

  // Constructor
  JtagExtensionSC ();

  // Mandatory implementation of the clone method
  virtual tlm::tlm_extension_base* clone() const;

  // Mandatory implementation of the copy_from method
  virtual void copy_from (tlm::tlm_extension_base const &ext);

  // Accessors
  AccessType  getType () const;
  void        setType (AccessType _type);

  int         getBitSize () const;
  void        setBitSize (int  _bitSize);

  bool        getDebugEnabled () const;
  void        setDebugEnabled (bool  _debugEnabled);


private:

  //! Local copy of access type
  AccessType  type;

  //! Local copy of bit size
  int  bitSize;

  //! Local copy of whether debug is enabled
  bool  debugEnabled;

};	// class JtagExtensionSC


#endif
