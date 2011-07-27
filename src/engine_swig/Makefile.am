#  Copyright (C) 2006-2011  CEA/DEN, EDF R&D
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

BUILT_SOURCES = pilotWRAP.cxx pypilotWRAP.cxx pypilotWRAP.h pilotWRAP.h

MYSWIG_FLAGS = -noexcept -DYACS_PTHREAD -I$(srcdir)/../bases -I$(srcdir)/../engine
if DOXYGEN_IS_OK
MYSWIG_FLAGS += -DDOXYGEN_IS_OK
endif

salomepython_PYTHON = pilot.py pypilot.py
salomepyexec_LTLIBRARIES = _pilot.la _pypilot.la

ENGINEINCLUDES=Any.hxx     DeploymentTree.hxx  InputDataStreamPort.hxx         OutPort.hxx               SharedPtr.hxx \
AnyInputPort.hxx         Dispatcher.hxx      InputPort.hxx                   OutputDataStreamPort.hxx  StaticDefinedComposedNode.hxx \
Bloc.hxx                 DynParaLoop.hxx     InvalidExtractionException.hxx  OutputPort.hxx            Switch.hxx \
ComponentInstance.hxx    ElementaryNode.hxx  LinkInfo.hxx                    Pool.hxx                  Task.hxx \
ComposedNode.hxx         Executor.hxx        Logger.hxx                      Port.hxx                  TypeCode.hxx \
ConditionInputPort.hxx   LogRecord.hxx                   Proc.hxx                  Visitor.hxx \
Container.hxx            ForEachLoop.hxx     Loop.hxx                        RefCounter.hxx            VisitorSaveSchema.hxx \
ConversionException.hxx  ForLoop.hxx         Node.hxx                        Runtime.hxx               VisitorSaveState.hxx \
DataFlowPort.hxx         InGate.hxx          OptimizerAlg.hxx                Scheduler.hxx             WhileLoop.hxx \
DataPort.hxx             InlineNode.hxx      OptimizerLoop.hxx               ServiceInlineNode.hxx     ComponentDefinition.hxx \
DataStreamPort.hxx       InPort.hxx          OutGate.hxx                     ServiceNode.hxx           Catalog.hxx  \
PropertyInterface.hxx    AnyOutputPort.hxx   Pool.hxx

SWIGINCLUDES = $(ENGINEINCLUDES:%=$(top_srcdir)/src/engine/%) $(top_srcdir)/src/bases/yacsconfig.h ExecutorSwig.hxx

pilotWRAP.cxx:pilot.i $(SWIGINCLUDES) engtypemaps.i docengine.i
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
	-I$(srcdir)/../engine \
	-I$(srcdir)/../bases

_pilot_la_LDFLAGS  = -module

_pilot_la_LIBADD = ../engine/libYACSlibEngine.la $(OMNIORB_LIBS) $(PYTHON_LIBS)


pypilotWRAP.cxx: pypilot.i pilot.i $(SWIGINCLUDES) engtypemaps.i pilotWRAP.cxx
	$(SWIG) $(SWIG_PYTHON_OPT) $(SWIG_PYTHON_INCLUDES) $(MYSWIG_FLAGS) -o pypilotWRAP.cxx $(srcdir)/pypilot.i

pypilotWRAP.h: pypilotWRAP.cxx
pilotWRAP.h: pilotWRAP.cxx

pypilot.py : pypilotWRAP.cxx 

_pypilot_la_SOURCES =  pypilotWRAP.cxx
_pypilot_la_CXXFLAGS =  $(THREAD_DEF) $(PYTHON_CPPFLAGS) $(OMNIORB_INCLUDES) $(OMNIORB_CXXFLAGS) -I$(srcdir)/../engine -I$(srcdir)/../bases
_pypilot_la_LDFLAGS  = -module
_pypilot_la_LIBADD = ../engine/libYACSlibEngine.la $(OMNIORB_LIBS) $(PYTHON_LIBS)

docengine.i:$(SWIGINCLUDES)
	DOXSRCDIR=$(top_srcdir)/src/engine doxygen $(top_srcdir)/Misc/Doxyfile
	$(PYTHON) $(top_srcdir)/Misc/doxy2swig.py -n xml/index.xml docengine.i

EXTRA_DIST = ExecutorSwig.hxx pilot.i pypilot.i engtypemaps.i pypilotWRAP.h

clean-local:
	rm -f pypilot.py* pypilotWRAP.* pilot.py* pilotWRAP.* docengine.i
	rm -rf xml

include $(top_srcdir)/adm/unix/make_end.am
