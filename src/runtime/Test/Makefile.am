include $(top_srcdir)/adm/unix/make_begin.am

check_SCRIPTS = runtimeTest.sh

check_PROGRAMS = TestRuntime echoSrv echo_clt
lib_LTLIBRARIES = libTestComponentLocal.la

IDL_FILES = echo.idl
IDL_SOURCES = echoSK.cc echoDynSK.cc 
BUILT_SOURCES = $(IDL_SOURCES) echo_idl.py xmlrun.sh echo.hh

EXTRA_DIST = $(IDL_FILES) standaloneTest.sh runtimeTest.hxx runtimeTest.sh TestComponent.hxx xmlrun_orig.sh

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
	../../engine/libYACSlibEngine.la \
	../../bases/libYACSBases.la


TestRuntime_LDFLAGS = \
	@CPPUNIT_LIBS@ -pthread -ldl $(LIBXML_LIBS)

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
	$(LIBXML_INCLUDES) \
	-DLOCATION="\"@prefix@\"" -DYACS_PTHREAD

xmlrun.sh:${srcdir}/xmlrun_orig.sh
	cp $(srcdir)/xmlrun_orig.sh xmlrun.sh

echoDynSK.cc echoSK.cc echo.hh:echo.idl
	$(OMNIORB_IDL) $(OMNIORB_IDLCXXFLAGS) -bcxx -Wba $<

echoSrv_SOURCES = echoSrv.cxx 
nodist_echoSrv_SOURCES = $(IDL_SOURCES)

echoSrv_CXXFLAGS = \
	-I$(srcdir)/../../bases \
	$(OMNIORB_INCLUDES) \
	$(OMNIORB_CXXFLAGS) 

echoSrv_LDFLAGS = \
	$(OMNIORB_LIBS)

echo_clt_SOURCES = echo_clt.cxx 
nodist_echo_clt_SOURCES = $(IDL_SOURCES)

echo_clt_CXXFLAGS = \
	-I$(srcdir)/../../bases \
	$(OMNIORB_INCLUDES) \
	-I$(OMNIORB_ROOT)/include/omniORB4/internal \
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

clean-local:
	rm -rf echo_idl.py* echoSK.cc echoDynSK.cc echo.hh eo eo__POA yacs* omnilog lib omniorb.cfg  traceExec_*  xmlrun.sh

include $(top_srcdir)/adm/unix/make_end.am
