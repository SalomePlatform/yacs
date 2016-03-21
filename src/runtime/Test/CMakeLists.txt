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
  ${CMAKE_CURRENT_SOURCE_DIR}/../../engine/Test
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${PTHREAD_DEFINITIONS}
  ${OMNIORB_DEFINITIONS}
  -DUSE_CPPUNIT
)

# --- sources ---

# idl files
SET(echo_IDL_FILES
  echo.idl
)
SET(echo_IDL_SOURCES
  echoSK.cc 
  echoDynSK.cc 
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

SET(TestRuntime_SOURCES
  TestRuntime.cxx
  runtimeTest.cxx
)
SET(TestRuntime_LIBRARIES
  ${CPPUNIT_LIBRARIES}
  ${OMNIORB_LIBRARIES}
  ${PYTHON_LIBRARIES}
  YACSRuntimeSALOME
  YACSlibEngine
  YACSBases
)
  
SET(runtimeTestEchoSrv_SOURCES
  echoSrv.cxx
)

SET(echo_clt_SOURCES
  echo_clt.cxx
)

SET(TestComponentLocal_SOURCES
  TestComponent.cxx
)

# --- rules ---

OMNIORB_ADD_MODULE(runtime_echo "${echo_IDL_FILES}" "${IDL_INCLUDE_DIRS}" "${IDL_LINK_FLAGS}")

ADD_EXECUTABLE(TestRuntime ${TestRuntime_SOURCES})
TARGET_LINK_LIBRARIES(TestRuntime ${TestRuntime_LIBRARIES})

ADD_EXECUTABLE(runtimeTestEchoSrv ${runtimeTestEchoSrv_SOURCES})
TARGET_LINK_LIBRARIES(runtimeTestEchoSrv runtime_echo ${OMNIORB_LIBRARIES})

ADD_EXECUTABLE(echo_clt ${echo_clt_SOURCES})
TARGET_LINK_LIBRARIES(echo_clt runtime_echo ${OMNIORB_LIBRARIES})

ADD_LIBRARY(TestComponentLocal ${TestComponentLocal_SOURCES})
TARGET_LINK_LIBRARIES(TestComponentLocal YACSlibEngine)

# For salome test
SET(LOCAL_TEST_DIR ${SALOME_YACS_INSTALL_TEST}/runtime)
INSTALL(TARGETS TestComponentLocal EXPORT ${PROJECT_NAME}TargetGroup
        DESTINATION ${LOCAL_TEST_DIR}/lib/salome)

SALOME_GENERATE_TESTS_ENVIRONMENT(tests_env)

IF(NOT WIN32)
  SET(SHELL /bin/sh)
  SALOME_CONFIGURE_FILE(xmlrun_orig.sh xmlrun.sh)
  ADD_TEST(NAME runtimeTest COMMAND ${SHELL} ${CMAKE_CURRENT_SOURCE_DIR}/runtimeTest.sh)
  SET_TESTS_PROPERTIES(runtimeTest PROPERTIES ENVIRONMENT "${tests_env}")
  INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/xmlrun.sh DESTINATION ${LOCAL_TEST_DIR})
ENDIF() 

INSTALL(PROGRAMS runtimeTest.sh DESTINATION ${LOCAL_TEST_DIR})
INSTALL(TARGETS runtime_echo DESTINATION ${SALOME_YACS_INSTALL_TEST_LIB})
INSTALL(TARGETS TestRuntime DESTINATION ${LOCAL_TEST_DIR})
INSTALL(TARGETS runtimeTestEchoSrv DESTINATION ${LOCAL_TEST_DIR})
INSTALL(TARGETS echo_clt DESTINATION ${LOCAL_TEST_DIR})
INSTALL(FILES CTestTestfileInstall.cmake
        DESTINATION ${LOCAL_TEST_DIR}
        RENAME CTestTestfile.cmake)