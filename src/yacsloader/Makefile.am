# Copyright (C) 2006-2011  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

include $(top_srcdir)/adm/unix/make_begin.am

if CPPUNIT_IS_OK
SUBDIRS = Test
endif

bin_PROGRAMS=driver debugger resume

lib_LTLIBRARIES = libYACSloader.la

libYACSloader_la_SOURCES =      \
	parsers.cxx parserBase.cxx dataParsers.cxx typeParsers.cxx \
	propertyParsers.cxx containerParsers.cxx xmlrpcParsers.cxx \
	rootParser.cxx codeParsers.cxx \
	LoadState.cxx  xmlParserBase.cxx ProcCataLoader.cxx   \
	componentinstanceParsers.cxx \
	$(__dummy__)

salomeinclude_HEADERS = \
YACSloaderExport.hxx \
blocParsers.hxx  codeParsers.hxx      containerParsers.hxx  dataParsers.hxx    factory.hxx \
linkParsers.hxx  LoadState.hxx        loopParsers.hxx       nodeParsers.hxx    outputParsers.hxx \
parserBase.hxx   parsers.hxx          portParsers.hxx       presetParsers.hxx  ProcCataLoader.hxx \
procParsers.hxx  propertyParsers.hxx  rootParser.hxx        switchParsers.hxx  inlineParsers.hxx \
componentinstanceParsers.hxx          remoteParsers.hxx     serverParsers.hxx  serviceParsers.hxx \
typeParsers.hxx  xmlParserBase.hxx    xmlrpcParsers.hxx     sinlineParsers.hxx

EXTRA_libYACSloader_la_SOURCES =  \
	$(__dummy__)

libYACSloader_la_LIBADD = ../engine/libYACSlibEngine.la
libYACSloader_la_LDFLAGS = $(OMNIORB_LIBS) $(EXPAT_LIBS)

AM_CXXFLAGS = \
							$(THREAD_DEF) \
              $(PYTHON_CPPFLAGS) \
              $(OMNIORB_INCLUDES) \
              $(OMNIORB_CXXFLAGS) \
							$(EXPAT_INCLUDES) \
              -I$(srcdir) \
              -I$(srcdir)/../bases \
              -I$(srcdir)/../engine

if SALOME_KERNEL
SALOME_LIBS=$(KERNEL_LDFLAGS) -lSalomeLifeCycleCORBA -lSalomeIDLKernel -lSalomeNS -lOpUtil \
            -lSalomeDSCContainer -lSalomeContainer -lSALOMEBasics -lSalomeResourcesManager \
            -lSALOMELocalTrace -lRegistry -lSalomeNotification -lResourcesManager \
            -lSalomeHDFPersist
SALOME_INCL_PATH=-I$(KERNEL_ROOT_DIR)/include/salome
endif


driver_SOURCES = driver.cxx 

driver_CXXFLAGS = -g -DYACS_PTHREAD \
              $(PYTHON_CPPFLAGS) \
							$(SALOME_INCL_PATH) \
              $(OMNIORB_INCLUDES) \
              $(OMNIORB_CXXFLAGS) \
              $(EXPAT_INCLUDES) \
              -I$(srcdir)/../bases \
              -I$(srcdir)/../engine \
              -I$(srcdir)/../runtime

driver_LDADD = libYACSloader.la \
               ../runtime/libYACSRuntimeSALOME.la \
               ../engine/libYACSlibEngine.la \
               $(SALOME_LIBS) \
               $(OMNIORB_LIBS) \
               $(PYTHON_LDFLAGS) \
               $(PYTHON_EXTRA_LIBS) 

driver_LDFLAGS = $(PYTHON_EXTRA_LDFLAGS) -pthread $(LIBXML_LIBS) $(EXPAT_LIBS)


resume_SOURCES = resume.cxx

resume_CXXFLAGS = -g -DYACS_PTHREAD \
              $(PYTHON_CPPFLAGS) \
              $(OMNIORB_INCLUDES) \
              $(OMNIORB_CXXFLAGS) \
              $(EXPAT_INCLUDES) \
              -I$(srcdir)/../bases \
              -I$(srcdir)/../engine \
              -I$(srcdir)/../runtime

resume_LDADD = libYACSloader.la \
               ../runtime/libYACSRuntimeSALOME.la \
               ../engine/libYACSlibEngine.la \
               $(SALOME_LIBS) \
               $(OMNIORB_LIBS) \
               $(PYTHON_LDFLAGS) \
               $(PYTHON_EXTRA_LIBS) 

resume_LDFLAGS = $(PYTHON_EXTRA_LDFLAGS) -pthread $(LIBXML_LIBS) $(EXPAT_LIBS)


debugger_SOURCES = debugger.cxx 

debugger_CXXFLAGS = -g -DYACS_PTHREAD \
              $(PYTHON_CPPFLAGS) \
              $(OMNIORB_INCLUDES) \
              $(OMNIORB_CXXFLAGS) \
              $(EXPAT_INCLUDES) \
              -I$(srcdir)/../bases \
              -I$(srcdir)/../engine \
              -I$(srcdir)/../runtime

debugger_LDADD = libYACSloader.la \
               ../runtime/libYACSRuntimeSALOME.la \
               ../engine/libYACSlibEngine.la \
               $(SALOME_LIBS) \
               $(OMNIORB_LIBS) \
               $(PYTHON_LDFLAGS) \
               $(PYTHON_EXTRA_LIBS) 

debugger_LDFLAGS = $(PYTHON_EXTRA_LDFLAGS) -pthread $(LIBXML_LIBS) $(EXPAT_LIBS)

EXTRA_DIST = samples schema.xsd

dist-hook:
	rm -rf `find $(distdir) -name CVS`

install-data-local:
	$(INSTALL) -d $(DESTDIR)$(prefix)/share/salome/yacssamples
	cp -rf $(srcdir)/samples/*.xml $(DESTDIR)$(prefix)/share/salome/yacssamples
	cp -rf $(srcdir)/samples/*.data $(DESTDIR)$(prefix)/share/salome/yacssamples

uninstall-local:
	rm -rf $(DESTDIR)$(prefix)/share/salome/yacssamples

include $(top_srcdir)/adm/unix/make_end.am
