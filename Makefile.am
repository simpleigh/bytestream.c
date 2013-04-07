## Process this file with automake to produce Makefile.in

include_HEADERS = include/bs.h
lib_LTLIBRARIES = libbs.la
libbs_la_SOURCES = include/bs.h    \
                   src/bs_alloc.h  \
                   src/bs_alloc.c  \
                   src/bs.c        \
                   src/bs_string.c \
                   src/bs_hex.c    \
                   src/bs_base64.c

warnings:
	$(MAKE) V=0 CFLAGS='-g -O -Wall -Wextra -std=c89 -pedantic -Werror'

TESTS = bs_alloc bs bs_string
check_PROGRAMS = $(TESTS)

bs_alloc_SOURCES = tests/bs_alloc.c include/bs.h src/bs_alloc.h
bs_alloc_CFLAGS = @CHECK_CFLAGS@
bs_alloc_LDADD = libbs.la @CHECK_LIBS@

bs_SOURCES = tests/bs.c include/bs.h
bs_CFLAGS = @CHECK_CFLAGS@
bs_LDADD = libbs.la @CHECK_LIBS@

bs_string_SOURCES = tests/bs_string.c include/bs.h
bs_string_CFLAGS = @CHECK_CFLAGS@
bs_string_LDADD = libbs.la @CHECK_LIBS@

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -I$(srcdir)/include

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
