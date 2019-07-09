/* Simple hello world program
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


int
main()
{
  simputs( "Hello World\n" );
  simexit( 0 );
  return  0;			/* So compiler does not barf */

}	/* main () */
