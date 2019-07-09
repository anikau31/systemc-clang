# OR32 test progs Makefile.am for automake

# Copyright (C) 2010 Embecosm Limited

# Contributor Jeremy Bennett <jeremy.bennett@embecosm.com>

# This file is part of the example programs for "Building a Loosely Timed SoC
# Model with OSCI TLM 2.0"

# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the Free
# Software Foundation; either version 3 of the License, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
# more details.

# You should have received a copy of the GNU General Public License along
# with this program.  If not, see <http://www.gnu.org/licenses/>.  */

# -----------------------------------------------------------------------------
# This code is commented throughout for use with Doxygen.
# -----------------------------------------------------------------------------

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = hello          \
	       logger-test    \
	       uart-loop      \
	       uart-loop-intr

noinst_LTLIBRARIES = libboot.la

EXTRA_DIST = default.ld

# The boot up library
libboot_la_SOURCES = bitutils.c \
		     start.s    \
		     utils.c

# Hello world program
hello_SOURCES = hello.c \
	        utils.h

hello_LDADD   = ./libboot.la

hello_LDFLAGS = -T$(srcdir)/default.ld

# Generic I/O program
logger_test_SOURCES = logger-test.c \
	 	      utils.h

logger_test_LDADD   = ./libboot.la

logger_test_LDFLAGS = -T$(srcdir)/default.ld

# UART loop test
uart_loop_SOURCES = uart-loop.c \
		    utils.h

uart_loop_LDADD   = ./libboot.la

uart_loop_LDFLAGS = -T$(srcdir)/default.ld

# UART loop test with interrupts
uart_loop_intr_SOURCES = uart-loop-intr.c \
		    	 utils.h

uart_loop_intr_LDADD   = ./libboot.la

uart_loop_intr_LDFLAGS = -T$(srcdir)/default.ld

# Doxygen description is always regenerated.
doxygen:
	sed < $(srcdir)/doxygen.config > ./doxygen.config \
		-e "s/\$$(srcdir)/$(srcdir)/g"
	doxygen ./doxygen.config

clean-local:
	-$(RM) -r doxygen-doc
