#  ________        _____     ____________
#  ___  __ )____  ___  /_______  ___/_  /__________________ _______ ___
#  __  __  |_  / / /  __/  _ \____ \_  __/_  ___/  _ \  __ `/_  __ `__ \
#  _  /_/ /_  /_/ // /_ /  __/___/ // /_ _  /   /  __/ /_/ /_  / / / / /
#  /_____/ _\__, / \__/ \___//____/ \__/ /_/    \___/\__,_/ /_/ /_/ /_/
#          /____/
#
#  Byte stream manipulation library.
#  Copyright (C) 2013  Leigh Simpson <code@simpleigh.com>
#
#  This library is free software; you can redistribute it and/or modify it
#  under the terms of the GNU Lesser General Public License as published by
#  the Free Software Foundation; either version 2.1 of the License, or any
#  later version.
#
#  This library is distributed in the hope that it will be useful, but
#  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
#  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
#  License for more details.
#
#  A copy of the GNU Lesser General Public License is available within
#  COPYING.LGPL; alternatively write to the Free Software Foundation, Inc.,
#  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

## Process this file with automake to produce Makefile.in

# Global configuration
ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -I$(srcdir)/include

# Set these test parameters by default (remove for the main library)
CFLAGS = @CHECK_CFLAGS@
LDADD = libbs.la @CHECK_LIBS@

# Main library
include_HEADERS = include/bs.h
lib_LTLIBRARIES = libbs.la
libbs_la_SOURCES = include/bs.h    \
                   src/bs_alloc.h  \
                   src/bs_alloc.c  \
                   src/bs.c        \
                   src/bs_string.c \
                   src/bs_hex.c    \
                   src/bs_base64.c
libbs_la_CFLAGS =

# WARNINGS target, compiles with strict compiler setting
warnings:
	$(MAKE) V=0 CFLAGS='-g -O -Wall -Wextra -std=c89 -pedantic -Werror'

# CHECK target, compiles and runs unit tests
TESTS = bs_alloc bs bs_string bs_hex bs_base64
check_PROGRAMS = $(TESTS)
bs_alloc_SOURCES = tests/bs_alloc.c include/bs.h src/bs_alloc.h
bs_SOURCES = tests/bs.c include/bs.h
bs_string_SOURCES = tests/bs_string.c include/bs.h
bs_hex_SOURCES = tests/bs_hex.c include/bs.h
bs_base64_SOURCES = tests/bs_base64.c include/bs.h

# DIST target
EXTRA_DIST = autogen

# MAINTAINER-CLEAN target
MAINTAINERCLEANFILES = Makefile.in            \
                       aclocal.m4             \
                       build-aux/config.guess \
                       build-aux/config.sub   \
                       build-aux/depcomp      \
                       build-aux/install-sh   \
                       build-aux/ltmain.sh    \
                       build-aux/missing      \
                       config.h.in            \
                       config.h.in~           \
                       configure
