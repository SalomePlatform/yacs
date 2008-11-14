
include $(top_srcdir)/adm/unix/make_begin.am

lib_LTLIBRARIES = libHMI.la

# Implementation files
LIBSOURCES = \
	commands.cxx \
	commandsProc.cxx \
	guiObservers.cxx \
	guiContext.cxx

# Headers
LIBHEADERS = 



libHMI_la_SOURCES = $(LIBSOURCES) $(LIBHEADERS)
libHMI_la_LDFLAGS =

libHMI_la_LIBADD = \
	../runtime/libYACSRuntimeSALOME.la \
	../yacsloader/libYACSloader.la \
	$(EXPAT_LIBS) $(LIBXML_LIBS)

libHMI_la_CXXFLAGS = \
	$(THREAD_DEF) \
	$(PYTHON_CPPFLAGS) \
	$(OMNIORB_INCLUDES) \
	-I$(KERNEL_ROOT_DIR)/include/salome \
	-I$(srcdir)/../bases \
	-I$(srcdir)/../engine \
	-I$(srcdir)/../runtime \
	-I$(srcdir)/../yacsloader \
	-I../../idl \
	-I../yacsorb 

EXTRA_DIST = \
commands.hxx      commandsProc.hxx \
guiContext.hxx  guiObservers.hxx  


include $(top_srcdir)/adm/unix/make_end.am
