# autoconf configuration file for the OR1200 test programs

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

# Configure script for the OpenRISC 1000 test programs. This uses a different
# tool chain, so has its own configuration script. Process this file with
# autoconf to produce a configure script.

AC_INIT([embecosm-eps1-sysc-tlm2.0-or32-progs], [2.0], [jeremy.bennett@embecosm.com])
AC_CONFIG_MACRO_DIR([m4])

AC_PROG_LIBTOOL
AM_INIT_AUTOMAKE

AC_CONFIG_HEADERS([config.h])

AC_COPYRIGHT(Copyright (C) 2010 Embecosm Limited)
AC_REVISION([$Id: configure.ac 1008 2010-05-31 16:15:07Z jeremy $])

# Make sure we are using a reasonably recent autoconf version
AC_PREREQ(2.63)

# We're using C
AC_LANG([C])

# Sort out the tools. Using the target tool chain here, not native. Note that
# AC_CHECK_PROGS will not override an already set variable, so we must
# explicitly unset them first.
unset AR
unset CC
unset RANLIB
unset LD
AC_CHECK_PROGS(AR, [$target_alias-ar])
AC_CHECK_PROGS(CC, [$target_alias-gcc])
AC_CHECK_PROGS(RANLIB, [$target_alias-ranlib])
AC_CHECK_PROGS(LD, [$target_alias-ld])
AC_SUBST(AR)
AC_SUBST(CC)
AC_SUBST(RANLIB)
AC_SUBST(LD)

AC_HEADER_STDC

# We need assembler, installer and per target C flags
AM_PROG_AS
AC_PROG_INSTALL
AM_PROG_CC_C_O

# Specify our baseline CFLAGS
CPPFLAGS=""
CFLAGS="-Wall -Werror -g -nostdlib -mhard-div -mhard-mul"

AC_SUBST(CPPFLAGS)
AC_SUBST(CFLAGS)

# We want to create a Makefile
AC_CONFIG_FILES([Makefile])

# Put it all out.
AC_OUTPUT
