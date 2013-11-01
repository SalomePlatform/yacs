# Copyright (C) 2012-2013  CEA/DEN, EDF R&D, OPEN CASCADE
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

INCLUDE(${SWIG_USE_FILE})

# --- options ---

IF(WIN32)                                       # if platform is Windows 64 bit 
  IF("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")          
    ADD_DEFINITIONS(/bigobj)                    # to avoid error C1128
  ENDIF()
ENDIF()

# additional include directories
INCLUDE_DIRECTORIES(
  ${PTHREAD_INCLUDE_DIR}
  ${OMNIORB_INCLUDE_DIR}
  ${PYTHON_INCLUDE_DIRS}
  ${PROJECT_BINARY_DIR}/idl
  ${PROJECT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}/src/bases
  ${PROJECT_SOURCE_DIR}/src/engine
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

# swig flags
SET_SOURCE_FILES_PROPERTIES(pilot.i pypilot.i PROPERTIES 
				    CPLUSPLUS ON 
				    SWIG_DEFINITIONS "-shadow")
SET_PROPERTY(SOURCE pilot.i pypilot.i PROPERTY 
		    SWIG_FLAGS "-noexcept" "-DYACS_PTHREAD")

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${PYTHON_DEFINITIONS}
  ${OMNIORB_DEFINITIONS}
  -DYACS_PTHREAD
  )
IF(WIN32)
  ADD_DEFINITIONS(-DNOGDI)
ENDIF(WIN32)

# libraries to link to
SET(_link_LIBRARIES
  YACSlibEngine
  ${PYTHON_LIBRARIES}
  ${OMNIORB_LIBRARIES}
  )

# --- sources ---
SET(pilot_SOURCES
  ExecutorSwig.cxx
  )
# --- headers ---
SET(pilot_HEADERS
  ExecutorSwig.hxx
  )
# --- sources ---
SET(pypilot_SOURCES
  
  )
# --- headers ---
SET(pypilot_HEADERS
  )

# --- scripts ---
# scripts / swig wrappings
SET(_swig_SCRIPTS
  ${CMAKE_CURRENT_BINARY_DIR}/pilot.py
  ${CMAKE_CURRENT_BINARY_DIR}/pypilot.py
)

IF(WIN32)
  SET(SET_CMD set)
ENDIF(WIN32)

IF(SALOME_BUILD_DOC)
  ADD_CUSTOM_TARGET(docengine.i
    ${SET_CMD} DOXSRCDIR=${PROJECT_SOURCE_DIR}/src/engine 
    COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_SOURCE_DIR}/Misc/Doxyfile
    COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/Misc/doxy2swig.py -n xml/index.xml docengine.i
    VERBATIM
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
ENDIF(SALOME_BUILD_DOC)

# --- rules ---
SET(ENGINEINCLUDES 
  Any.hxx     
  DeploymentTree.hxx  
  InputDataStreamPort.hxx         
  OutPort.hxx               
  SharedPtr.hxx 
  AnyInputPort.hxx         
  Dispatcher.hxx      
  InputPort.hxx                   
  OutputDataStreamPort.hxx  
  StaticDefinedComposedNode.hxx 
  Bloc.hxx                 
  DynParaLoop.hxx     
  InvalidExtractionException.hxx  
  OutputPort.hxx            
  Switch.hxx 
  ComponentInstance.hxx    
  ElementaryNode.hxx  
  LinkInfo.hxx                    
  Pool.hxx                  
  Task.hxx 
  ComposedNode.hxx         
  Executor.hxx        
  Logger.hxx                      
  Port.hxx                  
  TypeCode.hxx 
  ConditionInputPort.hxx   
  LogRecord.hxx                   
  Proc.hxx                  
  Visitor.hxx 
  Container.hxx            
  ForEachLoop.hxx     
  Loop.hxx                        
  RefCounter.hxx            
  VisitorSaveSchema.hxx 
  ConversionException.hxx  
  ForLoop.hxx         
  Node.hxx                        
  Runtime.hxx               
  VisitorSaveState.hxx 
  DataFlowPort.hxx         
  InGate.hxx          
  OptimizerAlg.hxx                
  Scheduler.hxx             
  WhileLoop.hxx 
  DataPort.hxx             
  InlineNode.hxx      
  OptimizerLoop.hxx               
  ServiceInlineNode.hxx     
  ComponentDefinition.hxx 
  DataStreamPort.hxx       
  InPort.hxx          
  OutGate.hxx                     
  ServiceNode.hxx           
  Catalog.hxx  
  PropertyInterface.hxx    
  AnyOutputPort.hxx   
  Pool.hxx
  InPropertyPort.hxx
)
SET(SWIGINCLUDES ${ENGINEINCLUDES} 
		 "${PROJECT_SOURCE_DIR}/src/bases/yacsconfig.h" 
		 "ExecutorSwig.hxx")
SWIG_ADD_MODULE(pilot   python pilot.i ExecutorSwig.cxx)
SWIG_ADD_MODULE(pypilot python pypilot.i)
ADD_DEPENDENCIES(_pilot pilot.i engtypemaps.i docengine.i ${SWIGINCLUDES})
ADD_DEPENDENCIES(_pypilot pypilot.i pilot.i engtypemaps.i _pilot ${SWIGINCLUDES})

SWIG_LINK_LIBRARIES(pilot   "${_link_LIBRARIES}")
SWIG_LINK_LIBRARIES(pypilot "${_link_LIBRARIES}")
IF(WIN32)
  SET_TARGET_PROPERTIES(_pilot   PROPERTIES DEBUG_OUTPUT_NAME _pilot_d)
  SET_TARGET_PROPERTIES(_pypilot PROPERTIES DEBUG_OUTPUT_NAME _pypilot_d)
ENDIF(WIN32)

INSTALL(TARGETS ${SWIG_MODULE_pilot_REAL_NAME} ${SWIG_MODULE_pypilot_REAL_NAME} DESTINATION ${SALOME_INSTALL_PYTHON})

SALOME_INSTALL_SCRIPTS("${_swig_SCRIPTS}"  ${SALOME_INSTALL_PYTHON})
