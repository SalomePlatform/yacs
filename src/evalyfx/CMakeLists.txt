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

IF(SALOME_YACS_USE_KERNEL)
  INCLUDE_DIRECTORIES(${KERNEL_INCLUDE_DIRS})
  ADD_DEFINITIONS(${KERNEL_DEFINITIONS})
  SET(SALOME_LIBS 
    ${KERNEL_SalomeLifeCycleCORBA}
    ${KERNEL_SalomeIDLKernel}
    ${KERNEL_SalomeNS}
    ${KERNEL_SalomeContainer}
    ${KERNEL_SALOMEBasics}
    ${KERNEL_SalomeResourcesManager}
    ${KERNEL_OpUtil}
    ${KERNEL_SALOMELocalTrace}
    ${KERNEL_Registry}
    ${KERNEL_SalomeNotification}
    ${KERNEL_ResourcesManager}
    ${KERNEL_SalomeHDFPersist}
    ${KERNEL_SalomeGenericObj}
    )
ENDIF(SALOME_YACS_USE_KERNEL)

# additional include directories
INCLUDE_DIRECTORIES(
  ${OMNIORB_INCLUDE_DIR}
  ${PYTHON_INCLUDE_DIR}
  ${PROJECT_SOURCE_DIR}/src/bases
  ${PROJECT_SOURCE_DIR}/src/engine
  ${PROJECT_SOURCE_DIR}/src/runtime
  ${PROJECT_SOURCE_DIR}/src/yacsloader
  )

ADD_DEFINITIONS(
  ${PYTHON_DEFINITIONS}
  ${OMNIORB_DEFINITIONS}
  )

SET(YACSevalYFX_LIBRARIES
  YACSloader
  YACSRuntimeSALOME
  ${SALOME_LIBS}
  ${OMNIORB_LIBRARIES}
  ${PYTHON_LIBRARIES}
  ${LIBXML2_LIBRARIES}
  )

SET(YACSEvalYFX_HEADERS
  YACSEvalYFXExport.hxx
  YACSEvalYFX.hxx
  YACSEvalPort.hxx
  YACSEvalSeqAny.hxx
  YACSEvalResource.hxx
  YACSEvalSession.hxx
  YACSEvalSessionInternal.hxx
  YACSEvalObserver.hxx
  YACSEvalExecParams.hxx
  )

SET(YACSevalYFX_SOURCES
  YACSEvalYFX.cxx
  YACSEvalYFXPattern.cxx
  YACSEvalPort.cxx
  YACSEvalSeqAny.cxx
  YACSEvalResource.cxx
  YACSEvalSession.cxx
  YACSEvalSessionInternal.cxx
  YACSEvalObserver.cxx
  )

ADD_LIBRARY(YACSevalYFX ${YACSevalYFX_SOURCES})
TARGET_LINK_LIBRARIES(YACSevalYFX ${YACSevalYFX_LIBRARIES})
INSTALL(TARGETS YACSevalYFX EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})
INSTALL(FILES ${YACSEvalYFX_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
