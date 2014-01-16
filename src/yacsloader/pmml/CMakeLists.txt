# Copyright (C) 2012-2013  CEA/DEN, EDF R&D
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
# Author : InckA

ADD_DEFINITIONS(${CPPUNIT_DEFINITIONS})


INCLUDE_DIRECTORIES(
  ${CPPUNIT_INCLUDE_DIRS}
  ${PTHREADS_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/
  ${CMAKE_CURRENT_SOURCE_DIR}/..
  ${CMAKE_CURRENT_SOURCE_DIR}/../../bases
  ${CMAKE_CURRENT_SOURCE_DIR}/../../bases/Test
  ${CMAKE_CURRENT_SOURCE_DIR}/../../engine
  ${CMAKE_CURRENT_SOURCE_DIR}/../../runtime
  ${CMAKE_CURRENT_SOURCE_DIR}/../../pmml
  )


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

ADD_TEST(TestYACSPMML TestYACSPMML)

INSTALL(TARGETS TestYACSPMML DESTINATION ${SALOME_INSTALL_BINS})