#  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
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

if SALOME_KERNEL
SALOME_LIBS=-L$(KERNEL_ROOT_DIR)/lib$(LIB_LOCATION_SUFFIX)/salome -lSalomeLifeCycleCORBA
endif

TestYacsLoader_LDADD = \
	$(OMNIORB_LIBS) \
	$(PYTHON_LDFLAGS) \
	$(PYTHON_EXTRA_LIBS) \
	$(SALOME_LIBS) \
	../libYACSloader.la \
	../../runtime/libYACSRuntimeSALOME.la \
	../../engine/libYACSlibEngine.la \
	../../bases/libYACSBases.la

TestYacsLoader_LDFLAGS = \
	@CPPUNIT_LIBS@ -pthread -ldl $(EXPAT_LIBS) $(LIBXML_LIBS)

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
	$(LIBXML_INCLUDES)

IDL_FILES = echo.idl
IDL_SOURCES = echoSK.cc
BUILT_SOURCES = $(IDL_SOURCES) echo_idl.py xmlrun.sh 

EXTRA_DIST = $(IDL_FILES) \
echoclt.py genPascal.py genTriangle.py testEdit.py testExec.py \
testLoader.py        testResume.py       testSave.py   testRefcount.py  \
waitContainers.py    xmlrun_orig.sh      YacsLoaderTest.hxx  

xmlrun.sh:xmlrun_orig.sh
	cp $(srcdir)/xmlrun_orig.sh xmlrun.sh

echoSrv_SOURCES = echoSrv.cxx 
nodist_echoSrv_SOURCES = $(IDL_SOURCES)

echoSrv_CXXFLAGS = \
	-I$(srcdir)/../../bases \
	$(OMNIORB_INCLUDES) \
	$(OMNIORB_CXXFLAGS) 

echoSrv_LDFLAGS = \
	$(OMNIORB_LIBS)

AM_CXXFLAGS = $(THREAD_DEF)

install-exec-local: $(IDL_FILES)
	$(INSTALL) -d  $(DESTDIR)$(salomepythondir)
	ls $^ | while read file; do \
	  $(OMNIORB_IDL) -bpython -C$(DESTDIR)$(salomepythondir) $$file ; \
	done

uninstall-local:
	rm -rf $(DESTDIR)$(salomepythondir)/echo_idl.py
	rm -rf $(DESTDIR)$(salomepythondir)/eo
	rm -rf $(DESTDIR)$(salomepythondir)/eo__POA

clean-local:
	rm -rf yacs* schema1_* schema2_* dump1_* dump2_* eo eo__POA dump*.xml CatalogResources.xml traceExec_*
	rm -f samples toto titi log1 xmlrun.sh echo_idl.py echo.hh echoSK.cc g.data
	if test -f "./bin/salome/appli_clean.sh"; then ./bin/salome/appli_clean.sh ; fi


check-local:
	cat /tmp/${USER}/UnitTestsResult

include $(top_srcdir)/adm/unix/make_end.am
