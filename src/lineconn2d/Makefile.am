
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
        LineConn2d_ZInterval.h

# MOC-generated files
MOCSOURCES = 

# Resources
LIBPOFILES = 

LIBICONS = 

# Add "resources" subdirectory to resource file names
POFILES = $(LIBPOFILES)
ICONS = $(LIBICONS)

libLineConn2d_la_SOURCES = $(LIBSOURCES) $(LIBHEADERS)
nodist_libLineConn2d_la_SOURCES = $(MOCSOURCES)

# List all generated files here
BUILT_SOURCES = $(MOCSOURCES)

libLineConn2d_la_LIBADD = @CAS_KERNEL@
libLineConn2d_la_CXXFLAGS = @CAS_CPPFLAGS@

include $(top_srcdir)/adm/unix/make_end.am
