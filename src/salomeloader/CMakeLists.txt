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

IF(WIN32)
  SET(salomeloader_SCRIPTS salomeloader.bat)
ELSE()
  SET(salomeloader_SCRIPTS salomeloader.sh)
ENDIF()

SET(salomeloader_PYTHON
  salomeloader.py
  ElementTree.py
  ElementPath.py
  graph.py
  )

# --- rules ---

INSTALL(PROGRAMS ${salomeloader_SCRIPTS} DESTINATION ${SALOME_INSTALL_BINS})

INSTALL(FILES ${salomeloader_PYTHON} DESTINATION ${SALOME_INSTALL_PYTHON})

FILE(GLOB samples ${PROJECT_SOURCE_DIR}/src/salomeloader/samples/*.xml)
INSTALL(FILES ${samples} DESTINATION ${SALOME_YACS_INSTALL_SUPERV_SAMPLES})
