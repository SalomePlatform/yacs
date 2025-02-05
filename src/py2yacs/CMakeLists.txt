# Copyright (C) 2012-2025  CEA, EDF
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

INCLUDE_DIRECTORIES(
  ${PYTHON_INCLUDE_DIR}
  ${OMNIORBPY_INCLUDE_DIR}
  ${SALOME_INCL_PATH}
  ${PROJECT_SOURCE_DIR}/src/bases
  ${PROJECT_SOURCE_DIR}/src/engine
  ${PROJECT_SOURCE_DIR}/src/runtime
  ${PROJECT_SOURCE_DIR}
  )

ADD_DEFINITIONS(
  ${PYTHON_DEFINITIONS}
  ${OMNIORB_DEFINITIONS}
  )

IF(SALOME_BUILD_TESTS)
  ADD_SUBDIRECTORY(Test)
ENDIF(SALOME_BUILD_TESTS)

SET(_link_LIBRARIES
  ${PYTHON_LIBRARIES}
  YACSRuntimeSALOME
  )

SET(_py2yacs_sources
  py2yacs.cxx
  )

SET(_py2yacs_headers
  py2yacs.hxx
  )

ADD_LIBRARY(py2yacslib ${_py2yacs_sources})
TARGET_LINK_LIBRARIES(py2yacslib ${_link_LIBRARIES})

INSTALL(TARGETS py2yacslib EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})
INSTALL(FILES ${_py2yacs_headers} DESTINATION ${SALOME_INSTALL_HEADERS})

SET( _py_SCRIPTS
  py2yacs.py
  yacsdecorator.py
  yacstools.py
  )

SET(_py_PROGS
  yacsbuild.py
  )

SALOME_INSTALL_SCRIPTS("${_py_SCRIPTS}"  ${SALOME_INSTALL_PYTHON})
SALOME_INSTALL_SCRIPTS("${_py_PROGS}" ${SALOME_INSTALL_SCRIPT_SCRIPTS})
