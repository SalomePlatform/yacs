# Copyright (C) 2012-2016  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
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

IF(SALOME_BUILD_TESTS)
  ADD_SUBDIRECTORY(Plugin)
  ADD_SUBDIRECTORY(Test)
ENDIF(SALOME_BUILD_TESTS)

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${PTHREAD_INCLUDE_DIR}
  ${PROJECT_SOURCE_DIR}/src/bases
  )

# libraries to link to
SET(_link_LIBRARIES
  ${PTHREAD_LIBRARIES}
  YACSBases
  )

# --- headers ---

SET(YACSlibEngine_HEADERS
  YACSlibEngineExport.hxx
  Any.hxx
  AnyInputPort.hxx
  Bloc.hxx
  Catalog.hxx
  ComponentDefinition.hxx
  ComponentInstance.hxx
  ComposedNode.hxx
  ConditionInputPort.hxx
  Container.hxx
  ConversionException.hxx
  DataFlowPort.hxx
  DataNode.hxx
  DataPort.hxx
  DataStreamPort.hxx
  DeploymentTree.hxx
  Dispatcher.hxx
  DynParaLoop.hxx
  ElementaryNode.hxx
  Executor.hxx
  ForEachLoop.hxx
  ForLoop.hxx
  InGate.hxx
  InlineNode.hxx
  InPort.hxx
  InputDataStreamPort.hxx
  InputPort.hxx
  InvalidExtractionException.hxx
  LinkInfo.hxx
  Logger.hxx
  LogRecord.hxx
  Loop.hxx
  Node.hxx
  OptimizerAlg.hxx
  OptimizerLoop.hxx
  OutGate.hxx
  OutPort.hxx
  OutputDataStreamPort.hxx
  OutputPort.hxx
  Pool.hxx
  Port.hxx
  Proc.hxx
  RefCounter.hxx
  Runtime.hxx
  Scheduler.hxx
  ServiceInlineNode.hxx
  ServiceNode.hxx
  SharedPtr.hxx
  StaticDefinedComposedNode.hxx
  Switch.hxx
  Task.hxx
  TypeCode.hxx
  Visitor.hxx
  VisitorSaveSchema.hxx
  VisitorSaveState.hxx
  WhileLoop.hxx
  PropertyInterface.hxx
  AnyOutputPort.hxx
  ServerNode.hxx
  InPropertyPort.hxx
  AbstractPoint.hxx
  BlocPoint.hxx
  BagPoint.hxx
  ForkBlocPoint.hxx
  LinkedBlocPoint.hxx
  ElementaryPoint.hxx
  SetOfPoints.hxx
  )

# --- sources ---

SET(YACSlibEngine_SOURCES
  PropertyInterface.cxx
  TypeCode.cxx
  RefCounter.cxx
  LinkInfo.cxx
  ConversionException.cxx
  InvalidExtractionException.cxx
  ConditionInputPort.cxx
  AnyInputPort.cxx
  AnyOutputPort.cxx
  Port.cxx InGate.cxx
  OutGate.cxx
  DataPort.cxx
  DataFlowPort.cxx
  InPort.cxx
  OutPort.cxx
  InputPort.cxx
  OutputPort.cxx
  DataStreamPort.cxx
  InputDataStreamPort.cxx
  OutputDataStreamPort.cxx
  Any.cxx
  Pool.cxx
  Node.cxx
  ElementaryNode.cxx
  ComposedNode.cxx
  InlineNode.cxx
  ServiceNode.cxx
  ServerNode.cxx
  ServiceInlineNode.cxx
  StaticDefinedComposedNode.cxx
  Bloc.cxx
  Proc.cxx
  Loop.cxx
  ForLoop.cxx
  WhileLoop.cxx
  Switch.cxx
  DynParaLoop.cxx
  ForEachLoop.cxx
  OptimizerAlg.cxx
  OptimizerLoop.cxx
  Runtime.cxx
  Scheduler.hxx
  Task.hxx
  Executor.cxx
  Visitor.cxx
  VisitorSaveState.cxx
  VisitorSaveSchema.cxx
  ComponentInstance.cxx
  Dispatcher.cxx
  Container.cxx
  HomogeneousPoolContainer.cxx
  DeploymentTree.cxx
  Logger.cxx
  LogRecord.cxx
  Catalog.cxx
  ComponentDefinition.cxx
  DataNode.cxx
  Task.cxx
  Scheduler.cxx
  InPropertyPort.cxx
  AbstractPoint.cxx
  BlocPoint.cxx
  BagPoint.cxx
  ForkBlocPoint.cxx
  LinkedBlocPoint.cxx
  ElementaryPoint.cxx
  SetOfPoints.cxx
  )
SET(YACSlibEngine_HEADERS ${YACSlibEngine_HEADERS} PARENT_SCOPE)  # Make it visible to src/engine_swig to handle dependencies

# --- rules ---

ADD_LIBRARY(YACSlibEngine ${YACSlibEngine_SOURCES})
TARGET_LINK_LIBRARIES(YACSlibEngine ${_link_LIBRARIES})
INSTALL(TARGETS YACSlibEngine EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${YACSlibEngine_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
