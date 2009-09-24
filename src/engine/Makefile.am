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

SUBDIRS = . Plugin Test

lib_LTLIBRARIES = libYACSlibEngine.la

libYACSlibEngine_la_SOURCES =             \
	PropertyInterface.cxx          \
	TypeCode.cxx                   \
	RefCounter.cxx                 \
	LinkInfo.cxx                   \
	ConversionException.cxx        \
	InvalidExtractionException.cxx \
	ConditionInputPort.cxx         \
	AnyInputPort.cxx               \
	AnyOutputPort.cxx              \
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
	Logger.cxx                     \
	LogRecord.cxx                  \
	Catalog.cxx                    \
	ComponentDefinition.cxx        \
	DataNode.cxx                   \
	$(__dummy__)

salomeinclude_HEADERS =                \
YACSlibEngineExport.hxx \
Any.hxx                  AnyInputPort.hxx  Bloc.hxx                        Catalog.hxx                    ComponentDefinition.hxx \
ComponentInstance.hxx    ComposedNode.hxx  ConditionInputPort.hxx          Container.hxx                  ConversionException.hxx \
DataFlowPort.hxx         DataNode.hxx      DataPort.hxx                    DataStreamPort.hxx             DeploymentTree.hxx \
Dispatcher.hxx           DynParaLoop.hxx   ElementaryNode.hxx              Executor.hxx                   ExecutorSwig.hxx \
ForEachLoop.hxx          ForLoop.hxx       InGate.hxx                      InlineNode.hxx                 InPort.hxx \
InputDataStreamPort.hxx  InputPort.hxx     InvalidExtractionException.hxx  LinkInfo.hxx                   Logger.hxx \
LogRecord.hxx            Loop.hxx          Node.hxx                        OptimizerAlg.hxx               OptimizerLoop.hxx \
OutGate.hxx              OutPort.hxx       OutputDataStreamPort.hxx        OutputPort.hxx                 Pool.hxx \
Port.hxx                 Proc.hxx          RefCounter.hxx                  Runtime.hxx                    Scheduler.hxx \
ServiceInlineNode.hxx    ServiceNode.hxx   SharedPtr.hxx                   StaticDefinedComposedNode.hxx  Switch.hxx \
Task.hxx                 TypeCode.hxx      Visitor.hxx                     VisitorSaveSchema.hxx          VisitorSaveState.hxx \
WhileLoop.hxx            PropertyInterface.hxx      AnyOutputPort.hxx \
	     $(__dummy__)


EXTRA_libYACSlibEngine_la_SOURCES =  \
	$(__dummy__)

libYACSlibEngine_la_LIBADD = ../bases/libYACSBases.la 
libYACSlibEngine_la_LDFLAGS = -pthread -lpthread -ldl

AM_CXXFLAGS = $(THREAD_DEF)  \
              -I$(srcdir)/../bases


# --- build libYACSlibEngine library before swig libraries (problem with make -j)

BUILT_SOURCES = pilotWRAP.cxx pypilotWRAP.cxx pypilotWRAP.h libYACSlibEngine.la

MYSWIG_FLAGS = -noexcept -I$(srcdir)/../bases

salomepython_PYTHON = pilot.py pypilot.py
salomepyexec_LTLIBRARIES = _pilot.la _pypilot.la

SWIGINCLUDES=Any.hxx     DeploymentTree.hxx  InputDataStreamPort.hxx         OutPort.hxx               SharedPtr.hxx \
AnyInputPort.hxx         Dispatcher.hxx      InputPort.hxx                   OutputDataStreamPort.hxx  StaticDefinedComposedNode.hxx \
Bloc.hxx                 DynParaLoop.hxx     InvalidExtractionException.hxx  OutputPort.hxx            Switch.hxx \
ComponentInstance.hxx    ElementaryNode.hxx  LinkInfo.hxx                    Pool.hxx                  Task.hxx \
ComposedNode.hxx         Executor.hxx        Logger.hxx                      Port.hxx                  TypeCode.hxx \
ConditionInputPort.hxx   ExecutorSwig.hxx    LogRecord.hxx                   Proc.hxx                  Visitor.hxx \
Container.hxx            ForEachLoop.hxx     Loop.hxx                        RefCounter.hxx            VisitorSaveSchema.hxx \
ConversionException.hxx  ForLoop.hxx         Node.hxx                        Runtime.hxx               VisitorSaveState.hxx \
DataFlowPort.hxx         InGate.hxx          OptimizerAlg.hxx                Scheduler.hxx             WhileLoop.hxx \
DataPort.hxx             InlineNode.hxx      OptimizerLoop.hxx               ServiceInlineNode.hxx     ComponentDefinition.hxx \
DataStreamPort.hxx       InPort.hxx          OutGate.hxx                     ServiceNode.hxx           Catalog.hxx  \
PropertyInterface.hxx  AnyOutputPort.hxx \
../bases/yacsconfig.h engtypemaps.i

pilotWRAP.cxx:pilot.i $(SWIGINCLUDES)
	$(SWIG) $(SWIG_PYTHON_OPT) $(SWIG_PYTHON_INCLUDES) $(MYSWIG_FLAGS) -o pilotWRAP.cxx $(srcdir)/pilot.i 

pilot.py : pilotWRAP.cxx

_pilot_la_SOURCES =   \
	pilotWRAP.cxx \
	ExecutorSwig.cxx


_pilot_la_CXXFLAGS = \
	$(THREAD_DEF) \
	$(PYTHON_CPPFLAGS) \
	$(OMNIORB_INCLUDES) \
  $(OMNIORB_CXXFLAGS) \
	-I$(srcdir) \
	-I$(srcdir)/../bases

_pilot_la_LDFLAGS  = -module

_pilot_la_LIBADD = libYACSlibEngine.la $(OMNIORB_LIBS) $(PYTHON_LIBS)

pypilotWRAP.cxx pypilotWRAP.h: pypilot.i pilot.i $(SWIGINCLUDES)
	$(SWIG) $(SWIG_PYTHON_OPT) $(SWIG_PYTHON_INCLUDES) $(MYSWIG_FLAGS) -o pypilotWRAP.cxx $(srcdir)/pypilot.i

pypilot.py : pypilotWRAP.cxx 

_pypilot_la_SOURCES =  pypilotWRAP.cxx
_pypilot_la_CXXFLAGS =  $(THREAD_DEF) $(PYTHON_CPPFLAGS) $(OMNIORB_INCLUDES) $(OMNIORB_CXXFLAGS) -I$(srcdir) -I$(srcdir)/../bases
_pypilot_la_LDFLAGS  = -module
_pypilot_la_LIBADD = libYACSlibEngine.la $(OMNIORB_LIBS) $(PYTHON_LIBS)

EXTRA_DIST = pilot.i pypilot.i engtypemaps.i pypilotWRAP.h

clean-local:
	rm -f pypilot.py* pypilotWRAP.*
include $(top_srcdir)/adm/unix/make_end.am
