# Copyright (C) 2012-2024  CEA, EDF
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

INCLUDE(${SWIG_USE_FILE})

# --- options ---

IF(SALOME_YACS_USE_KERNEL)
  SET(SALOME_INCL_PATH ${KERNEL_INCLUDE_DIRS})
ENDIF(SALOME_YACS_USE_KERNEL)

# additional include directories
INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${SALOME_INCL_PATH}
  ${PTHREAD_INCLUDE_DIRS}
  ${OMNIORB_INCLUDE_DIR}
  ${OMNIORBPY_INCLUDE_DIR}
  ${LIBXML2_INCLUDE_DIR}
  ${PYTHON_INCLUDE_DIRS}
  ${PROJECT_BINARY_DIR}/idl
  ${PROJECT_BINARY_DIR}/src/engine_swig
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/src/bases
  ${PROJECT_SOURCE_DIR}/src/engine
  ${PROJECT_SOURCE_DIR}/src/engine_swig
  ${PROJECT_SOURCE_DIR}/src/runtime
)

# swig flags
SET_SOURCE_FILES_PROPERTIES(SALOMERuntime.i PROPERTIES 
				    CPLUSPLUS ON 
				    SWIG_DEFINITIONS "-shadow")
SET_PROPERTY(SOURCE SALOMERuntime.i PROPERTY 
		    SWIG_FLAGS "-noexcept" "-DYACS_PTHREAD")

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${PYTHON_DEFINITIONS}
  ${OMNIORB_DEFINITIONS}
  )

ADD_LIBRARY(YACSRuntimePython ThreadLauncher.cxx)
TARGET_LINK_LIBRARIES(YACSRuntimePython YACSRuntimeSALOME ${PYTHON_LIBRARIES} )
INSTALL(TARGETS YACSRuntimePython EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

# libraries to link to
SET(_link_LIBRARIES
  YACSRuntimeSALOME
  YACSRuntimePython
  ${PYTHON_LIBRARIES}
  ${OMNIORB_LIBRARIES}
  )

# --- scripts ---
# scripts / swig wrappings
SET(_swig_SCRIPTS
  ${CMAKE_CURRENT_BINARY_DIR}/SALOMERuntime.py
)

IF(WIN32)
  SET(SET_CMD set)
  SET(SEP_CMD &&)
ENDIF(WIN32)

IF(SALOME_BUILD_DOC)
  ADD_CUSTOM_TARGET(docruntime.i
    ${SET_CMD} DOXSRCDIR=${PROJECT_SOURCE_DIR}/src/runtime ${SEP_CMD} ${DOXYGEN_EXECUTABLE} ${PROJECT_SOURCE_DIR}/Misc/Doxyfile
    COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/Misc/doxy2swig.py -n xml/index.xml docruntime.i
    VERBATIM
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
ENDIF(SALOME_BUILD_DOC)

# --- rules ---
SET(SWIGINCLUDES 
  ${PROJECT_SOURCE_DIR}/src/runtime/RuntimeSALOME.hxx
  ${PROJECT_SOURCE_DIR}/src/runtime/SALOMEDispatcher.hxx
  ${PROJECT_SOURCE_DIR}/src/runtime/SalomeProc.hxx
  ${PROJECT_SOURCE_DIR}/src/runtime/PythonNode.hxx
  ${PROJECT_SOURCE_DIR}/src/runtime/PythonPorts.hxx
  ${PROJECT_SOURCE_DIR}/src/runtime/CORBANode.hxx
  ${PROJECT_SOURCE_DIR}/src/runtime/CORBAPorts.hxx
  ${PROJECT_SOURCE_DIR}/src/runtime/TypeConversions.hxx
  ${PROJECT_SOURCE_DIR}/src/runtime/SalomeOptimizerLoop.hxx
  ${PROJECT_SOURCE_DIR}/src/runtime/VisitorSalomeSaveState.hxx
  )
SET(SWIG_MODULE_SALOMERuntime_EXTRA_DEPS 
    ${PROJECT_SOURCE_DIR}/src/engine_swig/pilot.i 
    ${PROJECT_SOURCE_DIR}/src/engine_swig/engtypemaps.i  
    ${SWIGINCLUDES})

IF(${CMAKE_VERSION} VERSION_LESS "3.8.0") 
  SWIG_ADD_MODULE(SALOMERuntime python SALOMERuntime.i)
ELSE()
  SWIG_ADD_LIBRARY(SALOMERuntime LANGUAGE python SOURCES SALOMERuntime.i)
ENDIF()

SWIG_LINK_LIBRARIES(SALOMERuntime ${_link_LIBRARIES})
SWIG_CHECK_GENERATION(SALOMERuntime)
IF(WIN32)
  SET_TARGET_PROPERTIES(_SALOMERuntime PROPERTIES DEBUG_OUTPUT_NAME _SALOMERuntime_d)
ENDIF(WIN32)

INSTALL(TARGETS ${SWIG_MODULE_SALOMERuntime_REAL_NAME} DESTINATION ${SALOME_INSTALL_PYTHON})

SALOME_INSTALL_SCRIPTS("${_swig_SCRIPTS}"  ${SALOME_INSTALL_PYTHON} EXTRA_DPYS "${SWIG_MODULE_SALOMERuntime_REAL_NAME}")
