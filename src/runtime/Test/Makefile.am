#  Copyright (C) 2006-2011  CEA/DEN, EDF R&D
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

include $(top_srcdir)/adm/unix/make_begin.am

check_SCRIPTS = runtimeTest.sh

if CPPUNIT_IS_OK
check_PROGRAMS = TestRuntime
else
check_PROGRAMS =
endif
check_PROGRAMS += runtimeTestEchoSrv echo_clt

lib_LTLIBRARIES = libTestComponentLocal.la

IDL_FILES = echo.idl
IDL_SOURCES = echoSK.cc echoDynSK.cc 
BUILT_SOURCES = $(IDL_SOURCES) echo_idl.py xmlrun.sh echo.hh

EXTRA_DIST = $(IDL_FILES) standaloneTest.sh runtimeTest.hxx runtimeTest.sh TestComponent.hxx xmlrun_orig.sh

TESTS = runtimeTest.sh
TESTS_ENVIRONMENT=LD_LIBRARY_PATH=@KERNEL_ROOT_DIR@/lib/salome:$$LD_LIBRARY_PATH $(SHELL) -x 

SALOME_INCL_PATH=-I$(KERNEL_ROOT_DIR)/include/salome

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
	$(SALOME_INCL_PATH) \
	-I.. \
	-I$(srcdir)/.. \
	-I$(srcdir)/../../bases \
	-I$(srcdir)/../../bases/Test \
	-I$(srcdir)/../../engine \
	-I$(srcdir)/../../engine/Test \
	$(LIBXML_INCLUDES) \
	-DLOCATION="\"@prefix@\"" -DYACS_PTHREAD

xmlrun.sh:${srcdir}/xmlrun_orig.sh
	cp -f $(srcdir)/xmlrun_orig.sh xmlrun.sh

echoDynSK.cc echoSK.cc echo.hh:echo.idl
	$(OMNIORB_IDL) $(OMNIORB_IDLCXXFLAGS) -bcxx -Wba $<

runtimeTestEchoSrv_SOURCES = echoSrv.cxx 
nodist_runtimeTestEchoSrv_SOURCES = $(IDL_SOURCES)

runtimeTestEchoSrv_CXXFLAGS = \
	-I. \
	-I$(srcdir)/../../bases \
	$(OMNIORB_INCLUDES) \
	$(OMNIORB_CXXFLAGS) 

runtimeTestEchoSrv_LDFLAGS = \
	$(OMNIORB_LIBS)

echo_clt_SOURCES = echo_clt.cxx 
nodist_echo_clt_SOURCES = $(IDL_SOURCES)

echo_clt_CXXFLAGS = \
	-I. \
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

libTestComponentLocal_la_LIBADD = \
	../../engine/libYACSlibEngine.la

clean-local:
	rm -rf echo_idl.py* echoSK.cc echoDynSK.cc echo.hh eo eo__POA yacs* omnilog lib omniorb.cfg  traceExec_*  xmlrun.sh UnitTestsResult

include $(top_srcdir)/adm/unix/make_end.am
