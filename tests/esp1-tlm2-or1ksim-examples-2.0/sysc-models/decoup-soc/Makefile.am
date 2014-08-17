# Decoupled SoC SystemC models Makefile.am for automake

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

# Build a complete program and a library for subsequent programs
bin_PROGRAMS              = decoup-soc
noinst_LTLIBRARIES        = libdecoup-soc.la
include_HEADERS           = Or1ksimDecoupSC.h \
		            UartDecoupSC.h

# Sources for the program. We set the CXX flags as a workaround for name
# clashing between program and library. It is a no-op effectively. See the
# automake manual for an explanation.
decoup_soc_SOURCES        = decoupSocMainSC.cpp \
		            Or1ksimDecoupSC.cpp \
		            Or1ksimDecoupSC.h   \
		            UartDecoupSC.cpp    \
		            UartDecoupSC.h

decoup_soc_LDADD          = ../sync-soc/libsync-soc.la

decoup_soc_CPPFLAGS       = -I$(srcdir)/../logger     \
			    -I$(srcdir)/../simple-soc \
			    -I$(srcdir)/../sync-soc

decoup_soc_CXXFLAGS       = $(AM_CXXFLAGS)

# Sources for the library
libdecoup_soc_la_SOURCES  = Or1ksimDecoupSC.cpp \
		            Or1ksimDecoupSC.h   \
		            UartDecoupSC.cpp       \
		            UartDecoupSC.h

libdecoup_soc_la_LIBADD   = ../sync-soc/libsync-soc.la

libdecoup_soc_la_CPPFLAGS = -I$(srcdir)/../logger     \
			    -I$(srcdir)/../simple-soc \
			    -I$(srcdir)/../sync-soc
