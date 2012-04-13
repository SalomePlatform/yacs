# Copyright (C) 2006-2012  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
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

include $(top_srcdir)/adm/unix/make_begin.am

SUBDIRS = Test

SWIG_PYTHON_INCLUDES = -noexcept -DYACS_PTHREAD -I$(srcdir)/../bases -I$(srcdir)/../engine_swig -I$(srcdir)/../engine \
                                 -I$(srcdir)/../yacsloader -I../engine_swig

BUILT_SOURCES = loaderWRAP.cxx 

MYSWIG_FLAGS =

if DOXYGEN_IS_OK
MYSWIG_FLAGS += -DDOXYGEN_IS_OK
endif

LOADERINCLUDES=parsers.hxx LoadState.hxx xmlParserBase.hxx ProcCataLoader.hxx 

SWIGINCLUDES = $(LOADERINCLUDES:%=$(top_srcdir)/src/yacsloader/%)

loaderWRAP.cxx loaderWRAP.h: loader.i $(SWIGINCLUDES) docyacsloader.i ../engine_swig/pilot.i ../engine_swig/engtypemaps.i
	$(SWIG) $(SWIG_PYTHON_OPT) $(SWIG_PYTHON_INCLUDES) $(MYSWIG_FLAGS) -o loaderWRAP.cxx $(srcdir)/loader.i

loader.py : loaderWRAP.cxx

salomepython_PYTHON = loader.py
salomepyexec_LTLIBRARIES = _loader.la

_loader_la_SOURCES =   \
	loaderWRAP.cxx

_loader_la_CXXFLAGS = \
	$(THREAD_DEF) \
	$(PYTHON_CPPFLAGS) \
	$(OMNIORB_INCLUDES) \
	$(OMNIORB_CXXFLAGS) \
        $(EXPAT_INCLUDES) \
	-I$(srcdir)/../bases \
	-I$(srcdir)/../engine \
	-I$(srcdir)/../yacsloader

_loader_la_LDFLAGS  = -module 

_loader_la_LIBADD =  ../yacsloader/libYACSloader.la $(OMNIORB_LIBS) $(PYTHON_LIBS)

docyacsloader.i:$(SWIGINCLUDES)
	DOXSRCDIR=$(top_srcdir)/src/yacsloader doxygen $(top_srcdir)/Misc/Doxyfile
	$(PYTHON) $(top_srcdir)/Misc/doxy2swig.py -n xml/index.xml docyacsloader.i

EXTRA_DIST = loader.i

clean-local:
	rm -f loader.py*  loaderWRAP.cxx docyacsloader.i loaderWRAP.h
	rm -rf xml

include $(top_srcdir)/adm/unix/make_end.am
