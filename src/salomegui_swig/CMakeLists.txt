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

# swig flags
SET_SOURCE_FILES_PROPERTIES(libYACS_Swig.i PROPERTIES 
                    CPLUSPLUS ON 
                    SWIG_DEFINITIONS "-shadow")
SET_PROPERTY(SOURCE libYACS_Swig.i PROPERTY SWIG_FLAGS "-noexcept" "-DYACS_PTHREAD")

# --- options ---

IF(SALOME_YACS_USE_KERNEL)
  SET(SALOME_INCL_PATH ${KERNEL_INCLUDE_DIRS})
ENDIF(SALOME_YACS_USE_KERNEL)

# additional include directories
INCLUDE_DIRECTORIES(
  ${SALOME_INCL_PATH}
  ${GUI_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/src/salomegui
  ${PROJECT_SOURCE_DIR}/src/salomewrap
  ${OMNIORB_INCLUDE_DIR}
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${QT_DEFINITIONS}
  ${PYTHON_DEFINITIONS}
  ${OMNIORB_DEFINITIONS}
  ${BOOST_DEFINITIONS}
  )

# libraries to link to
SET(_link_LIBRARIES
  YACS
  ${PYTHON_LIBRARIES}
  )

# --- sources ---
SET(YACS_Swig_SOURCES
  YACSGUI_Swig.cxx
  )

# --- scripts ---
# scripts / swig wrappings
SET(_swig_SCRIPTS
  ${CMAKE_CURRENT_BINARY_DIR}/libYACS_Swig.py
)

IF(WIN32)
  SET(SET_CMD set)
  SET(SEP_CMD &&)
ENDIF(WIN32)

IF(SALOME_BUILD_DOC)
  ADD_CUSTOM_TARGET(doc.i
    ${SET_CMD} DOXSRCDIR=${CMAKE_CURRENT_SOURCE_DIR} ${SEP_CMD} ${DOXYGEN_EXECUTABLE} ${PROJECT_SOURCE_DIR}/Misc/Doxyfile
    COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/Misc/doxy2swig.py -n xml/index.xml doc.i
    DEPENDS YACSGUI_Swig.hxx YACSGUI_Swig.cxx
    VERBATIM
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
ENDIF(SALOME_BUILD_DOC)

# --- rules ---
ADD_LIBRARY(YACS_Swig ${YACS_Swig_SOURCES})
TARGET_LINK_LIBRARIES(YACS_Swig ${_link_LIBRARIES})
INSTALL(TARGETS YACS_Swig EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

SET(SWIG_MODULE_libYACS_Swig_EXTRA_DEPS YACSGUI_Swig.hxx)
SWIG_ADD_MODULE(libYACS_Swig python libYACS_Swig.i YACSGUI_Swig.cxx)

SWIG_LINK_LIBRARIES(libYACS_Swig "${_link_LIBRARIES}")
IF(WIN32)
  SET_TARGET_PROPERTIES(_libYACS_Swig PROPERTIES DEBUG_OUTPUT_NAME _libYACS_Swig_d)
ENDIF(WIN32)

INSTALL(TARGETS ${SWIG_MODULE_libYACS_Swig_REAL_NAME} DESTINATION ${SALOME_INSTALL_PYTHON})

SALOME_INSTALL_SCRIPTS("${_swig_SCRIPTS}" ${SALOME_INSTALL_PYTHON})
