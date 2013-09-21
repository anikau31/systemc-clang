/* Test program for use with the logger module
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

#include "utils.h"

#define BASEADDR  0x90000000


struct  testdev
{
  volatile unsigned char       byte;
  volatile unsigned short int  halfword;
  volatile unsigned long  int  fullword;
};


int
main()
{
  struct testdev *dev = (struct testdev *)BASEADDR;

  unsigned char       byteRes;
  unsigned short int  halfwordRes;
  unsigned long int   fullwordRes;

  /* Write different sizes */
  simputs( "Writing byte 0xa5 to address 0x" );
  simputh( (unsigned long int)(&(dev->byte)) );
  simputs( "\n" );
  dev->byte     =       0xa5;

  simputs( "Writing half word 0xbeef to address 0x" );
  simputh( (unsigned long int)(&(dev->halfword)) );
  simputs( "\n" );
  dev->halfword =     0xbeef;

  simputs( "Writing full word 0xdeadbeef to address 0x" );
  simputh( (unsigned long int)(&(dev->fullword)) );
  simputs( "\n" );
  dev->fullword = 0xdeadbeef;

  /* Read different sizes */
  byteRes = dev->byte;
  simputs( "Read byte 0x" );
  simputh(  byteRes );
  simputs( " from address 0x" );
  simputh( (unsigned long int)(&(dev->byte)) );
  simputs( "\n" );

  halfwordRes = dev->halfword;
  simputs( "Read half word 0x" );
  simputh( halfwordRes );
  simputs( " from address 0x" );
  simputh( (unsigned long int)(&(dev->halfword)) );
  simputs( "\n" );

  fullwordRes = dev->fullword;
  simputs( "Read full word 0x" );
  simputh( fullwordRes );
  simputs( " from address 0x" );
  simputh( (unsigned long int)(&(dev->fullword)) );
  simputs( "\n" );

  /* Terminate the simulation */
  simexit( 0 );
  return  0;			/* So compiler does not barf */

}	/* main() */
