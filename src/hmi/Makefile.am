
include $(top_srcdir)/adm/unix/make_begin.am

bin_PROGRAMS = guidemo

guidemo_SOURCES = \
	main.cxx \
	editTree.cxx \
	editCanvas.cxx \
	nodeEdition.cxx \
	browseCatalog.cxx \
	chooseName.cxx \
	mainempty_moc.cxx \
	wiedittree_moc.cxx \
	dichoosename_moc.cxx \
	winodeedition_moc.cxx \
	catalog_moc.cxx \
	editTree_moc.cxx \
	editCanvas_moc.cxx \
	nodeEdition_moc.cxx \
	chooseName_moc.cxx \
	browseCatalog_moc.cxx


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
dichoosename.cxx: dichoosename.h
catalog.cxx: catalog.h

UISOURCES = \
	mainempty.cxx \
	wiedittree.cxx \
	winodeedition.cxx \
	dichoosename.cxx \
	catalog.cxx

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
#nodist_libHMI_la_SOURCES = $(UISOURCES) $(MOCSOURCES)
nodist_guidemo_SOURCES = $(UISOURCES) $(MOCSOURCES)

# List all generated files here
BUILT_SOURCES = $(UISOURCES) $(MOCSOURCES) mainempty.h

libHMI_la_LDFLAGS = @QT_MT_LDFLAGS@

libHMI_la_LIBADD = \
	../runtime/libYACSRuntimeSALOME.la \
	../yacsloader/libYACSloader.la \
	../yacsorb/libYACSorb.la \
	@QT_MT_LIBADD@ \
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
	-I../yacsorb \
	@QT_INCLUDES@


guidemo_CPPFLAGS = @QSCINTILLA_CCPFLAGS@

guidemo_CXXFLAGS = \
	$(libHMI_la_CXXFLAGS)

guidemo_LDFLAGS=@QT_MT_LDFLAGS@

guidemo_LDADD = \
	libHMI.la \
	@QSCINTILLA_LIBADD@

include $(top_srcdir)/adm/unix/make_end.am
