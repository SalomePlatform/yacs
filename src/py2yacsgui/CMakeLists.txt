# Copyright (C) 2012-2026  CEA, EDF
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
  ${QT_INCLUDES}
  ${PROJECT_SOURCE_DIR}/src/bases
  ${PROJECT_SOURCE_DIR}/src/engine
  ${PROJECT_SOURCE_DIR}/src/runtime
  ${PROJECT_SOURCE_DIR}/src/py2yacs
  ${PROJECT_BINARY_DIR}/src/py2yacs
  ${GUI_INCLUDE_DIRS}
  ${OMNIORB_INCLUDE_DIR}
  ${OMNIORB_ROOT_DIR}/include/omniORB4/internal
  ${OMNIORBPY_INCLUDE_DIR}
  )

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${QT_DEFINITIONS}
  ${GUI_DEFINITIONS}
  ${OMNIORB_DEFINITIONS}
  ${PYTHON_DEFINITIONS})

SET(_link_LIBRARIES
  ${QT_LIBRARIES}
  ${GUI_LightApp} # for PyEditor
  py2yacslib
  )

SET(_moc_HEADERS
  Py2YacsDialog.hxx
  Py2YacsModel.hxx
  Py2YacsDialog_raw.hxx
  )

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

SET(py2yacsgui_HEADERS
  Py2YacsModel.hxx
  Py2YacsDialog.hxx
  config_py2yacsgui.hxx
  Py2YacsDialog_raw.hxx
  )

SET(py2yacsgui_SOURCES
  Py2YacsDialog.cxx
  Py2YacsModel.cxx
  Py2YacsDialog_raw.cxx
  ${_moc_SOURCES}
  )

ADD_LIBRARY(py2yacsguilib ${py2yacsgui_SOURCES})
TARGET_LINK_LIBRARIES(py2yacsguilib ${_link_LIBRARIES})
ADD_EXECUTABLE(py2yacsgui py2yacsgui.cxx)
TARGET_LINK_LIBRARIES(py2yacsgui py2yacsguilib)

ADD_EXECUTABLE(py2yacsgui_rich py2yacsgui_rich.cxx)
TARGET_LINK_LIBRARIES(py2yacsgui_rich py2yacsguilib)

INSTALL(TARGETS py2yacsguilib EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})
INSTALL(TARGETS py2yacsgui DESTINATION ${SALOME_INSTALL_BINS})
INSTALL(TARGETS py2yacsgui_rich DESTINATION ${SALOME_INSTALL_BINS})
INSTALL(FILES ${py2yacsgui_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
