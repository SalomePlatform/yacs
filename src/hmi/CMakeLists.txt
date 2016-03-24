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

# --- options ---

IF(SALOME_YACS_USE_KERNEL)
  INCLUDE_DIRECTORIES(${KERNEL_INCLUDE_DIRS})
  ADD_DEFINITIONS(${KERNEL_DEFINITIONS})
ENDIF(SALOME_YACS_USE_KERNEL)

# additional include directories
INCLUDE_DIRECTORIES(
  ${OMNIORB_INCLUDE_DIR}
  ${LIBXML2_INCLUDE_DIR}
  ${PYTHON_INCLUDE_DIR}
  ${PROJECT_SOURCE_DIR}/src/bases
  ${PROJECT_SOURCE_DIR}/src/engine
  ${PROJECT_SOURCE_DIR}/src/runtime
  ${PROJECT_SOURCE_DIR}/src/yacsloader
  ${PROJECT_BINARY_DIR}/idl
  ${PROJECT_BINARY_DIR}/src/yacsorb
  )

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${PYTHON_DEFINITIONS}
  ${OMNIORB_DEFINITIONS}
  )

# libraries to link to
SET(_link_LIBRARIES
  ${LIBXML2_LIBRARIES}
  YACSRuntimeSALOME
  YACSloader
  )

# --- sources ---

SET(HMI_SOURCES
  commands.cxx
  commandsProc.cxx
  guiObservers.cxx
  guiContext.cxx
  )

# --- rules ---

ADD_LIBRARY(HMI ${HMI_SOURCES})
TARGET_LINK_LIBRARIES(HMI ${_link_LIBRARIES})
INSTALL(TARGETS HMI EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})
