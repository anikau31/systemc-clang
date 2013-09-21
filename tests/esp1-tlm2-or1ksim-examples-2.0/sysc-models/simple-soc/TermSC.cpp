// Xterm terminal module implementation

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

#include <unistd.h>
#include <iostream>
#include <stdlib.h>
#include <libgen.h>
#include <fcntl.h>
#include <signal.h>
#include <termios.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <iostream>
#include <iomanip>

#include "TermSC.h"


using sc_core::sc_time_stamp;

using std::cout;
using std::endl;


SC_HAS_PROCESS( TermSC );

// ----------------------------------------------------------------------------
//! Custom constructor for the terminal module

//! Passes the name to the parent constructor.

//! Sets up threads listening to the rx port from the UART (TermSC::rxThread)
//! and waiting for characters from the xterm (TermSC::xtermThread).

//! Initializes the xterm in a separate process, setting up a file descriptor
//! that can read and write from/to it.

//! @param name      Name of this module - passed to the parent constructor
// ----------------------------------------------------------------------------
TermSC::TermSC( sc_core::sc_module_name  name ) :
  sc_module( name )
{
  // Method for the Rx with static sensitivity
  SC_METHOD( rxMethod );
  sensitive << rx;
  dont_initialize();

  // Thread for the xterm
  SC_THREAD( xtermThread );

  // Start up the terminal
  xtermInit();

}	/* TermSC() */


// ----------------------------------------------------------------------------
//! Custom destructor for the terminal module

//! Custom destructors are not usually needed for SystemC models, but we use
//! this to kill the child process running the xterm.
// ----------------------------------------------------------------------------
TermSC::~TermSC()
{
  xtermKill( NULL );		// Get rid of the xterm

}	// ~TermSC()
  

// ----------------------------------------------------------------------------
//! Method handling characters on the Rx buffer

//! Statically sensitive to characters being written from the UART. When they
//! arrive, write them to the xterm. Print a time stamp
// ----------------------------------------------------------------------------
void
TermSC::rxMethod()
{
  xtermWrite( rx.read() );		// Write it to the screen

  // cout << "Char written at " << sc_time_stamp() << endl;

}	// rxMethod()


// ----------------------------------------------------------------------------
//! Thread listening for characters from the xterm.

//! Wait to be notified via the SystemC event TermSC::ioEvent that there is a
//! character available.

//! Read the character, then send it out to the UART.
// ----------------------------------------------------------------------------
void
TermSC::xtermThread()
{
  while( true ) {
    wait( *ioEvent );		// Wait for some I/O on the terminal
    tx.write( xtermRead());	// Get the char and sent it on
  }
}	// xtermThread()


// ----------------------------------------------------------------------------
//! Start an xTerm as a child process.

//! This uses the POSIX routines to obtain a pseudo TTY (pty) and then fire up
//! an xterm, using that pty for I/O

//! The I/O is set up to be canonical (i.e immediate, not at end of line),
//! with no echno.

//! Forks a child process to run the xterm, using the -S option to specify the
//! pty being used.

//! The parent is initalized using TermSC::xtermSetup()

//! @return   0 on success, -1 on failure, with an error code in errno
// ----------------------------------------------------------------------------
int
TermSC::xtermInit()
{
  // Set everything to a default state

  ptMaster = -1;
  ptSlave  = -1;
  xtermPid = -1;

  // Get a slave PTY from the master

  ptMaster = open("/dev/ptmx", O_RDWR);

  if( ptMaster < 0 ) {
    return -1;
  }

  if( grantpt( ptMaster ) < 0 ) {
    xtermKill( NULL );
    return -1;
  }

  if( unlockpt( ptMaster ) < 0 ) {
    xtermKill( NULL );
    return -1;
  }

  // Open the slave PTY

  char *ptSlaveName = ptsname( ptMaster );

  if( NULL == ptSlaveName ) {
    xtermKill( NULL );
    return -1;
  }

  ptSlave = open( ptSlaveName, O_RDWR );	// In and out are the same

  if( ptSlave < 0 ) {
    xtermKill( NULL );
    return -1;
  }

  // Turn off terminal echo and cannonical mode

  struct termios  termInfo;

  if( tcgetattr( ptSlave, &termInfo ) < 0 ) {
    xtermKill( NULL );
    return -1;
  }

  termInfo.c_lflag &= ~ECHO;
  termInfo.c_lflag &= ~ICANON;

  if( tcsetattr( ptSlave, TCSADRAIN, &termInfo ) < 0 ) {
    xtermKill( NULL );
    return -1;
  }

  // Fork off the xterm

  xtermPid = fork();

  switch( xtermPid ) {

  case -1:

    xtermKill( NULL );				// Failure
    return -1;

  case 0:

    xtermLaunch( ptSlaveName );		// Child opens the terminal
    return -1;				// Should be impossible!

  default:

    return  xtermSetup();		// The parent carries on.
  }

}	// xtermInit()


