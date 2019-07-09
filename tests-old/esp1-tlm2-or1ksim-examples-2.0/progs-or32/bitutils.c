/* OpenRISC Bit handling utilities
 *
 * Copyright (C) 2008, 2010 Embecosm Limited <info@embecosm.com>
 *
 * Contributor Jeremy Bennett <jeremy.bennett@embecosm.com>
 *
 * This file is part of the example programs for "Building a Loosely Timed SoC
 * Model with OSCI TLM 2.0"
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>. */
/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
/* This code is commented throughout for use with Doxygen.                   */
/* ------------------------------------------------------------------------- */


/* ------------------------------------------------------------------------- */
/*! Utility routine to set bit(s) in a volatile register
 *!
 *! @param reg   register
 *! @param flag  bit(s) to set                                               */
/* ------------------------------------------------------------------------- */
inline void  set( volatile unsigned char *reg,
		           unsigned char  flag )
{
  unsigned char  tmp = *reg;
  *reg = tmp | flag;

}	/* set() */


/* ------------------------------------------------------------------------- */
/*! Utility routine to clear bit(s) in a volatile register
 *!
 *! @param reg   register
 *! @param flag  bit(s) to set                                               */
/* ------------------------------------------------------------------------- */
inline void  clr( volatile unsigned char *reg,
		           unsigned char  flag )
{
  unsigned char  tmp = *reg;
  *reg = tmp & ~flag;

}	/* set() */


/* ------------------------------------------------------------------------- */
/*! Utility routine to identify if bits are all set in a register
 *!
 *! @param reg   register
 *! @param flag  bit(s) to test
 *!
 *! @return  1 if bits are all set, 0 otherwise                              */
/* ------------------------------------------------------------------------- */
inline int  is_set( volatile unsigned char  reg,
		             unsigned char  flag )
{
  unsigned char  tmp = reg & flag;
  return  flag == tmp;

}	/* is_set() */


/* ------------------------------------------------------------------------- */
/*! Utility routine to identify if bits are all clear in a register
 *!
 *! @param reg   register
 *! @param flag  bit(s) to test
 *!
 *! @return  1 if bits are all clear, 0 otherwise                            */
/* ------------------------------------------------------------------------- */
inline int  is_clr( volatile unsigned char  reg,
		             unsigned char  flag )
{
  unsigned char  tmp = reg & flag;
  return  0 == tmp;

}	/* is_clr() */
