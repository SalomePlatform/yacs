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

IF(SALOME_YACS_USE_KERNEL)
  SET(SALOME_INCL_PATH ${KERNEL_INCLUDE_DIRS})
ENDIF(SALOME_YACS_USE_KERNEL)

# additional include directories
INCLUDE_DIRECTORIES(
  ${OMNIORB_INCLUDE_DIR}
  ${LIBXML2_INCLUDE_DIR}
  ${SALOME_INCL_PATH}
  ${GUI_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/src/bases
  ${PROJECT_SOURCE_DIR}/src/engine
  ${PROJECT_SOURCE_DIR}/src/runtime
  ${PROJECT_SOURCE_DIR}/src/yacsloader
  ${PROJECT_SOURCE_DIR}/src/hmi
  ${PROJECT_SOURCE_DIR}/src/salomewrap
  ${PROJECT_SOURCE_DIR}/src/genericgui
  ${PROJECT_BINARY_DIR}/src/genericgui
  ${PROJECT_BINARY_DIR}/idl
  )

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${PYTHON_DEFINITIONS}
  ${OMNIORB_DEFINITIONS}
  ${BOOST_DEFINITIONS}
  ${QT_DEFINITIONS}
  ${GRAPHVIZ_DEFINITIONS}
  )

# libraries to link to
SET(_link_LIBRARIES
  GenericGui
  ${QT_LIBRARIES}
  ${GUI_Event}
  ${GUI_SalomeApp}
  ${GUI_SalomeSession}
  )

# --- resources ---

# resource files / to be processed by lrelease
SET(YACS_RESOURCES
  resources/YACS_icons.ts
  resources/YACS_msg_en.ts
  resources/YACS_msg_fr.ts
  resources/YACS_msg_ja.ts
)

SET(_res_files
  resources/YACSSchemaCatalog.xml
  resources/ModuleYacs.png
  resources/ExecYACS.png
  )

# --- headers ---

SET(YACS_HEADERS
  YACSExport.hxx
  Yacsgui.hxx
  )

# header files / to be processed by moc
SET(_moc_HEADERS
  Yacsgui.hxx
  )

# header files / uic wrappings
QT_WRAP_UIC(_uic_HEADERS ${_uic_files})

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

SET(YACS_SOURCES
  Yacsgui.cxx
  Yacsgui_DataModel.hxx
  Yacsgui_Resource.cxx
  Yacsgui_Resource.hxx
  Yacsgui_DataModel.cxx
  ${_moc_SOURCES}
  )

# --- rules ---

ADD_LIBRARY(YACS ${YACS_SOURCES})
TARGET_LINK_LIBRARIES(YACS ${_link_LIBRARIES})
INSTALL(TARGETS YACS EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${YACS_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
QT_INSTALL_TS_RESOURCES("${YACS_RESOURCES}" "${SALOME_YACS_INSTALL_RES_DATA}")	

INSTALL(FILES ${_res_files} DESTINATION ${SALOME_YACS_INSTALL_RES_DATA})
SALOME_CONFIGURE_FILE(resources/YACSCatalog.xml.in 
		      resources/YACSCatalog.xml 
		      INSTALL ${SALOME_YACS_INSTALL_RES_DATA})
SALOME_CONFIGURE_FILE(resources/SalomeApp.xml.in 
		      resources/SalomeApp.xml 
		      INSTALL ${SALOME_YACS_INSTALL_RES_DATA})
