# Top level autoconf configuration file

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
# with this program.  If not, see <http:#www.gnu.org/licenses/>.  */

# -----------------------------------------------------------------------------
# This code is commented throughout for use with Doxygen.
# -----------------------------------------------------------------------------

# Process this file with autoconf to produce a configure script.

AC_INIT([esp1-tlm2-or1ksim-examples], [2.0], [jeremy.bennett@embecosm.com])
AC_CONFIG_MACRO_DIR([m4])

# Generically use extensions such as _GNU_SOURCE if available.
AC_USE_SYSTEM_EXTENSIONS

# Separate config file for the OpenRISC test programs, since they'll need to
# use the cross compiling tool chain, rather than the native tools chain.
AC_CONFIG_SUBDIRS([progs-or32])

AC_CANONICAL_TARGET
AC_CANONICAL_HOST
AC_PROG_LIBTOOL
AM_INIT_AUTOMAKE

AC_CONFIG_HEADERS([config.h])

AC_COPYRIGHT(Copyright (C) 2010 Embecosm Limited)
AC_REVISION([$Id: configure.ac 1008 2010-05-31 16:15:07Z jeremy $])

# Make sure we are using a reasonably recent autoconf version
AC_PREREQ(2.63)

# We're using C++
AC_LANG([C++])

# C++ compiler and install utility
AC_PROG_CXX
AC_PROG_INSTALL

# This is GNU compliant source and uses GNU libraries
AC_DEFINE(_GNU_SOURCE, 1, "The source code uses the GNU libraries)

# Options to explicitly specify the location of the SystemC world and the TLM
# world. Deal with SystemC's OS naming convention (linux, not linux-gnu)
if test "x$host_os" == "xlinux-gnu"
then
  sysc_os=linux
else
  sysc_os=$host_os
fi

AC_ARG_WITH([systemc],
            [AS_HELP_STRING([--with-systemc=ARG],
              [specify the SystemC installation explicitly (otherwise use $SYSTEMC)])],
            [],
            [with_systemc=$SYSTEMC])
if test x"$with_systemc" = x
then
  AC_MSG_FAILURE(with-systemc not specified and \$SYSTEMC not defined)
else
  CPPFLAGS="$CPPFLAGS -I$with_systemc/include"
  LDFLAGS="$LDFLAGS -L$with_systemc/lib-$sysc_os"
fi

AC_ARG_WITH([tlm],
            [AS_HELP_STRING([--with-tlm=ARG],
              [specify the SystemC TLM 2.0 installation explicitly (otherwise use $TLM_HOME)])],
            [],
            [with_tlm=$TLM_HOME])
if test x"$with_tlm" = x
then
  AC_MSG_FAILURE(with-tlm not specified and \$TLM_HOME not defined)
else
  CPPFLAGS="$CPPFLAGS -DSC_INCLUDE_DYNAMIC_PROCESSES -I$with_tlm/include/tlm"
fi

# Option to specify the location of the Or1ksim installation directory.
AC_ARG_WITH([or1ksim],
            [AS_HELP_STRING([--with-or1ksim=ARG],
              [specify the Or1ksim installation explicitly (otherwise use $OR1KSIM_HOME)])],
            [],
            [with_or1ksim=$OR1KSIM_HOME])
if test x"$with_or1ksim" = x
then
  AC_MSG_FAILURE(with-or1ksim not specified and \$OR1KSIM_HOME not defined)
else
  CPPFLAGS="$CPPFLAGS -I$with_or1ksim/include"
  LDFLAGS="$LDFLAGS -L$with_or1ksim/lib"
fi

# Check for lbraries.

# For the SystemC library. We can't use AC_SEARCH_LIBS, because the namespace
# declaration will confuse it!
LIBS="$LIBS -lsystemc"
AC_LINK_IFELSE([AC_LANG_PROGRAM([#include <systemc>], \
                                [sc_core::sc_start ();])], , \
	       [AC_MSG_FAILURE(SystemC library not found)])

# Check for the Or1ksim library
AC_SEARCH_LIBS(or1ksim_init, sim, ,
	       [AC_MSG_FAILURE(Or1ksim library not found)])

# Check the headers. For now we'll assume that if we have the headers, we have
# the functions (no AC_CHECK_FUNCS). Headers from C++, C, SystemC and TLM 2.0
AC_CHECK_HEADERS(cerrno csignal cstddef cstdio cstdlib cstring iomanip \
		 iostream, , [AC_MSG_FAILURE(C++ headers missing)])

AC_CHECK_HEADERS(arpa/inet.h inttypes.h netdb.h netinet/in.h netinet/tcp.h \
		 poll.h stdint.h sys/socket.h, , \
		 [AC_MSG_FAILURE(C headers missing)])

AC_CHECK_HEADERS(systemc, , [AC_MSG_FAILURE(SystemC header missing)])

AC_CHECK_HEADERS(tlm.h, , \
		 [AC_MSG_FAILURE(SystemC TLM 2.0 main header missing)])

AC_CHECK_HEADERS(tlm_utils/simple_initiator_socket.h \
                 tlm_utils/simple_target_socket.h, , \
		 [AC_MSG_FAILURE(SystemC TLM 2.0 socket headers missing)],
		 [AC_INCLUDES_DEFAULT()
                  #include <tlm.h>])

AC_CHECK_HEADERS(or1ksim.h, , [AC_MSG_FAILURE(Or1ksim header missing)])

# check for uint{8,16,32,64}_t in inttypes.h.
AC_CHECK_TYPES([uint8_t, uint16_t, uint32_t, uint64_t], , \
	       [AC_MSG_FAILURE(fixed unsigned types not available)], \
	       [#include <inttypes.h>])

# Add extra warnings
CXXFLAGS="$CXXFLAGS -Wall -Werror -ggdb"

# Explicitly substitute the variables we have changed
AC_SUBST(CPPFLAGS)
AC_SUBST(CXXFLAGS)
AC_SUBST(LDFLAGS)
AC_SUBST(LIBS)

# We want to create a Makefile
AC_CONFIG_FILES([Makefile                        \
                 sysc-models/Makefile            \
		 sysc-models/logger/Makefile     \
		 sysc-models/simple-soc/Makefile \
		 sysc-models/sync-soc/Makefile   \
		 sysc-models/decoup-soc/Makefile \
		 sysc-models/intr-soc/Makefile   \
		 sysc-models/jtag-soc/Makefile])

# Put it all out.
AC_OUTPUT
