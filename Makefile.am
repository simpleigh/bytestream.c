## Process this file with automake to produce Makefile.in

include_HEADERS = include/bs.h
lib_LTLIBRARIES = libbs.la
libbs_la_SOURCES = include/bs.h    \
                   src/bs.c        \
                   src/bs_alloc.h  \
                   src/bs_alloc.c  \
                   src/bs_string.c \
                   src/bs_hex.c    \
                   src/bs_base64.c

TESTS = bs_alloc
check_PROGRAMS = bs_alloc
bs_alloc_SOURCES = tests/bs_alloc.c include/bs.h
bs_alloc_CFLAGS = @CHECK_CFLAGS@
bs_alloc_LDADD = libbs.la @CHECK_LIBS@

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -I$(srcdir)/include

warnings:
	$(MAKE) V=0 CFLAGS='-g -O -Wall -Wextra -std=c89 -pedantic -Werror'

EXTRA_DIST = autogen

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
