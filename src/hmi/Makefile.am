
include $(top_srcdir)/adm/unix/make_begin.am

bin_PROGRAMS = guidemo

guidemo_SOURCES = \
	main.cxx \
	editTree.cxx \
	editCanvas.cxx \
	nodeEdition.cxx \
	mainempty_moc.cxx \
	wiedittree_moc.cxx \
	winodeedition_moc.cxx \
	editTree_moc.cxx \
	editCanvas_moc.cxx \
	nodeEdition_moc.cxx


lib_LTLIBRARIES = libHMI.la

# Implementation files
LIBSOURCES = \
	commands.cxx \
	commandsProc.cxx \
	guiObservers.cxx \
	guiContext.cxx

# Headers
LIBHEADERS = 

# MOC-generated files
MOCSOURCES =

# UIC-generated files

mainempty.cxx: mainempty.h
wiedittree.cxx: wiedittree.h
winodeedition.cxx: winodeedition.h

UISOURCES = \
	mainempty.cxx \
	wiedittree.cxx \
	winodeedition.cxx 

# Resources
LIBPOFILES = 

LIBICONS = \
	editcopy.png \
	editcut.png \
	editpaste.png \
	filenew.png \
	fileopen.png \
	filesave.png \
	print.png \
	redo.png \
	searchfind.png \
	undo.png


# Add "resources" subdirectory to resource file names
POFILES = $(LIBPOFILES:%=resources/%)
ICONS = $(LIBICONS:%=resources/%)

libHMI_la_SOURCES = $(LIBSOURCES) $(LIBHEADERS)
nodist_libHMI_la_SOURCES = $(UISOURCES) $(MOCSOURCES)

# List all generated files here
BUILT_SOURCES = $(UISOURCES) $(MOCSOURCES) mainempty.h


libHMI_la_LIBADD = \
	../runtime/libYACSRuntimeSALOME.la \
	../yacsloader/libYACSloader.la \
	../yacsorb/libYACSorb.la \
	@QT_MT_LIBS@ \
	-lexpat -lxml2

libHMI_la_CXXFLAGS = \
	$(THREAD_DEF) \
	$(PYTHON_CPPFLAGS) \
	-I$(KERNEL_ROOT_DIR)/include/salome \
	-I$(srcdir)/../bases \
	-I$(srcdir)/../engine \
	-I$(srcdir)/../runtime \
	-I$(srcdir)/../yacsloader \
	-I../../idl \
	-I../yacsorb \
	@QT_INCLUDES@

guidemo_CXXFLAGS = $(libHMI_la_CXXFLAGS)

guidemo_LDADD = libHMI.la

include $(top_srcdir)/adm/unix/make_end.am
