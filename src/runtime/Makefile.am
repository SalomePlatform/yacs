
include $(top_srcdir)/adm/unix/make_begin.am

SUBDIRS = Test

lib_LTLIBRARIES = libYACSRuntimeSALOME.la

libYACSRuntimeSALOME_la_SOURCES =      \
	CORBACORBAConv.cxx             \
	CORBANode.cxx                  \
	CORBAPorts.cxx                 \
	CORBAPythonConv.cxx            \
	CORBAXMLConv.cxx               \
	CppNode.cxx                    \
	PythonCORBAConv.cxx            \
	PythonNode.cxx                 \
	PythonPorts.cxx                \
	RuntimeSALOME.cxx              \
	TypeConversions.cxx            \
	XMLCORBAConv.cxx               \
	XMLNode.cxx                    \
	XMLPorts.cxx                   \
	$(__dummy__)

EXTRA_libYACSRuntimeSALOME_la_SOURCES =  \
	$(__dummy__)

libYACSRuntimeSALOME_la_LIBADD = ../bases/libYACSBases.la

AM_CXXFLAGS = $(THREAD_DEF) \
               $(PYTHON_CPPFLAGS) \
               $(OMNIORB_INCLUDES) \
               $(OMNIORB_CCXFLAGS) \
              -I$(srcdir)/../bases \
              -I$(srcdir)/../engine \
              -I/usr/include/libxml2

include $(top_srcdir)/adm/unix/make_end.am
