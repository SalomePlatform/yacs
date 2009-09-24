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

SUBDIRS = Test



lib_LTLIBRARIES = libYACSRuntimeSALOME.la

libYACSRuntimeSALOME_la_SOURCES =      \
	TypeConversions.cxx            \
	CORBACORBAConv.cxx             \
	CORBAPythonConv.cxx            \
	CORBAXMLConv.cxx               \
	CORBANeutralConv.cxx           \
	XMLCORBAConv.cxx               \
	XMLPythonConv.cxx              \
	XMLNeutralConv.cxx             \
	CORBANode.cxx                  \
	CORBAPorts.cxx                 \
	CppNode.cxx                    \
	CppPorts.cxx                   \
	NeutralPythonConv.cxx          \
	NeutralCORBAConv.cxx           \
	NeutralXMLConv.cxx             \
	NeutralInitConv.cxx            \
	PythonCORBAConv.cxx            \
	PythonNeutralConv.cxx          \
	PythonXMLConv.cxx              \
	PythonInitConv.cxx             \
	PythonNode.cxx                 \
	SalomePythonNode.cxx           \
	SalomePythonComponent.cxx      \
	SalomeContainer.cxx            \
	PythonPorts.cxx                \
	XMLNode.cxx                    \
	XMLPorts.cxx                   \
	RuntimeSALOME.cxx              \
	SALOMEDispatcher.cxx           \
	SalomeProc.cxx                 \
	CalStreamPort.cxx              \
	CORBAComponent.cxx             \
	SalomeComponent.cxx            \
	CppComponent.cxx               \
	CppContainer.cxx               \
	CppCORBAConv.cxx               \
	CppNeutralConv.cxx             \
	CppXMLConv.cxx                 \
	CppCppConv.cxx                 \
	CppPythonConv.cxx              \
	CORBACppConv.cxx               \
	NeutralCppConv.cxx             \
	XMLCppConv.cxx                 \
	PythonCppConv.cxx              \
	SessionCataLoader.cxx          \
	PresetPorts.cxx                \
	PresetNode.cxx                 \
	OutNode.cxx                    \
	StudyNodes.cxx                 \
	StudyPorts.cxx                 \
	VisitorSaveSalomeSchema.cxx    \
	PyStdout.cxx                   \
	$(__dummy__)

salomeinclude_HEADERS = \
YACSRuntimeSALOMEExport.hxx \
CalStreamPort.hxx            CORBAComponent.hxx   CORBACORBAConv.hxx         CORBACppConv.hxx \
CORBANeutralConv.hxx         CORBANode.hxx        CORBAPorts.hxx             CORBAPythonConv.hxx \
CORBAXMLConv.hxx             CppComponent.hxx     CppContainer.hxx           CppCORBAConv.hxx \
CppCppConv.hxx               CppNeutralConv.hxx   CppNode.hxx                CppPorts.hxx \
CppPythonConv.hxx            CppXMLConv.hxx       NeutralCORBAConv.hxx       NeutralCppConv.hxx \
NeutralPythonConv.hxx        NeutralXMLConv.hxx   OutNode.hxx                PresetNode.hxx \
PresetPorts.hxx              PyStdout.hxx         PythonCORBAConv.hxx        PythonCppConv.hxx \
PythonNeutralConv.hxx        PythonNode.hxx       PythonPorts.hxx            PythonXMLConv.hxx \
RuntimeSALOME.hxx            SalomeComponent.hxx  SALOMEconfig.h             SalomeContainer.hxx \
SALOMEDispatcher.hxx         SalomeProc.hxx       SalomePythonComponent.hxx  SalomePythonNode.hxx \
SessionCataLoader.hxx        StudyNodes.hxx       StudyPorts.hxx             TypeConversions.hxx \
VisitorSaveSalomeSchema.hxx  XMLCORBAConv.hxx     XMLCppConv.hxx             XMLNeutralConv.hxx \
XMLNode.hxx                  XMLPorts.hxx         XMLPythonConv.hxx          PythonInitConv.hxx \
NeutralInitConv.hxx

EXTRA_libYACSRuntimeSALOME_la_SOURCES =  \
	$(__dummy__)

if SALOME_KERNEL
nodist_libYACSRuntimeSALOME_la_SOURCES = ../../idl/yacsguiSK.cc
SALOME_LIBS=$(KERNEL_LDFLAGS) -lSalomeLifeCycleCORBA -lSalomeIDLKernel -lSalomeNS -lSalomeContainer -lSALOMEBasics
SALOME_INCL_PATH=-I$(KERNEL_ROOT_DIR)/include/salome 
endif

if DSC_PORTS
DSC_LIBS=$(KERNEL_LDFLAGS) -lSalomeDSCContainer
endif

# --- build libYACSRuntimeSALOME library before swig libraries (problem with make -j)

BUILT_SOURCES=SALOMERuntimeWRAP.cxx libYACSRuntimeSALOME.la

libYACSRuntimeSALOME_la_LIBADD = \
	$(DSC_LIBS) $(SALOME_LIBS) $(OMNIORB_LIBS) $(PYTHON_LIBS) $(LIBXML_LIBS) \
	../engine/libYACSlibEngine.la

AM_CXXFLAGS = \
	$(THREAD_DEF) \
	$(PYTHON_CPPFLAGS) \
	$(OMNIORB_INCLUDES) \
	-I$(OMNIORB_ROOT)/include/omniORB4/internal \
	$(OMNIORB_CXXFLAGS) \
	$(SALOME_INCL_PATH) \
	-I$(srcdir)/../bases \
	-I$(srcdir)/../engine \
	-I../../idl \
	$(LIBXML_INCLUDES)

############################################################

salomepython_PYTHON = SALOMERuntime.py
salomepyexec_LTLIBRARIES = _SALOMERuntime.la

_SALOMERuntime_la_SOURCES =   \
	SALOMERuntimeWRAP.cxx

MYSWIG_FLAGS = -noexcept -I$(srcdir)/../bases -I$(srcdir)/../engine

SWIGINCLUDES= RuntimeSALOME.hxx SALOMEDispatcher.hxx SalomeProc.hxx PythonNode.hxx PythonPorts.hxx \
							CORBANode.hxx CORBAPorts.hxx TypeConversions.hxx \
							../engine/pilot.i ../engine/engtypemaps.i

SALOMERuntimeWRAP.cxx: SALOMERuntime.i $(SWIGINCLUDES)
	$(SWIG) $(SWIG_PYTHON_OPT) $(SWIG_PYTHON_INCLUDES) $(MYSWIG_FLAGS) -o $@ $(srcdir)/SALOMERuntime.i

SALOMERuntime.py : SALOMERuntimeWRAP.cxx

_SALOMERuntime_la_CXXFLAGS = \
	$(THREAD_DEF) \
	$(PYTHON_CPPFLAGS) \
	$(OMNIORB_INCLUDES) \
	$(OMNIORB_CXXFLAGS) \
	-I$(KERNEL_ROOT_DIR)/include/salome \
	-I$(srcdir) \
	-I$(srcdir)/../bases \
	-I$(srcdir)/../engine \
	-I../../idl \
	$(LIBXML_INCLUDES)

_SALOMERuntime_la_LDFLAGS  = -module

_SALOMERuntime_la_LIBADD = libYACSRuntimeSALOME.la 

EXTRA_DIST = SALOMERuntime.i

include $(top_srcdir)/adm/unix/make_end.am
