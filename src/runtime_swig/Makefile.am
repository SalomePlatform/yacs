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

salomepython_PYTHON = SALOMERuntime.py
salomepyexec_LTLIBRARIES = _SALOMERuntime.la

_SALOMERuntime_la_SOURCES =   \
	SALOMERuntimeWRAP.cxx

MYSWIG_FLAGS = -noexcept -I$(srcdir)/../bases -I$(srcdir)/../engine -I$(srcdir)/../runtime \
                         -I$(srcdir)/../engine_swig -I../engine_swig

RUNTIMEINCLUDES= RuntimeSALOME.hxx SALOMEDispatcher.hxx SalomeProc.hxx PythonNode.hxx PythonPorts.hxx \
							CORBANode.hxx CORBAPorts.hxx TypeConversions.hxx

SWIGINCLUDES = $(RUNTIMEINCLUDES:%=$(top_srcdir)/src/runtime/%) 

SALOMERuntimeWRAP.cxx: SALOMERuntime.i $(SWIGINCLUDES) docruntime.i ../engine_swig/pilot.i ../engine_swig/engtypemaps.i
	$(SWIG) $(SWIG_PYTHON_OPT) $(SWIG_PYTHON_INCLUDES) $(MYSWIG_FLAGS) -o $@ $(srcdir)/SALOMERuntime.i

SALOMERuntime.py : SALOMERuntimeWRAP.cxx

_SALOMERuntime_la_CXXFLAGS = \
	$(THREAD_DEF) \
	$(PYTHON_CPPFLAGS) \
	$(OMNIORB_INCLUDES) \
	$(OMNIORB_CXXFLAGS) \
	-I$(KERNEL_ROOT_DIR)/include/salome \
	-I$(srcdir)/../bases \
	-I$(srcdir)/../engine \
	-I$(srcdir)/../runtime \
	-I../../idl \
	$(LIBXML_INCLUDES)

_SALOMERuntime_la_LDFLAGS  = -module

_SALOMERuntime_la_LIBADD = ../runtime/libYACSRuntimeSALOME.la 

docruntime.i:$(SWIGINCLUDES)
	(cd $(srcdir)/../runtime;doxygen ../../Misc/Doxyfile)
	python $(top_srcdir)/Misc/doxy2swig.py -n $(srcdir)/../runtime/xml/index.xml docruntime.i;rm -rf $(srcdir)/../runtime/xml

EXTRA_DIST = SALOMERuntime.i

clean-local:
	rm -f SALOMERuntime.py*  SALOMERuntimeWRAP.cxx

include $(top_srcdir)/adm/unix/make_end.am
