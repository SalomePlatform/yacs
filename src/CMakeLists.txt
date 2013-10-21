# Copyright (C) 2012-2013  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
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
  bases engine runtime wrappergen yacsloader yacsorb salomeloader
  )

##
# SWIG wrapping
##
IF(SALOME_YACS_USE_SWIG)
  LIST(APPEND SUBDIRS_COMMON
    engine_swig runtime_swig yacsloader_swig
    )
ENDIF()

##
# GUI
##
IF(SALOME_BUILD_GUI)
  SET(SUBDIRS_GUI
    pyqt hmi salomewrap genericgui salomegui
    )
  IF(SALOME_YACS_USE_SWIG)
    LIST(APPEND SUBDIRS_GUI
      salomegui_swig
      )
  ENDIF()
ENDIF()

SET(SUBDIRS
  ${SUBDIRS_COMMON}
  ${SUBDIRS_GUI}
)

FOREACH(dir ${SUBDIRS})
  ADD_SUBDIRECTORY(${dir})
ENDFOREACH(dir ${SUBDIRS})
