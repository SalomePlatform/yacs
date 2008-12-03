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

bin_PROGRAMS = guidemo

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
POFILES += $(LIBPOFILES:%=resources/%)
ICONS += $(LIBICONS:%=resources/%)

libHMI_la_SOURCES = $(LIBSOURCES) $(LIBHEADERS)
#nodist_libHMI_la_SOURCES = $(UISOURCES) $(MOCSOURCES)
libHMI_la_LDFLAGS = @QT_MT_LDFLAGS@

libHMI_la_LIBADD = \
	../runtime/libYACSRuntimeSALOME.la \
	../yacsloader/libYACSloader.la \
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

# List all generated files here
BUILT_SOURCES = $(UISOURCES) $(MOCSOURCES) mainempty.h

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
	browseCatalog_moc.cxx \
  $(UISOURCES) $(MOCSOURCES)

guidemo_CPPFLAGS = @QSCINTILLA_CCPFLAGS@

guidemo_CXXFLAGS = \
	$(libHMI_la_CXXFLAGS)

guidemo_LDFLAGS=@QT_MT_LDFLAGS@

guidemo_LDADD = \
	libHMI.la \
	@QSCINTILLA_LIBADD@

if SALOME_KERNEL
guidemo_LDADD+=-L$(KERNEL_ROOT_DIR)/lib$(LIB_LOCATION_SUFFIX)/salome -lSalomeIDLKernel
endif

EXTRA_DIST = \
browseCatalog.h  chooseName.h    commands.hxx      commandsProc.hxx  editCanvas.h \
editTree.h       guiContext.hxx  guiObservers.hxx  nodeEdition.h \
mainempty.h wiedittree.h winodeedition.h dichoosename.h catalog.h


include $(top_srcdir)/adm/unix/make_end.am
