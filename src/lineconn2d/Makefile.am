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

lib_LTLIBRARIES = libLineConn2d.la

# Implementation files
LIBSOURCES = \
	LineConn2d.cxx \
        LineConn2d_Box.cxx \
        LineConn2d_BoxTree.cxx \
        LineConn2d_Connection.cxx \
        LineConn2d_IntervalBuffer.cxx \
        LineConn2d_Model.cxx \
        LineConn2d_Object.cxx \
        LineConn2d_Path.cxx \
        LineConn2d_PathIterator.cxx \
	LineConn2d_Segment.cxx

# Headers
LIBHEADERS = \
	LineConn2d.h \
        LineConn2d_Box.h \
	LineConn2d_BoxTree.h \
        LineConn2d_Connection.h \
        LineConn2d_IntervalBuffer.h \
        LineConn2d_Model.h \
        LineConn2d_Object.h \
        LineConn2d_Path.h \
        LineConn2d_PathIterator.h \
        LineConn2d_Port.h \
        LineConn2d_Segment.h \
        LineConn2d_SegmentIterator.h \
        LineConn2d_ZInterval.h \
        LineConn2dAfx.h    


# MOC-generated files
MOCSOURCES = 

# Resources
LIBPOFILES = 

LIBICONS = 

# Add "resources" subdirectory to resource file names
POFILES += $(LIBPOFILES)
ICONS += $(LIBICONS)

libLineConn2d_la_SOURCES = $(LIBSOURCES) $(LIBHEADERS)
nodist_libLineConn2d_la_SOURCES = $(MOCSOURCES)

# List all generated files here
BUILT_SOURCES = $(MOCSOURCES)

libLineConn2d_la_LIBADD = @CAS_KERNEL@
libLineConn2d_la_CXXFLAGS = @CAS_CPPFLAGS@

include $(top_srcdir)/adm/unix/make_end.am
