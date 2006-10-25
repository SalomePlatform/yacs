
include $(top_srcdir)/adm/unix/make_begin.am

AM_CXXFLAGS = $(THREAD_DEF) -I$(srcdir)/..

check_PROGRAMS = TestBases

TestBases_SOURCES = \
	TestBases.cxx \
	basesTest.cxx

TestBases_LDADD = ../libYACSBases.la

TestBases_LDFLAGS = $(CPPUNIT_LIBS) -pthread -ldl

TestBases_CXXFLAGS = $(THREAD_DEF) $(CPPUNIT_INCLUDES) -I$(srcdir)/..

TESTS = TestBases

include $(top_srcdir)/adm/unix/make_end.am
