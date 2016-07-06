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

INCLUDE(${SWIG_USE_FILE})

IF(SALOME_BUILD_TESTS)
  ADD_SUBDIRECTORY(Test)
ENDIF(SALOME_BUILD_TESTS)

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/src/bases
  ${PROJECT_SOURCE_DIR}/src/engine
  ${PROJECT_SOURCE_DIR}/src/engine_swig
  ${PROJECT_SOURCE_DIR}/src/yacsloader
  ${PROJECT_BINARY_DIR}/src/engine_swig
  ${OMNIORB_INCLUDE_DIR}
  ${PYTHON_INCLUDE_DIRS}
)

# swig flags
SET_SOURCE_FILES_PROPERTIES(loader.i PROPERTIES 
				    CPLUSPLUS ON 
				    SWIG_DEFINITIONS "-shadow")
SET_PROPERTY(SOURCE loader.i PROPERTY 
		    SWIG_FLAGS "-noexcept" "-DYACS_PTHREAD")

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${PYTHON_DEFINITIONS}
  ${OMNIORB_DEFINITIONS}
  )

# libraries to link to
SET(_link_LIBRARIES
  YACSloader
  ${PYTHON_LIBRARIES}
  ${OMNIORB_LIBRARIES}
  )

# --- scripts ---
# scripts / swig wrappings
SET(_swig_SCRIPTS
  ${CMAKE_CURRENT_BINARY_DIR}/loader.py
)

IF(WIN32)
  SET(SET_CMD set)
  SET(SEP_CMD &&)
ENDIF(WIN32)

IF(SALOME_BUILD_DOC)
  ADD_CUSTOM_TARGET(docyacsloader.i
    ${SET_CMD} DOXSRCDIR=${PROJECT_SOURCE_DIR}/src/yacsloader ${SEP_CMD} ${DOXYGEN_EXECUTABLE} ${PROJECT_SOURCE_DIR}/Misc/Doxyfile
    COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/Misc/doxy2swig.py -n xml/index.xml docyacsloader.i
    VERBATIM
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
ENDIF(SALOME_BUILD_DOC)

# --- rules ---
SET(SWIGINCLUDES 
  ${PROJECT_SOURCE_DIR}/src/yacsloader/parsers.hxx
  ${PROJECT_SOURCE_DIR}/src/yacsloader/LoadState.hxx
  ${PROJECT_SOURCE_DIR}/src/yacsloader/xmlParserBase.hxx
  ${PROJECT_SOURCE_DIR}/src/yacsloader/ProcCataLoader.hxx
  )

SET(SWIG_MODULE_loader_EXTRA_DEPS 
    ${PROJECT_SOURCE_DIR}/src/engine_swig/pilot.i 
    ${PROJECT_SOURCE_DIR}/src/engine_swig/engtypemaps.i  
    ${SWIGINCLUDES})
SWIG_ADD_MODULE(loader python loader.i)

SWIG_LINK_LIBRARIES(loader ${_link_LIBRARIES})
IF(WIN32)
  SET_TARGET_PROPERTIES(_loader PROPERTIES DEBUG_OUTPUT_NAME _loader_d)
ENDIF(WIN32)

INSTALL(TARGETS ${SWIG_MODULE_loader_REAL_NAME} DESTINATION ${SALOME_INSTALL_PYTHON})

SALOME_INSTALL_SCRIPTS("${_swig_SCRIPTS}"  ${SALOME_INSTALL_PYTHON})
