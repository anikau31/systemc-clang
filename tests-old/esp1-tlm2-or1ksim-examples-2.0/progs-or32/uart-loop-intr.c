/* Loopback test of the UART using interrupts
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

#define BASEADDR   0x90000000
#define BAUD_RATE        9600
#define CLOCK_RATE  100000000		/* 100 Mhz */

struct uart16450
{
  volatile unsigned char  buf;		/* R/W: Rx & Tx buffer when DLAB=0 */
  volatile unsigned char  ier;		/* R/W: Interrupt Enable Register */
  volatile unsigned char  iir;		/* R: Interrupt ID Register */
  volatile unsigned char  lcr;		/* R/W: Line Control Register */
  volatile unsigned char  mcr;		/* W: Modem Control Register */
  volatile unsigned char  lsr;		/* R: Line Status Register */
  volatile unsigned char  msr;		/* R: Modem Status Register */
  volatile unsigned char  scr;		/* R/W: Scratch Register */
};

#define UART_IER_ETBEI  0x02		/* Enable trans holding register int. */
#define UART_IER_ERBFI  0x01		/* Enable receiver data interrupt */

#define UART_IIR_RDI    0x04		/* Receiver data interrupt */
#define UART_IIR_THRE   0x02	  	/* Trans holding reg empty interrupt */
#define UART_IIR_IPEND  0x01		/* Interrupt pending */

#define UART_LSR_TEMT   0x40		/* Transmitter serial register empty */
#define UART_LSR_THRE   0x20		/* Transmitter holding register empty */
#define UART_LSR_DR     0x01		/* Receiver data ready */

#define UART_LCR_DLAB   0x80		/* Divisor latch access bit */
#define UART_LCR_8BITS  0x03		/* 8 bit data bits */


#include "bitutils.c"


int
main()
{
  volatile struct uart16450 *uart = (struct uart16450 *)BASEADDR;
  unsigned short int         divisor;

  divisor = CLOCK_RATE/16/BAUD_RATE;		/* DL is for 16x baud rate */

  set( &(uart->lcr), UART_LCR_DLAB );		/* Set the divisor latch */
  uart->buf  = (unsigned char)( divisor       & 0x00ff);
  uart->ier  = (unsigned char)((divisor >> 8) & 0x00ff);
  clr( &(uart->lcr), UART_LCR_DLAB );

  set( &(uart->lcr), UART_LCR_8BITS );		/* Set 8 bit data packet */
  
  set( &(uart->ier), UART_IER_ERBFI | UART_IER_ETBEI );  /* Both Intrs */

  /* Loop echoing characters*/
  while( 1 ) {
    unsigned char  ch;

    do {			/* Loop until a char is available */
      ;
    } while( is_clr(uart->lsr, UART_LSR_DR) );

    ch = uart->buf;

    simputs( "Read: '" );	/* Log what was read */
    simputc( ch );
    simputs( "'\n" );

    do {			/* Wait for interrupts to clear */
      ;
    } while( is_set( uart->iir, UART_IIR_IPEND ) );

    do {			/* Loop until the trasmit register is free */
      ;
    } while( is_clr( uart->lsr, UART_LSR_TEMT | UART_LSR_THRE ) );
      
    uart->buf = ch;
  }

  return  0;			/* So compiler does not barf */

}	/* main () */
