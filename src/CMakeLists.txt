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

##
# Common packages
##
SET(SUBDIRS_COMMON
  bases engine wrappergen yacsorb salomeloader
  pmml
  )

##
# KERNEL
##
IF(SALOME_YACS_USE_KERNEL)
  SET(SUBDIRS_KERNEL runtime yacsloader evalyfx)
ENDIF(SALOME_YACS_USE_KERNEL)

##
# SWIG wrapping
##
IF(SALOME_YACS_USE_SWIG)
  SET(SUBDIRS_SWIG engine_swig)
  IF(SALOME_YACS_USE_KERNEL)
    LIST(APPEND SUBDIRS_SWIG runtime_swig yacsloader_swig evalyfx_swig)
  ENDIF(SALOME_YACS_USE_KERNEL)
ENDIF(SALOME_YACS_USE_SWIG)

##
# GUI
##
IF(SALOME_BUILD_GUI)
  SET(SUBDIRS_GUI ydfx_gui pyqt hmi salomewrap genericgui salomegui)
  IF(SALOME_YACS_USE_SWIG)
    LIST(APPEND SUBDIRS_GUI salomegui_swig)
  ENDIF()
ENDIF()

# For salome test
IF(SALOME_BUILD_TESTS)
  SET(SALOME_YACS_INSTALL_TEST_LIB ${SALOME_YACS_INSTALL_TEST}/lib)
  INSTALL(FILES CTestTestfileInstall.cmake
          DESTINATION ${SALOME_YACS_INSTALL_TEST}
          RENAME CTestTestfile.cmake)
ENDIF(SALOME_BUILD_TESTS)

SET(SUBDIRS
  ${SUBDIRS_COMMON}
  ${SUBDIRS_KERNEL}
  ${SUBDIRS_SWIG}
  ${SUBDIRS_GUI}
)

FOREACH(dir ${SUBDIRS})
  ADD_SUBDIRECTORY(${dir})
ENDFOREACH(dir ${SUBDIRS})
