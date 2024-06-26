# Copyright (C) 2012-2024  CEA, EDF
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

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${CPPUNIT_DEFINITIONS}
  ${OMNIORB_DEFINITIONS}
)

INCLUDE_DIRECTORIES(
  ${CPPUNIT_INCLUDE_DIRS}
  ${PTHREADS_INCLUDE_DIRS}
  ${OMNIORBPY_INCLUDE_DIR}
  ${LIBXML2_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/
  ${CMAKE_CURRENT_SOURCE_DIR}/..
  ${CMAKE_CURRENT_SOURCE_DIR}/../../bases
  ${CMAKE_CURRENT_SOURCE_DIR}/../../bases/Test
  ${CMAKE_CURRENT_SOURCE_DIR}/../../engine
  ${CMAKE_CURRENT_SOURCE_DIR}/../../runtime
)

IF(SALOME_YACS_USE_EMBEDDED_PMML)
  LIST(APPEND YACS_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/../../pmml )
ELSE(SALOME_YACS_USE_EMBEDDED_PMML)
  LIST(APPEND YACS_INCLUDE_DIRECTORIES ${PMML_ROOT_DIR}/include)
ENDIF(SALOME_YACS_USE_EMBEDDED_PMML)

INCLUDE_DIRECTORIES( ${YACS_INCLUDE_DIRECTORIES})

SET(TestYACSPMML_SOURCES
  TestYACSPMML.cxx
  YACSPMMLBasicsTest1.cxx
  )

ADD_EXECUTABLE(TestYACSPMML ${TestYACSPMML_SOURCES})


TARGET_LINK_LIBRARIES(TestYACSPMML pmmlLib   
                      YACSloader
                      YACSRuntimeSALOME
                      YACSlibEngine
                      YACSBases 
                      ${CPPUNIT_LIBRARIES} ${PLATFORM_LIBS})

IF(NOT WIN32)
  SET(SHELL /bin/sh)
  SALOME_CONFIGURE_FILE(config_appli.xml.in config_appli.xml)  
  SALOME_CONFIGURE_FILE(PmmlExeTest.sh.in PmmlExeTest.sh)  
  SALOME_CONFIGURE_FILE(PmmlInSessionTest.sh.in PmmlInSessionTest.sh)
  ADD_TEST(NAME PmmlExeTest COMMAND ${SHELL} PmmlExeTest.sh)
ENDIF()

SET(LOCAL_TEST_DIR ${SALOME_YACS_INSTALL_TEST}/yacsloader)
INSTALL(TARGETS TestYACSPMML DESTINATION ${LOCAL_TEST_DIR})
