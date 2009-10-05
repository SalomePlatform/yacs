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


SWIGSRC=libYACS_Swig.i
MYSWIG_FLAGS =

salomepython_PYTHON = libYACS_Swig.py
salomepyexec_LTLIBRARIES = _libYACS_Swig.la

libYACS_Swig.py: swig_wrap.cpp
_libYACS_Swig_la_SOURCES        =  $(SWIGSRC) YACSGUI_Swig.cxx
nodist__libYACS_Swig_la_SOURCES = swig_wrap.cpp

_libYACS_Swig_la_CPPFLAGS =   -I$(KERNEL_ROOT_DIR)/include/salome \
                              -I$(GUI_ROOT_DIR)/include/salome \
                              -I$(srcdir)/../salomegui \
                              -I$(srcdir)/../salomewrap \
                              $(qt4_cppflags) $(THREAD_DEF) $(PYTHON_CPPFLAGS) $(OMNIORB_INCLUDES)


_libYACS_Swig_la_LDFLAGS = -module
_libYACS_Swig_la_LIBADD  = ../salomegui/libYACS.la $(PYTHON_LIBS)

swig_wrap.cpp : $(SWIGSRC) YACSGUI_Swig.hxx doc.i
	$(SWIG) $(SWIG_PYTHON_OPT) $(SWIG_PYTHON_INCLUDES) $(MYSWIG_FLAGS) -o $@ $<

doc.i:YACSGUI_Swig.hxx YACSGUI_Swig.cxx
	(cd $(srcdir);doxygen ../../Misc/Doxyfile)
	python $(top_srcdir)/Misc/doxy2swig.py -n $(srcdir)/xml/index.xml doc.i; rm -rf $(srcdir)/xml

CLEANFILES = swig_wrap.cpp libYACS_Swig.py doc.i

include $(top_srcdir)/adm/unix/make_end.am

