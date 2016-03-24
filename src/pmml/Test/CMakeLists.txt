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
# Author : InckA

#
# TODO  : URANIE AND WIN32 : to compile on Windows, user uranietm 
#         
#         To adapt when YACS will be available on Windows
# 

IF(URANIE AND WIN32) 
    # Find the CppUnit includes and library
    #
    # This module defines
    # CPPUNIT_INCLUDE_DIR, where to find tiff.h, etc.
    # CPPUNIT_LIBRARIES, the libraries to link against to use CppUnit.
    # CPPUNIT_FOUND, If false, do not try to use CppUnit.

    # also defined, but not for general use are
    # CPPUNIT_LIBRARY, where to find the CppUnit library.
    # CPPUNIT_DEBUG_LIBRARY, where to find the CppUnit library in debug mode.

    FIND_PATH(CPPUNIT_INCLUDE_DIR cppunit/TestCase.h)

    # With Win32, important to have both

    FIND_LIBRARY(CPPUNIT_LIBRARY cppunit
                    ${CPPUNIT_INCLUDE_DIR}/../lib)
    FIND_LIBRARY(CPPUNIT_DEBUG_LIBRARY cppunitd
                    ${CPPUNIT_INCLUDE_DIR}/../lib)

    IF(CPPUNIT_INCLUDE_DIR)
      IF(CPPUNIT_LIBRARY)
        SET(CPPUNIT_FOUND "YES")
        SET(CPPUNIT_LIBRARIES ${CPPUNIT_LIBRARY} ${CMAKE_DL_LIBS})
        SET(CPPUNIT_DEBUG_LIBRARIES ${CPPUNIT_DEBUG_LIBRARY} ${CMAKE_DL_LIBS})
      ENDIF(CPPUNIT_LIBRARY)
    ENDIF(CPPUNIT_INCLUDE_DIR)
         
ELSE(URANIE AND WIN32) 
    FIND_PACKAGE(SalomeCppUnit)
    SALOME_LOG_OPTIONAL_PACKAGE(CppUnit SALOME_BUILD_TESTS)	
ENDIF(URANIE AND WIN32) 

ADD_DEFINITIONS(${CPPUNIT_DEFINITIONS})

IF(URANIE AND WIN32) 
    INCLUDE_DIRECTORIES(
      ${CPPUNIT_INCLUDE_DIR}  
      ${CMAKE_CURRENT_SOURCE_DIR}/ 
      ${CMAKE_CURRENT_SOURCE_DIR}/.. 
      )   
ELSE(URANIE AND WIN32) 
    INCLUDE_DIRECTORIES(
      ${CPPUNIT_INCLUDE_DIRS}
      ${CMAKE_CURRENT_SOURCE_DIR}/
      ${CMAKE_CURRENT_SOURCE_DIR}/..
      )      
ENDIF(URANIE AND WIN32) 

SET(TestPMML_SOURCES
  TestPMML.cxx
  PMMLBasicsTest1.cxx
  tools.cxx
  )

ADD_EXECUTABLE(TestPMML ${TestPMML_SOURCES})

IF(URANIE AND WIN32) 
    IF(SALOME_CMAKE_DEBUG)
		TARGET_LINK_LIBRARIES(TestPMML pmmlLib ${CPPUNIT_DEBUG_LIBRARY} ${PLATFORM_LIBS})
    ELSE(SALOME_CMAKE_DEBUG)
		TARGET_LINK_LIBRARIES(TestPMML pmmlLib ${CPPUNIT_LIBRARY} ${PLATFORM_LIBS})
	ENDIF(SALOME_CMAKE_DEBUG)	
ELSE(URANIE AND WIN32) 
    TARGET_LINK_LIBRARIES(TestPMML pmmlLib  
                          ${CPPUNIT_LIBRARIES} ${PLATFORM_LIBS})
ENDIF(URANIE AND WIN32) 

ADD_TEST(TestPMML TestPMML)

FILE(COPY ${PROJECT_SOURCE_DIR}/src/pmml/Test/samples  DESTINATION . )
INSTALL(TARGETS TestPMML DESTINATION ${SALOME_INSTALL_BINS})

#

SET(LOCAL_TEST_DIR ${SALOME_YACS_INSTALL_TEST}/pmml)
FILE(COPY ${PROJECT_SOURCE_DIR}/src/pmml/Test/samples DESTINATION "${CMAKE_INSTALL_PREFIX}/${LOCAL_TEST_DIR}")
INSTALL(TARGETS TestPMML DESTINATION ${LOCAL_TEST_DIR})
INSTALL(FILES CTestTestfileInstall.cmake DESTINATION ${LOCAL_TEST_DIR} RENAME CTestTestfile.cmake)
