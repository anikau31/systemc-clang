/* default.ld. Default linker script for OR1200 test programs

   Copyright (C) 2010 Embecosm Limited

   Contributor Jeremy Bennett <jeremy.bennett@embecosm.com>

   This file is part of the example programs for "Building a Loosely Timed SoC
   Model with OSCI TLM 2.0"

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 3 of the License, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
   more details.

   You should have received a copy of the GNU General Public License along
   with this program.  If not, see <http:  www.gnu.org/licenses/>.  */

/* ----------------------------------------------------------------------------
   This code is commented throughout for use with Doxygen.
   --------------------------------------------------------------------------*/

MEMORY
{
	ram : ORIGIN = 0x00000000, LENGTH = 0x00200000
}

/* Force _start to be linked in from the library */
EXTERN(_start)

SECTIONS
{
	/* Section .boot-text guarantees that the code for exception
	   handling is placed first. */
	.text :
        {
	*(.boot-text)
        *(.text)
        *(.rodata)
        } > ram

	.data :
	{
	*(.data)
        } > ram

	.bss :
	{
	*(.bss)
        } > ram

	.stack ALIGN (0x10) (NOLOAD) :
	{
        *(.stack)
        } > ram
}

ENTRY (_start)
