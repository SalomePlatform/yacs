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

lib_LTLIBRARIES            = libCpp_Template_CXX.la
libCpp_Template_CXX_la_SOURCES  = Cpp_Template_.cxx
libCpp_Template_CXX_la_LIBADD   = $(MED_LIBS)

# exported headers
library_includedir=$(includedir)
library_include_HEADERS = Cpp_Template_.hxx
			
INCLUDES  = $(MED_INCLUDES)


bin_PROGRAMS = Cpp_Template__test
Cpp_Template__test_SOURCES = main.cxx
Cpp_Template__test_LDADD   = -L. -lCpp_Template_CXX 


#
# Common end part
#

include $(top_srcdir)/adm/unix/make_end.am


