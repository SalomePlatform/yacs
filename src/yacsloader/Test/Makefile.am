
include $(top_srcdir)/adm/unix/make_begin.am

check_PROGRAMS = TestYacsLoader echoSrv
bin_PROGRAMS = echoSrv

check_SCRIPTS = \
	YacsLoaderTest.sh \
	YacsLoaderInSessionTest.sh \
	YacsLoaderInSessionTest2.sh \
	display.sh \
	waitContainers.py \
	testExec.py

bin_SCRIPTS = display.sh

TESTS = YacsLoaderTest.sh
TESTS_ENVIRONMENT=$(SHELL) -x

TestYacsLoader_SOURCES = \
	TestYacsLoader.cxx \
	YacsLoaderTest.cxx

TestYacsLoader_LDADD = \
	$(OMNIORB_LIBS) \
	$(PYTHON_LDFLAGS) \
	$(PYTHON_EXTRA_LIBS) \
	../libYACSloader.la \
	../../runtime/libYACSRuntimeSALOME.la \
	../../engine/libYACSEngine.la \
	../../bases/libYACSBases.la

TestYacsLoader_LDFLAGS = \
	@CPPUNIT_LIBS@ -pthread -ldl -lxml2 -lexpat

TestYacsLoader_CXXFLAGS = \
	$(THREAD_DEF) \
	$(CPPUNIT_INCLUDES) \
	$(PYTHON_CPPFLAGS) \
  $(OMNIORB_INCLUDES) \
  $(OMNIORB_CCXFLAGS) \
	-I$(srcdir)/.. \
	-I$(srcdir)/../../bases \
	-I$(srcdir)/../../bases/Test \
	-I$(srcdir)/../../runtime \
	-I$(srcdir)/../../engine \
  -I/usr/include/libxml2

IDL_FILES = echo.idl
IDL_SOURCES = echoSK.cc
BUILT_SOURCES = $(IDL_SOURCES) echo_idl.py xmlrun.sh

xmlrun.sh:xmlrun_orig.sh
	cp $(srcdir)/xmlrun_orig.sh xmlrun.sh

echoSrv_SOURCES = echoSrv.cxx $(IDL_SOURCES)

echoSrv_CXXFLAGS = \
	-I$(srcdir)/../../bases \
	$(OMNIORB_INCLUDES) \
	$(OMNIORB_CXXFLAGS) 

echoSrv_LDFLAGS = \
	$(OMNIORB_LIBS)

AM_CXXFLAGS = $(THREAD_DEF)

install-exec-local: $(IDL_FILES)
	$(INSTALL) -d  $(pkgpythondir)
	ls $^ | while read file; do \
	  $(OMNIORB_IDL) -bpython -C$(pkgpythondir) $$file ; \
	done

check-local:
	cat /tmp/${USER}/UnitTestsResult

include $(top_srcdir)/adm/unix/make_end.am
