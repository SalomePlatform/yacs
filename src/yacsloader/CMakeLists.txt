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
  ADD_SUBDIRECTORY(Test)
  ADD_SUBDIRECTORY(pmml)  
ENDIF(SALOME_BUILD_TESTS)

# --- options ---

IF(SALOME_YACS_USE_KERNEL)
  INCLUDE_DIRECTORIES(${KERNEL_INCLUDE_DIRS})
  ADD_DEFINITIONS(${KERNEL_DEFINITIONS})

  SET(SALOME_LIBS 
    ${KERNEL_SalomeLifeCycleCORBA}
    ${KERNEL_SalomeIDLKernel}
    ${KERNEL_SalomeNS}
    ${KERNEL_SalomeContainer}
    ${KERNEL_SALOMEBasics}
    ${KERNEL_SalomeResourcesManager}
    ${KERNEL_OpUtil}
    ${KERNEL_SALOMELocalTrace}
    ${KERNEL_Registry}
    ${KERNEL_SalomeNotification}
    ${KERNEL_ResourcesManager}
    ${KERNEL_SalomeHDFPersist}
    ${KERNEL_SalomeGenericObj}
    )
  IF(SALOME_YACS_USE_DSC)
    LIST(APPEND SALOME_LIBS ${KERNEL_SalomeDSCContainer})
  ENDIF(SALOME_YACS_USE_DSC)
ENDIF(SALOME_YACS_USE_KERNEL)

# additional include directories
INCLUDE_DIRECTORIES(
  ${OMNIORB_INCLUDE_DIR}
  ${PYTHON_INCLUDE_DIR}
  ${PROJECT_SOURCE_DIR}/src/bases
  ${PROJECT_SOURCE_DIR}/src/engine
  ${PROJECT_SOURCE_DIR}/src/runtime
  ${PROJECT_SOURCE_DIR}/src/yacsloader
  ${PROJECT_SOURCE_DIR}
  )

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${PYTHON_DEFINITIONS}
  ${OMNIORB_DEFINITIONS}
  )
SET(_exec_DEFINITIONS
  -g
  )

# libraries to link to
SET(YACSloader_LIBRARIES
  ${OMNIORB_LIBRARIES}
  ${LIBXML2_LIBRARIES}
  YACSlibEngine
  )
SET(_link_LIBRARIES
  YACSloader
  YACSRuntimeSALOME
  YACSlibEngine
  ${SALOME_LIBS}
  ${OMNIORB_LIBRARIES}
  ${PYTHON_LIBRARIES}
  ${LIBXML2_LIBRARIES}
  )

# --- headers ---

SET(YACSloader_HEADERS
  YACSloaderExport.hxx
  blocParsers.hxx
  codeParsers.hxx
  containerParsers.hxx
  dataParsers.hxx
  factory.hxx
  linkParsers.hxx
  LoadState.hxx
  loopParsers.hxx
  nodeParsers.hxx
  outputParsers.hxx
  parsers.hxx
  parserBase.hxx
  portParsers.hxx
  presetParsers.hxx
  ProcCataLoader.hxx
  procParsers.hxx
  propertyParsers.hxx
  rootParser.hxx
  switchParsers.hxx
  inlineParsers.hxx
  componentinstanceParsers.hxx
  remoteParsers.hxx
  serverParsers.hxx
  serviceParsers.hxx
  typeParsers.hxx
  xmlParserBase.hxx
  xmlrpcParsers.hxx
  sinlineParsers.hxx
  )

# --- sources ---

SET(YACSloader_SOURCES
  parsers.cxx
  parserBase.cxx
  dataParsers.cxx
  typeParsers.cxx
  propertyParsers.cxx
  containerParsers.cxx
  xmlrpcParsers.cxx
  rootParser.cxx
  codeParsers.cxx
  LoadState.cxx
  xmlParserBase.cxx
  ProcCataLoader.cxx
  componentinstanceParsers.cxx
  )
SET(driver_SOURCES
  driver.cxx
  )
SET(resume_SOURCES
  resume.cxx
  )
SET(debugger_SOURCES
  debugger.cxx
  )
SET(ExampleOfObserversPluginForDriver_SOURCES
  ExampleOfObserversPluginForDriver.cxx
  )

# --- rules ---

ADD_LIBRARY(YACSloader ${YACSloader_SOURCES})
TARGET_LINK_LIBRARIES(YACSloader ${YACSloader_LIBRARIES})
INSTALL(TARGETS YACSloader EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

ADD_LIBRARY(ExampleOfObserversPluginForDriver ${ExampleOfObserversPluginForDriver_SOURCES})
TARGET_LINK_LIBRARIES(ExampleOfObserversPluginForDriver ${_link_LIBRARIES})
INSTALL(TARGETS ExampleOfObserversPluginForDriver DESTINATION ${SALOME_INSTALL_LIBS})

ADD_EXECUTABLE(driver ${driver_SOURCES})
TARGET_LINK_LIBRARIES(driver ${_link_LIBRARIES})

ADD_EXECUTABLE(resume ${resume_SOURCES})
TARGET_LINK_LIBRARIES(resume ${_link_LIBRARIES})

ADD_EXECUTABLE(debugger ${debugger_SOURCES})
TARGET_LINK_LIBRARIES(debugger ${_link_LIBRARIES})

SET_TARGET_PROPERTIES(driver resume debugger PROPERTIES COMPILE_FLAGS ${_exec_DEFINITIONS})

INSTALL(TARGETS driver resume debugger EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_BINS})

INSTALL(FILES ${YACSloader_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})

FILE(GLOB sample_files
  ${PROJECT_SOURCE_DIR}/src/yacsloader/samples/*.xml 
  ${PROJECT_SOURCE_DIR}/src/yacsloader/samples/*.pmml
  ${PROJECT_SOURCE_DIR}/src/yacsloader/samples/*.data)
INSTALL(FILES ${sample_files} DESTINATION ${SALOME_YACS_INSTALL_SAMPLES})
