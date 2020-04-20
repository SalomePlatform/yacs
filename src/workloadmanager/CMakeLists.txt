# Copyright (C) 2020  CEA/DEN, EDF R&D
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

SET (_link_LIBRARIES
  #Threads::Threads
  ${PTHREAD_LIBRARIES}
)

SET (_wlm_sources
  Task.cxx
  WorkloadManager.cxx
  DefaultAlgorithm.cxx
)

SET (_wlm_headers
  Task.hxx
  WorkloadManager.hxx
  WorkloadAlgorithm.hxx
  DefaultAlgorithm.hxx
)

ADD_LIBRARY(YACSlibWorkloadmanager ${_wlm_sources})
TARGET_LINK_LIBRARIES(YACSlibWorkloadmanager ${_link_LIBRARIES})
INSTALL(TARGETS YACSlibWorkloadmanager EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})
INSTALL(FILES ${_wlm_headers} DESTINATION ${SALOME_INSTALL_HEADERS}/workloadmanager)

IF(SALOME_BUILD_TESTS)
  ADD_SUBDIRECTORY(Test)
ENDIF(SALOME_BUILD_TESTS)
