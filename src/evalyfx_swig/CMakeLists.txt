# Copyright (C) 2015-2016  CEA/DEN, EDF R&D
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

# additional include directories
INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/src/bases
  ${PROJECT_SOURCE_DIR}/src/engine
  ${PROJECT_SOURCE_DIR}/src/engine_swig
  ${PROJECT_SOURCE_DIR}/src/yacsloader
  ${PROJECT_SOURCE_DIR}/src/yacsloader_swig
  ${PROJECT_SOURCE_DIR}/src/evalyfx
  ${OMNIORB_INCLUDE_DIR}
  ${PYTHON_INCLUDE_DIRS}
)

# swig flags
SET_SOURCE_FILES_PROPERTIES(evalyfx.i PROPERTIES CPLUSPLUS ON SWIG_DEFINITIONS "-shadow")

SET_PROPERTY(SOURCE evalyfx.i PROPERTY SWIG_FLAGS "-noexcept" "-DYACS_PTHREAD")

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${PYTHON_DEFINITIONS}
  ${OMNIORB_DEFINITIONS}
  )

# libraries to link to
SET(_link_LIBRARIES
  YACSevalYFX
  ${PYTHON_LIBRARIES}
  ${OMNIORB_LIBRARIES}
  )

SET(_swig_SCRIPTS ${CMAKE_CURRENT_BINARY_DIR}/evalyfx.py)

SWIG_ADD_MODULE(evalyfx python evalyfx.i)

SWIG_LINK_LIBRARIES(evalyfx ${_link_LIBRARIES})
IF(WIN32)
  SET_TARGET_PROPERTIES(_evalyfx PROPERTIES DEBUG_OUTPUT_NAME _evalyfx_d)
ENDIF(WIN32)

INSTALL(TARGETS ${SWIG_MODULE_evalyfx_REAL_NAME} DESTINATION ${SALOME_INSTALL_PYTHON})

SALOME_INSTALL_SCRIPTS("${_swig_SCRIPTS}"  ${SALOME_INSTALL_PYTHON})
