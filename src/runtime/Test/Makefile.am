
include $(top_srcdir)/adm/unix/make_begin.am

check_SCRIPTS = runtimeTest.sh

check_PROGRAMS = TestRuntime echoSrv

IDL_FILES = echo.idl
IDL_SOURCES = echoSK.cc
BUILT_SOURCES = $(IDL_SOURCES) echo_idl.py

TESTS = runtimeTest.sh
TESTS_ENVIRONMENT=$(SHELL) -x

TestRuntime_SOURCES = \
	TestRuntime.cxx \
	runtimeTest.cxx

TestRuntime_LDADD = \
	$(OMNIORB_LIBS) \
	$(PYTHON_LDFLAGS) \
	$(PYTHON_EXTRA_LIBS) \
	../libYACSRuntimeSALOME.la \
	../../engine/libYACSEngine.la \
	../../bases/libYACSBases.la


TestRuntime_LDFLAGS = \
	@CPPUNIT_LIBS@ -pthread -ldl -lxml2

TestRuntime_CXXFLAGS = \
	$(CPPUNIT_INCLUDES) \
	$(PYTHON_CPPFLAGS) \
        $(OMNIORB_INCLUDES) \
        $(OMNIORB_CCXFLAGS) \
	-I$(srcdir)/.. \
	-I$(srcdir)/../../bases \
	-I$(srcdir)/../../bases/Test \
	-I$(srcdir)/../../engine \
        -I/usr/include/libxml2


echoSrv_SOURCES = echoSrv.cxx $(IDL_SOURCES)

echoSrv_CXXFLAGS = \
	$(OMNIORB_INCLUDES) \
	$(OMNIORB_CXXFLAGS) 

echoSrv_LDFLAGS = \
	$(OMNIORB_LIBS)

include $(top_srcdir)/adm/unix/make_end.am
