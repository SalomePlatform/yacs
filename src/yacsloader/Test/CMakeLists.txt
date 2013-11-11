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

INCLUDE(UseOmniORB)

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${CPPUNIT_INCLUDE_DIRS}
  ${PYTHON_INCLUDE_DIR}
  ${OMNIORB_INCLUDE_DIR}
  ${EXPAT_INCLUDE_DIR}
  ${KERNEL_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/..
  ${CMAKE_CURRENT_SOURCE_DIR}/../../bases
  ${CMAKE_CURRENT_SOURCE_DIR}/../../bases/Test
  ${CMAKE_CURRENT_SOURCE_DIR}/../../engine
  ${CMAKE_CURRENT_SOURCE_DIR}/../../runtime
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

# --- sources ---

# idl files
SET(echo_IDL_FILES
  echo.idl
)
SET(echo_IDL_SOURCES
  echoSK.cc
)
SET(IDL_INCLUDE_DIRS
  ${KERNEL_ROOT_DIR}/idl/salome
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)
SET(IDL_LINK_FLAGS
  ${KERNEL_SalomeIDLKernel}
)

# tests variables

SET(echoSrv_SOURCES
  echoSrv.cxx 
)
SET(echoSrv_LIBRARIES
  ${OMNIORB_LIBRARIES}
  ${PTHREAD_LIBRARIES}
)
  
SET(TestYacsLoader_SOURCES
  TestYacsLoader.cxx
  YacsLoaderTest.cxx
)
SET(TestYacsLoader_LIBRARIES
  ${CPPUNIT_LIBRARIES}
  ${OMNIORB_LIBRARIES}
  ${PYTHON_LIBRARIES}
  YACSloader
  YACSRuntimeSALOME
  YACSlibEngine
  YACSBases
)

SET(TestOptLoop_SOURCES
  OptimizerAlgSyncExample.cxx 
  OptimizerAlgASyncExample.cxx
)

# --- rules ---

OMNIORB_ADD_MODULE(yacsloader_echo "${echo_IDL_FILES}" "${IDL_INCLUDE_DIRS}" "${IDL_LINK_FLAGS}")

ADD_EXECUTABLE(TestYacsLoader ${TestYacsLoader_SOURCES})
TARGET_LINK_LIBRARIES(TestYacsLoader ${TestYacsLoader_LIBRARIES})

ADD_EXECUTABLE(echoSrv ${echoSrv_SOURCES})
TARGET_LINK_LIBRARIES(echoSrv yacsloader_echo ${echoSrv_LIBRARIES})
INSTALL(TARGETS echoSrv DESTINATION ${SALOME_INSTALL_BINS})

ADD_LIBRARY(TestOptLoop ${TestOptLoop_SOURCES})
TARGET_LINK_LIBRARIES(TestOptLoop YACSlibEngine)

IF(NOT WIN32)
  SET(SHELL /bin/sh)
  SALOME_CONFIGURE_FILE(config_appli.xml.in config_appli.xml)
  SALOME_CONFIGURE_FILE(YacsLoaderTest.sh.in YacsLoaderTest.sh)
  SALOME_CONFIGURE_FILE(YacsLoaderInSessionTest.sh.in YacsLoaderInSessionTest.sh)
  SALOME_CONFIGURE_FILE(display.sh.in VERSION INSTALL ${SALOME_INSTALL_BINS})
  SALOME_CONFIGURE_FILE(waitContainers.py waitContainers.py)
  ADD_TEST(NAME YacsLoaderTest COMMAND ${SHELL} YacsLoaderTest.sh)
ENDIF()