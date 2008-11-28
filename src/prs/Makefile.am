#  Copyright (C) 2006-2008  CEA/DEN, EDF R&D, OPEN CASCADE
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

lib_LTLIBRARIES = libYACSPrs.la

# Implementation files
LIBSOURCES = \
	YACSPrs_ElementaryNode.cxx \
	YACSPrs_toString.cxx \
	YACSPrs_ServiceNode.cxx \
	YACSPrs_InlineNode.cxx \
	YACSPrs_IfNode.cxx \
	YACSPrs_SwitchNode.cxx \
	YACSPrs_LoopNode.cxx \
	YACSPrs_ForEachLoopNode.cxx \
	YACSPrs_BlocNode.cxx \
	YACSPrs_Link.cxx

# Headers
LIBHEADERS = \
	YACSPrs_Def.h \
	YACSPrs_ElementaryNode.h \
	YACSPrs_toString.h \
	YACSPrs_ServiceNode.h \
	YACSPrs_InlineNode.h \
	YACSPrs_IfNode.h \
	YACSPrs_SwitchNode.h \
	YACSPrs_LoopNode.h \
	YACSPrs_ForEachLoopNode.h \
	YACSPrs_BlocNode.h \
	YACSPrs_Link.h

# MOC-generated files
MOCSOURCES = \
	YACSPrs_Link_moc.cxx

# Resources
LIBPOFILES = \
	YACSPrs_msg_en.po \
	YACSPrs_images.po

LIBICONS = \
	no_status.png \
	ledgreen.png \
	ledred.png \
	disabled.png \
	running.png \
	waiting.png \
	aborted1.png \
	aborted2.png \
	aborted3.png \
	done.png \
	right_arrows.png \
	left_arrows.png

# Add "resources" subdirectory to resource file names
POFILES += $(LIBPOFILES:%=resources/%)
ICONS += $(LIBICONS:%=resources/%)

libYACSPrs_la_SOURCES = $(LIBSOURCES) $(LIBHEADERS)
nodist_libYACSPrs_la_SOURCES = $(MOCSOURCES)

# List all generated files here
BUILT_SOURCES = $(MOCSOURCES)

libYACSPrs_la_LDFLAGS = \
	 @GUI_LDFLAGS@ @PYTHON_LDFLAGS@ @QT_MT_LDFLAGS@

libYACSPrs_la_LIBADD = \
	../engine/libYACSlibEngine.la \
	../runtime/libYACSRuntimeSALOME.la \
	../hmi/libHMI.la \
	@QT_MT_LIBADD@ -lQxGraph $(LIBXML_LIBS)

libYACSPrs_la_CXXFLAGS = \
	$(THREAD_DEF) \
	$(PYTHON_CPPFLAGS) \
	$(OMNIORB_INCLUDES) \
	$(LIBXML_INCLUDES) \
	-I$(KERNEL_ROOT_DIR)/include/salome \
	-I$(srcdir)/../bases \
	-I$(srcdir)/../engine \
	-I$(srcdir)/../runtime \
	-I$(srcdir)/../hmi \
	@QT_INCLUDES@ @GUI_CXXFLAGS@

include $(top_srcdir)/adm/unix/make_end.am
