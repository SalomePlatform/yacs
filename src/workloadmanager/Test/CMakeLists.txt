# Copyright (C) 2020-2022  CEA/DEN, EDF R&D
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

SET(_link_LIBRARIES
  ${CPPUNIT_LIBRARIES}
  ${PTHREAD_LIBRARIES}
  YACSlibWorkloadmanager
)

ADD_EXECUTABLE(WorkloadManagerTest TestMain.cxx)
TARGET_LINK_LIBRARIES(WorkloadManagerTest ${_link_LIBRARIES})
ADD_TEST(WorkloadManager WorkloadManagerTest)

# For salome test
SET(LOCAL_TEST_DIR ${SALOME_YACS_INSTALL_TEST}/workloadmanager)
INSTALL(TARGETS WorkloadManagerTest DESTINATION ${LOCAL_TEST_DIR})
INSTALL(FILES CTestTestfileInstall.cmake
        DESTINATION ${LOCAL_TEST_DIR}
        RENAME CTestTestfile.cmake)
