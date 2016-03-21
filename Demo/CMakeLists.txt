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

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${OMNIORB_INCLUDE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )

SET(echo_IDLSOURCES
  echo.idl
)

SET(IDL_INCLUDE_DIRS
  ${KERNEL_ROOT_DIR}/idl/salome
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)
SET(IDL_LINK_FLAGS
  ${KERNEL_SalomeIDLKernel}
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${OMNIORB_DEFINITIONS}
  )

# libraries to link to
SET(_link_LIBRARIES
  ${OMNIORB_LIBRARIES}
  
  )

# --- sources ---

SET(echoSrvDemo_SOURCES
  echoSrv.cxx
  echoSK.cc
  )

# --- rules ---

OMNIORB_ADD_MODULE(echo "${echo_IDLSOURCES}" "${IDL_INCLUDE_DIRS}" "${IDL_LINK_FLAGS}")

ADD_EXECUTABLE(echoSrvDemo ${echoSrvDemo_SOURCES})
TARGET_LINK_LIBRARIES(echoSrvDemo ${_link_LIBRARIES})
INSTALL(TARGETS echoSrvDemo EXPORT ${PROJECT_NAME}TargetGroup
DESTINATION ${SALOME_INSTALL_BINS})

ADD_DEPENDENCIES(echoSrvDemo echo)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/schema_orig.xml schema.xml)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/xmlrpcprog_orig.py xmlrpcprog.py)
