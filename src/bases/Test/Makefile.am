
include $(top_srcdir)/adm/unix/make_begin.am

AM_CXXFLAGS = $(THREAD_DEF) -I$(srcdir)/..

check_PROGRAMS = test1

test1_SOURCES = test1.cxx

test1_LDADD = ../libYACSBases.la

test1_LDFLAGS = $(CPPUNIT_LIBS) -pthread -ldl

test1_CXXFLAGS = $(THREAD_DEF) $(CPPUNIT_INCLUDES) -I$(srcdir)/..

TESTS = test1

include $(top_srcdir)/adm/unix/make_end.am
