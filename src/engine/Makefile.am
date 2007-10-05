include $(top_srcdir)/adm/unix/make_begin.am

SUBDIRS = Plugin Test

lib_LTLIBRARIES = libYACSEngine.la

libYACSEngine_la_SOURCES =             \
	TypeCode.cxx                   \
	RefCounter.cxx                 \
	LinkInfo.cxx                   \
	ConversionException.cxx        \
	InvalidExtractionException.cxx \
	ConditionInputPort.cxx         \
	AnyInputPort.cxx               \
	Port.cxx InGate.cxx            \
	OutGate.cxx                    \
	DataPort.cxx                   \
	DataFlowPort.cxx               \
	InPort.cxx                     \
	OutPort.cxx                    \
	InputPort.cxx                  \
	OutputPort.cxx                 \
	DataStreamPort.cxx             \
	InputDataStreamPort.cxx        \
	OutputDataStreamPort.cxx       \
	Any.cxx                        \
	Pool.cxx                       \
	Node.cxx                       \
	ElementaryNode.cxx             \
	ComposedNode.cxx               \
	InlineNode.cxx                 \
	ServiceNode.cxx                \
	ServiceInlineNode.cxx          \
	StaticDefinedComposedNode.cxx  \
	Bloc.cxx                       \
	Proc.cxx                       \
	Loop.cxx                       \
	ForLoop.cxx                    \
	WhileLoop.cxx                  \
	Switch.cxx                     \
	DynParaLoop.cxx                \
	ForEachLoop.cxx                \
	OptimizerAlg.cxx               \
	OptimizerLoop.cxx              \
	Runtime.cxx                    \
	Scheduler.hxx                  \
	Task.hxx                       \
	Executor.cxx                   \
	Visitor.cxx                    \
	VisitorSaveState.cxx           \
	VisitorSaveSchema.cxx          \
	ComponentInstance.cxx          \
	Dispatcher.cxx                 \
	Container.cxx                  \
	DeploymentTree.cxx             \
	$(__dummy__)

salomeinclude_HEADERS =                \
             Any.hxx \
             AnyInputPort.hxx \
             Bloc.hxx \
             ComponentInstance.hxx \
             ComposedNode.hxx \
             ConditionInputPort.hxx \
             Container.hxx \
             ConversionException.hxx \
             DataFlowPort.hxx \
             DataPort.hxx \
             DataStreamPort.hxx \
             DeploymentTree.hxx \
             Dispatcher.hxx \
             DynParaLoop.hxx \
             ElementaryNode.hxx \
             Executor.hxx \
             ExecutorSwig.hxx \
             ForEachLoop.hxx \
             ForLoop.hxx \
             InGate.hxx \
             InlineNode.hxx \
             InPort.hxx \
             InputDataStreamPort.hxx \
             InputPort.hxx \
             InvalidExtractionException.hxx \
             LinkInfo.hxx \
             Loop.hxx \
             Node.hxx \
             OptimizerAlg.hxx \
             OptimizerLoop.hxx \
             OutGate.hxx \
             OutPort.hxx \
             OutputDataStreamPort.hxx \
             OutputPort.hxx \
             Pool.hxx \
             Port.hxx \
             Proc.hxx \
             RefCounter.hxx \
             Runtime.hxx \
             Scheduler.hxx \
             ServiceInlineNode.hxx \
             ServiceNode.hxx \
             SharedPtr.hxx \
             StaticDefinedComposedNode.hxx \
             Switch.hxx \
             Task.hxx \
             TypeCode.hxx \
             Visitor.hxx \
             VisitorSaveSchema.hxx \
             VisitorSaveState.hxx \
             WhileLoop.hxx \
	     $(__dummy__)


EXTRA_libYACSEngine_la_SOURCES =  \
	$(__dummy__)

libYACSEngine_la_LIBADD = ../bases/libYACSBases.la 

AM_CXXFLAGS = $(THREAD_DEF)  \
              -I$(srcdir)/../bases


BUILT_SOURCES = pilotWRAP.cxx

MYSWIG_FLAGS = -noexcept -I$(srcdir)/../bases

pkgpython_PYTHON = pilot.py
pkgpyexec_LTLIBRARIES = _pilot.la

pilotWRAP.cxx:pilot.i InlineNode.hxx ServiceNode.hxx
	$(SWIG) $(SWIG_PYTHON_OPT) $(SWIG_PYTHON_INCLUDES) $(MYSWIG_FLAGS) -o $@ $<

_pilot_la_SOURCES =   \
	pilotWRAP.cxx \
	ExecutorSwig.cxx

_pilot_la_CXXFLAGS = \
	$(THREAD_DEF) \
	$(PYTHON_CPPFLAGS) \
	-I$(srcdir)/../bases

_pilot_la_LDFLAGS  = -module

_pilot_la_LIBADD = libYACSEngine.la


include $(top_srcdir)/adm/unix/make_end.am
