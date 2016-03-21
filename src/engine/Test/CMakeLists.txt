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

# additional include directories
INCLUDE_DIRECTORIES(
  ${CPPUNIT_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/..
  ${CMAKE_CURRENT_SOURCE_DIR}/../../bases
  ${CMAKE_CURRENT_SOURCE_DIR}/../../bases/Test
)

# libraries to link to
SET(_link_LIBRARIES
  ${CPPUNIT_LIBRARIES} 
  ${PTHREAD_LIBRARIES}
  YACSlibEngine 
  YACSBases
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${OMNIORB_DEFINITIONS}
)

# --- sources ---

SET(PluginOptEvTest1_SOURCES
  PluginOptEvTest1.cxx
)
  
SET(TestEngine_SOURCES
  TestEngine.cxx
  RuntimeForEngineTest.cxx
  engineTest.cxx
)

SET(IntegrationTestEngine_SOURCES
  ToyNode.cxx
  ContainerTest.cxx
  ComponentInstanceTest.cxx
  IntegrationTestEngine.cxx
  engineIntegrationTest.cxx
  RuntimeForEngineIntegrationTest.cxx
)

# --- rules ---

ADD_LIBRARY(PluginOptEvTest1 ${PluginOptEvTest1_SOURCES})
TARGET_LINK_LIBRARIES(PluginOptEvTest1 YACSlibEngine)
INSTALL(TARGETS PluginOptEvTest1 EXPORT ${PROJECT_NAME}TargetGroup
        DESTINATION ${SALOME_YACS_INSTALL_TEST_LIB})

ADD_EXECUTABLE(TestEngine ${TestEngine_SOURCES})
TARGET_LINK_LIBRARIES(TestEngine ${_link_LIBRARIES})
ADD_TEST(TestEngine TestEngine)

ADD_EXECUTABLE(IntegrationTestEngine ${IntegrationTestEngine_SOURCES})
TARGET_LINK_LIBRARIES(IntegrationTestEngine ${_link_LIBRARIES})
SALOME_GENERATE_TESTS_ENVIRONMENT(tests_env)
ADD_TEST(IntegrationTestEngine IntegrationTestEngine)
SET_TESTS_PROPERTIES(IntegrationTestEngine PROPERTIES ENVIRONMENT "${tests_env}")

# For salome test
SET(LOCAL_TEST_DIR ${SALOME_YACS_INSTALL_TEST}/engine)
INSTALL(TARGETS TestEngine DESTINATION ${LOCAL_TEST_DIR})
INSTALL(TARGETS IntegrationTestEngine DESTINATION ${LOCAL_TEST_DIR})
INSTALL(FILES CTestTestfileInstall.cmake
        DESTINATION ${LOCAL_TEST_DIR}
        RENAME CTestTestfile.cmake)