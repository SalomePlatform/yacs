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
)

# --- sources ---

SET(YACSDLTest_SOURCES
  DLTest.cxx
)
  
SET(InitTests_SOURCES
  InitTests.cxx
)

SET(TestBases_SOURCES
  TestBases.cxx
  basesTest.cxx
)

# --- rules ---

ADD_LIBRARY(YACSDLTest ${YACSDLTest_SOURCES})
INSTALL(TARGETS YACSDLTest EXPORT ${PROJECT_NAME}TargetGroup 
        DESTINATION ${SALOME_YACS_INSTALL_TEST_LIB})

ADD_EXECUTABLE(InitTests ${InitTests_SOURCES})
ADD_TEST(InitTests InitTests)

ADD_EXECUTABLE(TestBases ${TestBases_SOURCES})
TARGET_LINK_LIBRARIES(TestBases YACSBases ${CPPUNIT_LIBRARIES} ${PTHREAD_LIBRARIES})
SALOME_GENERATE_TESTS_ENVIRONMENT(tests_env)
ADD_TEST(TestBases TestBases)
SET_TESTS_PROPERTIES(TestBases PROPERTIES ENVIRONMENT "${tests_env}")

# For salome test
SET(LOCAL_TEST_DIR ${SALOME_YACS_INSTALL_TEST}/bases)
INSTALL(TARGETS InitTests DESTINATION ${LOCAL_TEST_DIR})
INSTALL(TARGETS TestBases DESTINATION ${LOCAL_TEST_DIR})
INSTALL(FILES CTestTestfileInstall.cmake
        DESTINATION ${LOCAL_TEST_DIR}
        RENAME CTestTestfile.cmake)
