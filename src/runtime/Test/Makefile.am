include $(top_srcdir)/adm/unix/make_begin.am

check_SCRIPTS = runtimeTest.sh

check_PROGRAMS = TestRuntime echoSrv echo_clt
lib_LTLIBRARIES = libTestComponentLocal.la

IDL_FILES = echo.idl
IDL_SOURCES = echoSK.cc
BUILT_SOURCES = $(IDL_SOURCES) echo_idl.py xmlrun.sh

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
	-DUSE_CPPUNIT \
        $(CPPUNIT_INCLUDES) \
	$(PYTHON_CPPFLAGS) \
        $(OMNIORB_INCLUDES) \
        $(OMNIORB_CXXFLAGS) \
	-I.. \
	-I$(srcdir)/.. \
	-I$(srcdir)/../../bases \
	-I$(srcdir)/../../bases/Test \
	-I$(srcdir)/../../engine \
	-I$(srcdir)/../../engine/Test \
        -I/usr/include/libxml2 \
	-DLOCATION="\"@prefix@\"" -DYACS_PTHREAD

xmlrun.sh:${srcdir}/xmlrun_orig.sh
	cp $(srcdir)/xmlrun_orig.sh xmlrun.sh

echoSrv_SOURCES = echoSrv.cxx $(IDL_SOURCES)

echoSrv_CXXFLAGS = \
	-I$(srcdir)/../../bases \
	$(OMNIORB_INCLUDES) \
	$(OMNIORB_CXXFLAGS) 

echoSrv_LDFLAGS = \
	$(OMNIORB_LIBS)

echo_clt_SOURCES = echo_clt.cxx $(IDL_SOURCES)

echo_clt_CXXFLAGS = \
	$(OMNIORB_INCLUDES) \
	$(OMNIORB_CXXFLAGS) 

echo_clt_LDFLAGS = \
	$(OMNIORB_LIBS)

libTestComponentLocal_la_SOURCES = TestComponent.cxx

libTestComponentLocal_la_CXXFLAGS = \
     -I.. \
	-I$(srcdir)/.. \
	-I$(srcdir)/../../bases \
	-I$(srcdir)/../../bases/Test \
	-I$(srcdir)/../../engine \
	-I$(srcdir)/../../engine/Test

libTestComponentLocal_la_LDFLAGS = -module

include $(top_srcdir)/adm/unix/make_end.am
