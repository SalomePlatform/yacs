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

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${OMNIORB_INCLUDE_DIR}
  ${KERNEL_INCLUDE_DIRS}
  ${GUI_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/src/bases
  )

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${PYTHON_DEFINITIONS}
  ${OMNIORB_DEFINITIONS}
  ${BOOST_DEFINITIONS}
  ${QT_DEFINITIONS}
  ${KERNEL_DEFINITIONS}
  ${GUI_DEFINITIONS}
  )

# libraries to link to
SET(_link_LIBRARIES
  ${QT_LIBRARIES}
  ${OMNIORB_LIBRARIES}
  ${SALOME_LIBS}
  ${KERNEL_SalomeIDLKernel}
  ${GUI_CAM}
  ${GUI_suit}
  ${GUI_LightApp}
  ${GUI_qtx}
  ${GUI_QxScene}
  ${GUI_ObjBrowser}
  ${GUI_SalomeApp}
  YACSlibEngine
  )

# header files / to be processed by moc
SET(_moc_HEADERS
  SalomeWrap_Module.hxx
  WrapGraphicsView.hxx
  )

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

SET(SalomeWrap_SOURCES
  SalomeWrapExport.hxx
  SalomeWrap_Module.hxx
  SalomeWrap_Module.cxx
  SalomeWrap_DataModel.hxx
  SalomeWrap_DataModel.cxx
  SuitWrapper.hxx
  SuitWrapper.cxx
  WrapGraphicsView.hxx
  WrapGraphicsView.cxx
  ${_moc_SOURCES}
  )

# --- rules ---

ADD_LIBRARY(SalomeWrap ${SalomeWrap_SOURCES})
TARGET_LINK_LIBRARIES(SalomeWrap ${_link_LIBRARIES})
INSTALL(TARGETS SalomeWrap EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})
