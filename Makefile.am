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


# Main library
include_HEADERS = include/bs.h
lib_LTLIBRARIES = libbs.la
libbs_la_SOURCES = include/bs.h           \
                   lib/bs_internal.h      \
                   lib/alloc.h            \
                   lib/alloc.c            \
                   lib/bs.c               \
                   lib/stream.c           \
                   lib/encodings.h        \
                   lib/encodings.c        \
                   lib/encodings/hex.c    \
                   lib/encodings/base64.c \
                   lib/walk.c             \
                   lib/combine.c          \
                   lib/accumulate.c       \
                   lib/compare.c


# CHECK target, compiles and runs unit tests
TESTS = test_alloc      \
        test_bs         \
        test_stream     \
        test_encodings  \
        test_hex        \
        test_base64     \
        test_walk       \
        test_combine    \
        test_accumulate \
        test_compare

check_PROGRAMS = $(TESTS)

test_alloc_SOURCES = tests/alloc.c
test_alloc_CFLAGS = @CHECK_CFLAGS@
test_alloc_LDADD = libbs.la @CHECK_LIBS@

test_bs_SOURCES = tests/bs.c
test_bs_CFLAGS = @CHECK_CFLAGS@
test_bs_LDADD = libbs.la @CHECK_LIBS@

test_stream_SOURCES = tests/stream.c
test_stream_CFLAGS = @CHECK_CFLAGS@
test_stream_LDADD = libbs.la @CHECK_LIBS@

test_encodings_SOURCES = tests/encodings.c
test_encodings_CFLAGS = @CHECK_CFLAGS@
test_encodings_LDADD = libbs.la @CHECK_LIBS@

test_hex_SOURCES = tests/encodings/hex.c
test_hex_CFLAGS = @CHECK_CFLAGS@
test_hex_LDADD = libbs.la @CHECK_LIBS@

test_base64_SOURCES = tests/encodings/base64.c
test_base64_CFLAGS = @CHECK_CFLAGS@
test_base64_LDADD = libbs.la @CHECK_LIBS@

test_walk_SOURCES = tests/walk.c
test_walk_CFLAGS = @CHECK_CFLAGS@
test_walk_LDADD = libbs.la @CHECK_LIBS@

test_combine_SOURCES = tests/combine.c
test_combine_CFLAGS = @CHECK_CFLAGS@
test_combine_LDADD = libbs.la @CHECK_LIBS@

test_accumulate_SOURCES = tests/accumulate.c
test_accumulate_CFLAGS = @CHECK_CFLAGS@
test_accumulate_LDADD = libbs.la @CHECK_LIBS@

test_compare_SOURCES = tests/compare.c
test_compare_CFLAGS = @CHECK_CFLAGS@
test_compare_LDADD = libbs.la @CHECK_LIBS@

# COVERAGE target, cleans everything out, compiles and runs tests, builds report
coverage:
	$(MAKE) V=0 clean
	$(MAKE) V=0 CFLAGS='-g -fprofile-arcs -ftest-coverage'
	$(MAKE) V=0 check
	gcov --object-directory .libs lib/*
# TODO can we make the above line less fragile?


# COVERAGE-CLEAN target
coverage-clean:
	-rm *.gcov
	-rm *.gcno
	-rm .libs/*.gcno
	-rm .libs/*.gcda


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


# WARNINGS target, compiles with strict compiler settings
warnings:
	$(MAKE) V=0 CFLAGS='-g -O -Wall -Wextra -std=c89 -pedantic -Werror'
