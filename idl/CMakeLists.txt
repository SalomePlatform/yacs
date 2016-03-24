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

INCLUDE(UseOmniORB)

IF(SALOME_YACS_USE_KERNEL)
  SET(SALOME_INCL_PATH ${KERNEL_INCLUDE_DIRS})
ENDIF(SALOME_YACS_USE_KERNEL)

INCLUDE_DIRECTORIES(
  ${SALOME_INCL_PATH}
  ${OMNIORB_INCLUDE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

SET(SalomeIDLYACS_IDLSOURCES
  yacsgui.idl
)

SET(IDL_INCLUDE_DIRS
  ${KERNEL_ROOT_DIR}/idl/salome
  ${CMAKE_CURRENT_SOURCE_DIR}
)
SET(IDL_LINK_FLAGS
  ${KERNEL_SalomeIDLKernel}
)

OMNIORB_ADD_MODULE(SalomeIDLYACS "${SalomeIDLYACS_IDLSOURCES}" "${IDL_INCLUDE_DIRS}" "${IDL_LINK_FLAGS}")
INSTALL(TARGETS SalomeIDLYACS EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})
