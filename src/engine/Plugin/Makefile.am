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

lib_LTLIBRARIES = libPluginSimplex.la

libPluginSimplex_la_SOURCES = PluginSimplex.cxx \
	maestro.cxx decode.cxx point.cxx aleas.cxx \
	salomevent.cxx simplex.cxx solution.cxx \
        local.cxx mt19937ar.cxx 

libPluginSimplex_la_CXXFLAGS = $(THREAD_DEF) \
	-I$(srcdir)/..                       \
	-I$(srcdir)/../../bases

libPluginSimplex_la_LIBADD = ../libYACSlibEngine.la

EXTRA_DIST = \
aleas.hxx     critere.hxx    decode.hxx         distrib.hxx  fonction.hxx \
local.hxx     maestro.hxx    PluginSimplex.hxx  point.hxx    saclass.hxx \
saemul.hxx    salomesup.hxx  salomevent.hxx     sasimpl.hxx  simplex.hxx \
solution.hxx  saconst.h

include $(top_srcdir)/adm/unix/make_end.am
