# Copyright (C) 2012-2022  CEA/DEN, EDF R&D
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

INCLUDE_DIRECTORIES(
  ${CPPUNIT_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/..
)

SET(_link_LIBRARIES
  ${CPPUNIT_LIBRARIES} 
  py2yacslib
)

SET(Py2yacsTest_SOURCES
  TestPy2yacs.cxx
  Py2yacsTest.cxx
  Py2yacsTest.hxx
)

ADD_EXECUTABLE(TestPy2yacs ${Py2yacsTest_SOURCES})
TARGET_LINK_LIBRARIES(TestPy2yacs ${_link_LIBRARIES})

ADD_EXECUTABLE(test_py2yacs test_py2yacs.cxx)
TARGET_LINK_LIBRARIES(test_py2yacs py2yacslib)

# For salome test
IF(NOT WIN32)
  # This test needs a running salome session
  # TODO: Run the test in a salome session with make test
  #ADD_TEST(TestPy2yacs TestPy2yacs)
  #SET_TESTS_PROPERTIES(TestPy2yacs PROPERTIES
  #                     ENVIRONMENT "PYTHONPATH=${CMAKE_CURRENT_SOURCE_DIR}:${CMAKE_CURRENT_SOURCE_DIR}/..:$ENV{PYTHONPATH}"
  #                    )

  SET(LOCAL_TEST_DIR ${SALOME_YACS_INSTALL_TEST}/py2yacs)
  SET(LOCAL_TEST_FILES
    bad_parsers.py
    err_py2yacs_invalid.py
    testforeach.py
    formule.py
    )
  SET(LOCAL_TEST_SCRIPTS
    testDeco.py
    )
  INSTALL(FILES ${LOCAL_TEST_FILES}
          DESTINATION ${LOCAL_TEST_DIR})
  INSTALL(PROGRAMS ${LOCAL_TEST_SCRIPTS}
        DESTINATION ${LOCAL_TEST_DIR})
  INSTALL(FILES CTestTestfileInstall.cmake
          DESTINATION ${LOCAL_TEST_DIR}
          RENAME CTestTestfile.cmake)
  INSTALL(TARGETS TestPy2yacs test_py2yacs
          DESTINATION ${LOCAL_TEST_DIR})

ENDIF()