// ----------------------------------------------------------------------------
//! Kill an open xterm

//!  Remove the mapping from file descriptor to process for this xterm. Close
//! any open file descriptors and shut down the Xterm process.

//! If the message is non-null print it with perror.

//! @param mess  An error message
// ----------------------------------------------------------------------------
void
TermSC::xtermKill( const char *mess )
{
  if( -1 != ptSlave ) {			// Close down the slave
    Fd2Inst *prev = NULL;
    Fd2Inst *cur  = instList;

    while( NULL != cur ) {		// Delete this FD from the FD->inst
      if( cur->fd == ptSlave ) {	// mapping if it exists
	if( NULL == prev ) {
	  instList->next = cur->next;
	}
	else {
	  prev->next = cur->next;
	}

	delete cur->inst->ioEvent;	// Free the SystemC event
	delete cur;			// Free the node
	break;				// No more expected
      }
    }

    close( ptSlave );			// Close the FD
    ptSlave  = -1;
  }

  if( -1 != ptMaster ) {		// Close down the master
    close( ptMaster );
    ptMaster = -1;
  }

  if( xtermPid > 0 ) {			// Kill the terminal
    kill( xtermPid, SIGKILL );
    waitpid( xtermPid, NULL, 0 );
  }

  if( NULL != mess ) {			// If we really want a message
    perror( mess );
  }
}	// xtermKill()


// ----------------------------------------------------------------------------
//! Launch an xterm

//! Called in the child to launch the xterm. Should never return - just exit.

//! @param slaveName  The name of the slave pty to pass to xterm
// ----------------------------------------------------------------------------
void
TermSC::xtermLaunch( char *slaveName )
{
  char *arg;
  char *fin = &(slaveName[strlen( slaveName ) - 2]);	// Last 2 chars of name

  // Two different naming conventions for PTYs, lead to two different ways of
  // passing the -S parameter to xterm. Work out the length of string needed
  // in each case (max size of %d is sign + 19 chars for 64 bit!)

  if( NULL == strchr(fin, '/' )) {
    arg = new char[2 + 1 + 1 + 20 + 1];
    sprintf( arg, "-S%c%c%d", fin[0], fin[1], ptMaster );
  }
  else {
    char *slaveBase = ::basename( slaveName );

    arg = new char[2 + strlen( slaveBase ) + 1 + 20 + 1];
    sprintf( arg, "-S%s/%d", slaveBase, ptMaster );
  }

  // Set up the arguments (must be a null terminated list)

  char *argv[3];
  
  argv[0] = (char *)( "xterm" );
  argv[1] = arg;
  argv[2] = NULL;

  // Start up xterm. After this nothing should execute (the image is replaced)

  execvp( "xterm", argv );
  exit( 1 );			// Impossible!!!

}	// xtermLaunch()


// ----------------------------------------------------------------------------
//! Set up xterm I/O for the parent process

//! Called in the parent to set the terminal up. Swallows up the initial
//! string sent by the xterm.

//! I/O from the xterm MUST be non-blocking, since a thread that waits on an
//! operating system call will block the SystemC thread scheduling.

//! The I/O is set up to signal data using SIGIO, with a handler that will
//! then notify a SystemC event (TermSC::ioEvent).

//! The handler must be static, since it uses C, not C++ linkage, so a mapping
//! is needed to match the file descriptor to the class instance. This is a
//! linked list (allowing for multiple instances of this module), pointed to
//! by TermSC::instList.

//! The SystemC event is created here, rather than declared statically as a
//! member of the class. This avoids it being used during elaboration, which
//! will crash SystemC.

