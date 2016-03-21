# Copyright (C) 2006-2016  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
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

# Common begin part
#
include $(top_srcdir)/adm/unix/make_begin.am

#
# Developper part
#

lib_LTLIBRARIES = libCpp_Template_SWIG.la
nodist_libCpp_Template_SWIG_la_SOURCES = Cpp_Template__wrap.cxx 

BUILT_SOURCES = Cpp_Template__wrap.cxx

SWIG_PYTHON_INCLUDES = $(MED_INCLUDES) -I$(top_srcdir)/src/Cpp_Template_/Cpp_Template__CXX

INCLUDES = -I$(top_srcdir)/src/Cpp_Template_/Cpp_Template__CXX $(PYTHON_INCLUDES) $(MED_INCLUDES)

libCpp_Template_SWIG_la_LIBADD = -L../Cpp_Template__CXX -lCpp_Template_CXX $(MED_LIBS) -lMEDMEM_Swigcmodule


bin_SCRIPTS = Cpp_Template_SWIG.py

clean-local:
	$(RM) Cpp_Template_SWIG.py Cpp_Template__wrap.cxx

#
# Common end part
#

include $(top_srcdir)/adm/unix/make_end.am
