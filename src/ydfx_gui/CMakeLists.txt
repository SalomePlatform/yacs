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

INCLUDE(UseQtExt)

# additional include directories
INCLUDE_DIRECTORIES(
  ${PYTHON_INCLUDE_DIR}
  ${QT_INCLUDES}
  ${KERNEL_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/src/bases
  ${PROJECT_SOURCE_DIR}/src/engine
  ${PROJECT_SOURCE_DIR}/src/runtime
  ${PROJECT_SOURCE_DIR}/src/yacsloader
  ${PROJECT_SOURCE_DIR}/src/evalyfx
  )

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${QT_DEFINITIONS}
  ${PYTHON_DEFINITIONS})

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


SET(ydfxwidgets_LIBRARIES
  YACSevalYFX
  ${QT_LIBRARIES}
  ${PYTHON_LIBRARIES}
  )

SET(_moc_HEADERS
  YDFXGUIPortsSelector.hxx
  YDFXGUIPushButtons.hxx
  YDFXGUIMain.hxx
  YDFXGUIWrap.hxx
  YDFXGUISeqInit.hxx
  YDFXGUIParametrizer.hxx
  YDFXGUIHostParametrizer.hxx
  )

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

SET(ydfxwidgets_HEADERS
  ydfxwidgetsExport.hxx
  YDFXGUIHostParametrizer.hxx
  YDFXGUIMain.hxx
  YDFXGUIParametrizer.hxx
  YDFXGUIPortsSelector.hxx
  YDFXGUIPortsValidator.hxx
  YDFXGUIPushButtons.hxx
  YDFXGUIPyThreadSaver.hxx
  YDFXGUISeqInit.hxx
  YDFXGUIWrap.hxx
  )

SET(ydfxwidgets_SOURCES
  YDFXGUIHostParametrizer.cxx
  YDFXGUIMain.cxx
  YDFXGUIParametrizer.cxx
  YDFXGUIPortsSelector.cxx
  YDFXGUIPortsValidator.cxx
  YDFXGUIPushButtons.cxx
  YDFXGUIPyThreadSaver.cxx
  YDFXGUISeqInit.cxx
  YDFXGUIWrap.cxx
  ${_moc_SOURCES}
  )

ADD_LIBRARY(ydfxwidgets ${ydfxwidgets_SOURCES})
TARGET_LINK_LIBRARIES(ydfxwidgets ${ydfxwidgets_LIBRARIES})
ADD_EXECUTABLE(ydfxgui ydfxgui.cxx)
TARGET_LINK_LIBRARIES(ydfxgui ydfxwidgets)

INSTALL(TARGETS ydfxwidgets EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})
INSTALL(TARGETS ydfxgui DESTINATION ${SALOME_INSTALL_BINS})
INSTALL(FILES ${ydfxwidgets_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
