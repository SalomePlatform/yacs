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

INCLUDE(UseOmniORB)

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${CPPUNIT_INCLUDE_DIRS}
  ${PYTHON_INCLUDE_DIR}
  ${OMNIORB_INCLUDE_DIR}
  ${KERNEL_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/..
  ${CMAKE_CURRENT_SOURCE_DIR}/../../bases
  ${CMAKE_CURRENT_SOURCE_DIR}/../../bases/Test
  ${CMAKE_CURRENT_SOURCE_DIR}/../../engine
  ${CMAKE_CURRENT_SOURCE_DIR}/../../runtime
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${OMNIORB_DEFINITIONS}
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
SET(LOCAL_TEST_DIR ${SALOME_YACS_INSTALL_TEST}/yacsloader)

OMNIORB_ADD_MODULE(yacsloader_echo "${echo_IDL_FILES}" "${IDL_INCLUDE_DIRS}" "${IDL_LINK_FLAGS}")
INSTALL(TARGETS yacsloader_echo DESTINATION ${SALOME_INSTALL_LIBS})

ADD_EXECUTABLE(TestYacsLoader ${TestYacsLoader_SOURCES})
TARGET_LINK_LIBRARIES(TestYacsLoader yacsloader_echo ${TestYacsLoader_LIBRARIES})

ADD_EXECUTABLE(echoSrv ${echoSrv_SOURCES})
TARGET_LINK_LIBRARIES(echoSrv yacsloader_echo ${echoSrv_LIBRARIES})

ADD_LIBRARY(TestOptLoop ${TestOptLoop_SOURCES})
TARGET_LINK_LIBRARIES(TestOptLoop yacsloader_echo YACSlibEngine)

IF(NOT WIN32)
  SET(SHELL /bin/sh)
  SALOME_CONFIGURE_FILE(config_appli.xml.in config_appli.xml)
  SALOME_CONFIGURE_FILE(YacsLoaderTest.sh.in YacsLoaderTest.sh)
  SALOME_CONFIGURE_FILE(YacsLoaderInSessionTest.sh.in YacsLoaderInSessionTest.sh)
  SALOME_CONFIGURE_FILE(display.sh.in display.sh)
  SALOME_CONFIGURE_FILE(waitContainers.py waitContainers.py)
  SALOME_CONFIGURE_FILE(xmlrun_orig.sh xmlrun.sh)
  ADD_TEST(NAME YacsLoaderTest COMMAND ${SHELL} YacsLoaderTest.sh)
ENDIF()

# For salome test
IF(NOT WIN32)
  SET(LOCAL_TEST_FILES
    algoasyncexample.py
    algosyncexample.py
    genPascal.py
    genTriangle.py
    echoclt.py)
  INSTALL(FILES ${LOCAL_TEST_FILES}
          DESTINATION ${LOCAL_TEST_DIR})
  INSTALL(TARGETS echoSrv TestYacsLoader
          DESTINATION ${LOCAL_TEST_DIR})
  INSTALL(TARGETS yacsloader_echo TestOptLoop
          DESTINATION ${SALOME_YACS_INSTALL_TEST_LIB})
  INSTALL(FILES CTestTestfileInstall.cmake
          DESTINATION ${LOCAL_TEST_DIR}
          RENAME CTestTestfile.cmake)
  INSTALL(FILES optim_plugin.py
          DESTINATION ${LOCAL_TEST_DIR})
  INSTALL(PROGRAMS runYacsLoaderTest.sh ${CMAKE_CURRENT_BINARY_DIR}/xmlrun.sh
          DESTINATION ${LOCAL_TEST_DIR})
  # Use relative path for symbolic links in order to allow the copy of the directory.
  # REL_PATH_TO_INSTALL goes to CMAKE_INSTALL_PREFIX from ${CMAKE_INSTALL_PREFIX}/${LOCAL_TEST_DIR}
  IF(IS_ABSOLUTE ${SALOME_YACS_INSTALL_SAMPLES})
    SET(SAMPLES_LINK_TARGET ${SALOME_YACS_INSTALL_SAMPLES})
  ELSE()
    STRING(REGEX REPLACE [^/]+ ".." REL_PATH_TO_INSTALL ${LOCAL_TEST_DIR})
    SET(SAMPLES_LINK_TARGET ${REL_PATH_TO_INSTALL}/${SALOME_YACS_INSTALL_SAMPLES})
  ENDIF()
  INSTALL(CODE "EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E create_symlink
                ${SAMPLES_LINK_TARGET}
                ${CMAKE_INSTALL_PREFIX}/${LOCAL_TEST_DIR}/samples )" )
ENDIF()
