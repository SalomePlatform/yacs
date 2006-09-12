
include $(top_srcdir)/adm/unix/make_begin.am

SUBDIRS = Test

lib_LTLIBRARIES = libYACSEngine.la

libYACSEngine_la_SOURCES =         \
	Port.cxx InGate.cxx        \
	OutGate.cxx                \
	DataFlowPort.cxx           \
	InPort.cxx                 \
	OutPort.cxx                \
	InputPort.cxx              \
	OutputPort.cxx             \
	DataStreamPort.cxx         \
	InputDataStreamPort.cxx    \
	OutputDataStreamPort.cxx   \
	Node.cxx                   \
	ElementaryNode.cxx         \
	ComposedNode.cxx           \
	Bloc.cxx                   \
	Loop.cxx                   \
	Switch.cxx                 \
	Scheduler.hxx              \
	Task.hxx                   \
	Executor.cxx               \
	$(__dummy__)

EXTRA_libYACSEngine_la_SOURCES =  \
	$(__dummy__)

libYACSEngine_la_LIBADD = ../bases/libYACSBases.la \
                          ../basicData/libYACSBasicData.la 

AM_CXXFLAGS = $(THREAD_DEF)  \
              -I$(srcdir)/../bases \
              -I$(srcdir)/../basicData

include $(top_srcdir)/adm/unix/make_end.am
