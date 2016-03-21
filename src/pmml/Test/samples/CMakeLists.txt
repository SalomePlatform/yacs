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

SET(PMML_RESOURCES_FILES
  # ici les noms des fichiers ressources 
  ann_model.pmml
  ann_model_2.pmml
  lr_model.pmml
  lr_model_2.pmml
  no_model.pmml
  two_models_ann_lr.pmml
  unittest_ref_ann_model.cpp
  unittest_ref_ann_model.f
  unittest_ref_ann_model.py
  unittest_ref_lr_model.cpp
  unittest_ref_lr_model.f
  unittest_ref_lr_model.py
  win32_ann_model.pmml
  win32_lr_model.pmml 
  )

INSTALL(FILES ${PMML_RESOURCES_FILES} DESTINATION ${SALOME_PMML_INSTALL_RES_DATA})

# MESSAGE(STATUS "Creation of ${CMAKE_CURRENT_BINARY_DIR}/PMMLCatalog.xml")
# CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/PMMLCatalog.xml.in ${CMAKE_CURRENT_BINARY_DIR}/PMMLCatalog.xml @ONLY)
# MESSAGE(STATUS "Creation of ${CMAKE_CURRENT_BINARY_DIR}/SalomeApp.xml")
# CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/SalomeApp.xml.in ${CMAKE_CURRENT_BINARY_DIR}/SalomeApp.xml @ONLY)

# INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/PMMLCatalog.xml ${CMAKE_CURRENT_BINARY_DIR}/SalomeApp.xml DESTINATION ${SALOME_PMML_INSTALL_RES_DATA})