//! @return  -1 on failure, 0 on success
// ----------------------------------------------------------------------------
int
TermSC::xtermSetup()
{
  int   res;
  char  ch;

  // The xTerm spits out some code, followed by a newline which we swallow up

  do {
    res = read( ptSlave, &ch, 1 );
  } while( (res >= 0) && (ch != '\n') );

  if( res < 0 ) {
    xtermKill( NULL );
    return -1;
  }

  // I/O from the terminal will be asynchronous, with SIGIO triggered when
  // anything happens. That way, we can let the SystemC thread yield.

  // Record the mapping of slave FD to this instance

  Fd2Inst *newMap = new Fd2Inst;

  newMap->fd   = ptSlave;
  newMap->inst = this;
  newMap->next = instList;
  instList     = newMap;

  // Dynamically create the SystemC event we will use.
  ioEvent = new sc_core::sc_event();

  // Install a signal handler

  struct sigaction  action;
  action.sa_sigaction = ioHandler;
  action.sa_flags     = SA_SIGINFO;

  if( sigaction( SIGIO, (const struct sigaction *)&action, NULL ) != 0 ) {
    xtermKill( "Sigaction Failed" );
    return -1;
  }

  // Make the Slave FD asynchronous with this process.

  if( fcntl( ptSlave, F_SETOWN, getpid()) != 0 ) {
    xtermKill( "SETOWN" );
    return -1;
  }

  int flags = fcntl( ptSlave, F_GETFL );

  if( fcntl( ptSlave, F_SETFL, flags | O_ASYNC ) != 0 ) {
    xtermKill( "SETFL" );
    return -1;
  }

  return 0;		// Success

}	// xtermSetup()


//! Static pointer to the file descriptor to instance mapping list
Fd2Inst *TermSC::instList = NULL;

// ----------------------------------------------------------------------------
//! Static handler for SIGIO

//! Must be static, since sigaction cannot cope with a member function. The
//! static member list TermSC::instList holds details of each instance of this
//! class and its associated file descriptor for talking to the xterm.

//! POSIX select() is used to identify the source of the SIGIO, and the
//! appropriate instance is notified using the SystemC event in
//! TermSC::ioEvent.

//! @param signum  The signal invoking us (SIGIO)
//! @param si      A siginfo structure with information about the
//!                signal. According to POSIX, it should contain the FD for
//!                SIGIO, but that is not working, hence the use of select.
//! @param p       A ucontext_t structure, passed as void*.
// ----------------------------------------------------------------------------
void
TermSC::ioHandler( int        signum,
		   siginfo_t *si,
		   void      *p )
{
  // Use select to find which FD atually triggered to ioHandler. According to
  // the iterature, SIGIO ought to set this in the siginfo, but it doesn't.

  fd_set          readFdSet;
  struct timeval  timeout = { 0, 0 };
  int             maxFd   = 0;		// Largest FD we find

  FD_ZERO( &readFdSet );

  for( Fd2Inst *cur = instList; cur != NULL ; cur = cur->next ) {
    FD_SET( cur->fd, &readFdSet );
    maxFd = (cur->fd > maxFd ) ? cur->fd : maxFd;
  }

  // Non-blocking select, in case there is nothing there!!
  switch( select( maxFd + 1, &readFdSet, NULL, NULL, &timeout ) ) {
  case -1:
    perror( "TermSC: Error on handler select" );
    return;
  case 0:
    return;		// Nothing to read - ignore
  }

  // Now trigger the handlers of any FD's that were set
  for( Fd2Inst *cur = instList; cur != NULL ; cur = cur->next ) {
    if( FD_ISSET( cur->fd, &readFdSet )) {
      (cur->inst)->ioEvent->notify();
    }
  }
}	// ioHandler()


// ----------------------------------------------------------------------------
//! Native read from the xterm.

//! We'll only be calling this if we already know there is something to read,
//! so blocking call is fine.

//! @return The character read. NULL if there was an error - only detected by
//!         a rude message on the console!
// ----------------------------------------------------------------------------
unsigned char
TermSC::xtermRead()
{
  // Have we an xterm?
  if( ptSlave < 0 ) {
    std::cerr << "TermSC: No channel available for read" << std::endl;
    return -1;
  }

  // Use a blocking read! No need to select, since that was done in the IO
  // event handler

  char  ch;

  if( read( ptSlave, &ch, 1 ) != 1 ) {
    perror( "TermSC: Error on read" );
    return  0;
  }
  else {
    return  ch;
  }
}	// xtermRead()


// ----------------------------------------------------------------------------
//! Native write to the xterm.

//! Errors are ignored, apart from a rude message on the console.

//! @param ch  The character to be written
// ----------------------------------------------------------------------------
void
TermSC::xtermWrite( unsigned char  ch )
{
  // Have we an xterm?
  if( ptSlave < 0 ) {
    std::cerr << "TermSC: No channel available for write" << std::endl;
    return;
  }

  // Blocking write should be fine. Note any error!
  if( write( ptSlave, &ch, 1 ) != 1 ) {
    std::cerr << "TermSC: Error on write" << std::endl;
  }
}	// xtermWrite()


// EOF
