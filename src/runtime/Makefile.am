
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
	PythonCORBAConv.cxx            \
	PythonNeutralConv.cxx          \
	PythonXMLConv.cxx              \
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
  $(__dummy__)


EXTRA_libYACSRuntimeSALOME_la_SOURCES =  \
	$(__dummy__)

if SALOME_KERNEL
libYACSRuntimeSALOME_la_SOURCES += ../../idl/yacsguiSK.cc

SALOME_IDL = \
	$(KERNEL_ROOT_DIR)/idl/salome/SALOME_ContainerManager.idl \
	$(KERNEL_ROOT_DIR)/idl/salome/SALOME_Component.idl \
	$(KERNEL_ROOT_DIR)/idl/salome/SALOME_Exception.idl

libYACSRuntimeSALOME_la_SOURCES += \
	SALOME_GenericObjSK.cc \
	SALOME_GenericObjDynSK.cc \
	SALOME_ComponentSK.cc \
	SALOME_ComponentDynSK.cc \
	SALOME_ContainerManagerSK.cc \
	SALOME_ContainerManagerDynSK.cc \
	SALOME_ExceptionSK.cc \
	SALOME_ExceptionDynSK.cc


SALOME_INCLUDES =SALOME_ContainerManager.hh SALOME_Exception.hh SALOME_Component.hh SALOME_GenericObj.hh
SALOME_LIBS=-L$(KERNEL_ROOT_DIR)/lib@LIB_LOCATION_SUFFIX@/salome -lSalomeLifeCycleCORBA
SALOME_INCL_PATH=-I$(KERNEL_ROOT_DIR)/include/salome $(QT_INCLUDES)
SALOME_OMNIPY=
endif

if DSC_PORTS
DSC_IDL = \
	$(KERNEL_ROOT_DIR)/idl/salome/DSC_Engines.idl \
	$(KERNEL_ROOT_DIR)/idl/salome/SALOME_Ports.idl

libYACSRuntimeSALOME_la_SOURCES += \
	DSC_EnginesSK.cc \
	DSC_EnginesDynSK.cc \
	SALOME_PortsSK.cc \
	SALOME_PortsDynSK.cc

DSC_INCLUDES = DSC_Engines.hh SALOME_Ports.hh
DSC_LIBS=-L$(KERNEL_ROOT_DIR)/lib@LIB_LOCATION_SUFFIX@/salome -lSalomeDSCContainer
DSC_INCL_PATH=-I$(KERNEL_ROOT_DIR)/include
DSC_OMNIPY=DSC_Engines_idl.py
endif

IDL_FILES=$(DSC_IDL) $(SALOME_IDL)
BUILT_SOURCES=$(DSC_INCLUDES) $(SALOME_INCLUDES) SALOMERuntimeWRAP.cxx

libYACSRuntimeSALOME_la_LIBADD = \
	$(DSC_LIBS) $(SALOME_LIBS) $(OMNIORB_LIBS) \
	../engine/libYACSEngine.la

AM_CXXFLAGS = \
	$(THREAD_DEF) \
	$(PYTHON_CPPFLAGS) \
	$(OMNIORB_INCLUDES) \
	-I$(OMNIORB_ROOT)/include/omniORB4/internal \
	$(OMNIORB_CXXFLAGS) \
	$(SALOME_INCL_PATH) \
	$(DSC_INCL_PATH) \
	-I$(srcdir)/../bases \
	-I$(srcdir)/../engine \
	-I../../idl \
	-I/usr/include/libxml2



############################################################

if SALOME_KERNEL
SALOME_Component.hh:$(KERNEL_ROOT_DIR)/idl/salome/SALOME_Component.idl
	    $(OMNIORB_IDL) $(OMNIORB_IDLCXXFLAGS) -bcxx -Wba -I$(KERNEL_ROOT_DIR)/idl/salome $<
SALOME_Exception.hh:$(KERNEL_ROOT_DIR)/idl/salome/SALOME_Exception.idl
	    $(OMNIORB_IDL) $(OMNIORB_IDLCXXFLAGS) -bcxx -Wba -I$(KERNEL_ROOT_DIR)/idl/salome $<
SALOME_ContainerManager.hh:$(KERNEL_ROOT_DIR)/idl/salome/SALOME_ContainerManager.idl
	    $(OMNIORB_IDL) $(OMNIORB_IDLCXXFLAGS) -bcxx -Wba -I$(KERNEL_ROOT_DIR)/idl/salome $<
SALOME_GenericObj.hh:$(KERNEL_ROOT_DIR)/idl/salome/SALOME_GenericObj.idl
	    $(OMNIORB_IDL) $(OMNIORB_IDLCXXFLAGS) -bcxx -Wba -I$(KERNEL_ROOT_DIR)/idl/salome $<

SALOME_Component_idl.py:$(KERNEL_ROOT_DIR)/idl/salome/SALOME_Component.idl
	    $(OMNIORB_IDL) -bpython -Wba -I. -I$(KERNEL_ROOT_DIR)/idl/salome $<
endif

if DSC_PORTS
DSC_Engines.hh:$(KERNEL_ROOT_DIR)/idl/salome/DSC_Engines.idl
	    $(OMNIORB_IDL) $(OMNIORB_IDLCXXFLAGS) -bcxx -Wba -I$(KERNEL_ROOT_DIR)/idl $<
SALOME_Ports.hh:$(KERNEL_ROOT_DIR)/idl/salome/SALOME_Ports.idl
	    $(OMNIORB_IDL) $(OMNIORB_IDLCXXFLAGS) -bcxx -Wba -I$(KERNEL_ROOT_DIR)/idl $<

SALOME_Ports_idl.py :$(KERNEL_ROOT_DIR)/idl/salome/SALOME_Ports.idl
	    $(OMNIORB_IDL) -bpython -Wba -I. -I$(KERNEL_ROOT_DIR)/idl $<
endif

#################################################################

pkgpython_PYTHON = SALOMERuntime.py
pkgpyexec_LTLIBRARIES = _SALOMERuntime.la

_SALOMERuntime_la_SOURCES =   \
	SALOMERuntimeWRAP.cxx

MYSWIG_FLAGS = -noexcept -I$(srcdir)/../bases -I$(srcdir)/../engine

SWIGINCLUDES= RuntimeSALOME.hxx SALOMEDispatcher.hxx SalomeProc.hxx PythonNode.hxx PythonPorts.hxx \
							CORBANode.hxx CORBAPorts.hxx TypeConversions.hxx \
							../engine/pilot.i ../engine/engtypemaps.i

SALOMERuntimeWRAP.cxx:SALOMERuntime.i $(SWIGINCLUDES)
	$(SWIG) $(SWIG_PYTHON_OPT) $(SWIG_PYTHON_INCLUDES) $(MYSWIG_FLAGS) -o $@ $<

_SALOMERuntime_la_CXXFLAGS = \
	$(THREAD_DEF) \
	$(PYTHON_CPPFLAGS) \
	$(OMNIORB_INCLUDES) \
	$(OMNIORB_CXXFLAGS) \
	-I$(KERNEL_ROOT_DIR)/include/salome \
	-I$(srcdir)/../bases \
	-I$(srcdir)/../engine \
	-I../../idl \
	-I/usr/include/libxml2

_SALOMERuntime_la_LDFLAGS  = -module

_SALOMERuntime_la_LIBADD = libYACSRuntimeSALOME.la 

include $(top_srcdir)/adm/unix/make_end.am
