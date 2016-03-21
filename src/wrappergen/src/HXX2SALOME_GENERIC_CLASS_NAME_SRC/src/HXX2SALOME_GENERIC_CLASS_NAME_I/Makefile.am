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

include $(top_srcdir)/adm_local/unix/make_common_starter.am

USE_MED=0

AM_CXXFLAGS = -fPIC @OMNIORB_INCLUDES@ @OMNIORB_CXXFLAGS@ -DXXX  -I${top_builddir}/idl -I${top_builddir}/adm_local/unix -I ${KERNEL_ROOT_DIR}/include/salome HXX2SALOME_INCLUDE

lib_LTLIBRARIES=libHXX2SALOME_GENERIC_CLASS_NAMEEngine.la
libHXX2SALOME_GENERIC_CLASS_NAMEEngine_la_SOURCES=HXX2SALOME_GENERIC_CLASS_NAME_i.cxx
libHXX2SALOME_GENERIC_CLASS_NAMEEngine_la_LIBADD=-L${top_builddir}/idl -lHXX2SALOME_GENERIC_CLASS_NAMESK HXX2SALOME_LIB

