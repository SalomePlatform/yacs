
include $(top_srcdir)/adm/unix/make_begin.am

AM_CXXFLAGS = $(THREAD_DEF) -I$(srcdir)/..

check_PROGRAMS = InitTests TestBases

lib_LTLIBRARIES = libYACSDLTest.la

libYACSDLTest_la_SOURCES = DLTest.cxx

libYACSDLTest_la_CXXFLAGS = $(THREAD_DEF)

InitTests_SOURCES = InitTests.cxx

InitTests__CXXFLAGS =  -I$(srcdir)/..

TestBases_SOURCES = \
	TestBases.cxx \
	basesTest.cxx

TestBases_LDADD = ../libYACSBases.la

TestBases_LDFLAGS = $(CPPUNIT_LIBS) -pthread -ldl

TestBases_CXXFLAGS = $(THREAD_DEF) $(CPPUNIT_INCLUDES) -I$(srcdir)/..

TESTS = InitTests TestBases 

EXTRA_DIST = BasicMainTest.hxx  basesTest.hxx  UnitTestsResult.hxx

include $(top_srcdir)/adm/unix/make_end.am
