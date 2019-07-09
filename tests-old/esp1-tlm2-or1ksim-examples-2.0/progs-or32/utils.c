/* Utility printing routines for OpenRISC
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
/* l.nop constants only used here */
/* ------------------------------------------------------------------------- */
#define NOP_NOP         0x0000      /* Normal nop instruction */
#define NOP_EXIT        0x0001      /* End of simulation */
#define NOP_REPORT      0x0002      /* Simple report */
#define NOP_PRINTF      0x0003      /* Simprintf instruction */
#define NOP_PUTC        0x0004      /* JPB: Simputc instruction */
#define NOP_CNT_RESET   0x0005	    /* Reset statistics counters */


/* ------------------------------------------------------------------------- */
/*! Exit the simulator
 *!
 *! @param  rc Return code (not used)                                        */
/* ------------------------------------------------------------------------- */
void  simexit( int  rc )
{
  __asm__ __volatile__ ( "\tl.nop\t%0" : : "K"( NOP_EXIT ));

}	/* simexit() */


/* ------------------------------------------------------------------------- */
/*! Print a character
 *!
 *! @param  c Character to print                                             */
/* ------------------------------------------------------------------------- */
void  simputc( int  c )
{
  __asm__ __volatile__ ( "\tl.nop\t%0" : : "K"( NOP_PUTC ));

}	/* simputc() */


/* ------------------------------------------------------------------------- */
/*! Print a hex number
 *!
 *! @param  i Number to print                                                */
/* ------------------------------------------------------------------------- */
extern void  simputh( unsigned long int  i )
{
  char  lsd = i & 0xf;
  char  ch  = lsd < 10 ? '0' + lsd : 'A' + lsd - 10;

  if( i > 0 ) {
    simputh( i >> 4 );
  }

  simputc( ch );

}	/* simputh() */
  
    
/* ------------------------------------------------------------------------- */
/*! Print a string
 *!
 *! @param  str String to print                                              */
/* ------------------------------------------------------------------------- */
void  simputs( char *str )
{
  int  i;

  for( i = 0; str[i] != '\0' ; i++ ) {
    simputc( (int)(str[i]) );
  }

}	/* simputs() */
