
include $(top_srcdir)/adm/unix/make_begin.am

SUBDIRS = Test

bin_PROGRAMS=driver debugger resume

lib_LTLIBRARIES = libYACSloader.la

libYACSloader_la_SOURCES =      \
	parsers.cxx parserBase.cxx dataParsers.cxx typeParsers.cxx \
	propertyParsers.cxx containerParsers.cxx xmlrpcParsers.cxx \
	rootParser.cxx codeParsers.cxx \
	LoadState.cxx  xmlParserBase.cxx ProcCataLoader.cxx   \
	$(__dummy__)

salomeinclude_HEADERS = \
blocParsers.hxx  codeParsers.hxx      containerParsers.hxx  dataParsers.hxx    factory.hxx \
linkParsers.hxx  LoadState.hxx        loopParsers.hxx       nodeParsers.hxx    outputParsers.hxx \
parserBase.hxx   parsers.hxx          portParsers.hxx       presetParsers.hxx  ProcCataLoader.hxx \
procParsers.hxx  propertyParsers.hxx  rootParser.hxx        switchParsers.hxx \
typeParsers.hxx  xmlParserBase.hxx    xmlrpcParsers.hxx

EXTRA_libYACSloader_la_SOURCES =  \
	$(__dummy__)

libYACSloader_la_LIBADD = ../engine/libYACSEngine.la
libYACSloader_la_LDFLAGS = $(EXPAT_LIBS)

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
SALOME_LIBS=-L$(KERNEL_ROOT_DIR)/lib/salome -lSalomeLifeCycleCORBA
SALOME_INCL_PATH=-I$(KERNEL_ROOT_DIR)/include/salome
endif


driver_SOURCES = driver.cxx 

driver_CXXFLAGS = -g -DYACS_PTHREAD \
              $(PYTHON_CPPFLAGS) \
							$(SALOME_INCL_PATH) \
              $(OMNIORB_INCLUDES) \
              $(OMNIORB_CXXFLAGS) \
              -I$(srcdir)/../bases \
              -I$(srcdir)/../engine \
              -I$(srcdir)/../runtime

driver_LDADD = libYACSloader.la \
               ../runtime/libYACSRuntimeSALOME.la \
               ../engine/libYACSEngine.la \
               $(SALOME_LIBS) \
               $(OMNIORB_LIBS) \
               $(PYTHON_LDFLAGS) \
               $(PYTHON_EXTRA_LIBS) 

driver_LDFLAGS = $(PYTHON_EXTRA_LDFLAGS) -pthread $(EXPAT_LIBS) $(LIBXML_LIBS)


resume_SOURCES = resume.cxx

resume_CXXFLAGS = -g -DYACS_PTHREAD \
              $(PYTHON_CPPFLAGS) \
              $(OMNIORB_INCLUDES) \
              $(OMNIORB_CXXFLAGS) \
              -I$(srcdir)/../bases \
              -I$(srcdir)/../engine \
              -I$(srcdir)/../runtime

resume_LDADD = libYACSloader.la \
               ../runtime/libYACSRuntimeSALOME.la \
               ../engine/libYACSEngine.la \
               $(SALOME_LIBS) \
               $(OMNIORB_LIBS) \
               $(PYTHON_LDFLAGS) \
               $(PYTHON_EXTRA_LIBS) 

resume_LDFLAGS = $(PYTHON_EXTRA_LDFLAGS) -pthread $(EXPAT_LIBS) $(LIBXML_LIBS)


debugger_SOURCES = debugger.cxx 

debugger_CXXFLAGS = -g -DYACS_PTHREAD \
              $(PYTHON_CPPFLAGS) \
              $(OMNIORB_INCLUDES) \
              $(OMNIORB_CXXFLAGS) \
              -I$(srcdir)/../bases \
              -I$(srcdir)/../engine \
              -I$(srcdir)/../runtime

debugger_LDADD = libYACSloader.la \
               ../runtime/libYACSRuntimeSALOME.la \
               ../engine/libYACSEngine.la \
               $(SALOME_LIBS) \
               $(OMNIORB_LIBS) \
               $(PYTHON_LDFLAGS) \
               $(PYTHON_EXTRA_LIBS) 

debugger_LDFLAGS = $(PYTHON_EXTRA_LDFLAGS) -pthread $(EXPAT_LIBS) $(LIBXML_LIBS)


SWIG_PYTHON_INCLUDES = -noexcept -I$(srcdir)/../bases -I$(srcdir)/../engine
BUILT_SOURCES = loaderWRAP.cxx

SWIGINCLUDES=parsers.hxx LoadState.hxx xmlParserBase.hxx ProcCataLoader.hxx ../engine/engtypemaps.i ../engine/pilot.i

loaderWRAP.cxx loaderWRAP.h: loader.i $(SWIGINCLUDES)
	$(SWIG) $(SWIG_PYTHON_OPT) $(SWIG_PYTHON_INCLUDES) $(MYSWIG_FLAGS) -o loaderWRAP.cxx $(srcdir)/loader.i

salomepython_PYTHON = loader.py
salomepyexec_LTLIBRARIES = _loader.la

_loader_la_SOURCES =   \
	loaderWRAP.cxx

_loader_la_CXXFLAGS = \
	$(THREAD_DEF) \
	$(PYTHON_CPPFLAGS) \
	$(OMNIORB_INCLUDES) \
	$(OMNIORB_CXXFLAGS) \
	-I$(srcdir)/../bases \
	-I$(srcdir)/../engine

_loader_la_LDFLAGS  = -module 

_loader_la_LIBADD =  libYACSloader.la $(OMNIORB_LIBS)

EXTRA_DIST = loader.i samples

dist-hook:
	rm -rf `find $(distdir) -name CVS`

include $(top_srcdir)/adm/unix/make_end.am
