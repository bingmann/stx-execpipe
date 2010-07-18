# $Id$

noinst_PROGRAMS = test_ringbuffer test_execpipe test_segfault

TESTS = test_ringbuffer test_execpipe

test_ringbuffer_SOURCES = test_ringbuffer.cc

test_execpipe_SOURCES = test_execpipe.cc

test_segfault_SOURCES = test_segfault.cc

AM_CXXFLAGS = -g -W -Wall -Wno-old-style-cast -I$(top_srcdir)/src

LDADD = $(top_builddir)/src/libstx-execpipe.a