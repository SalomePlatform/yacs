# Copyright (C) 2006-2012  CEA/DEN, EDF R&D
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

SET(GRAPHVIZ_LIBS)
IF(WINDOWS)
  SET(GRAPHVIZHOME $ENV{GRAPHVIZHOME})
  FIND_PATH(GRAPHVIZ_INCLUDE_DIR gvc.h ${GRAPHVIZHOME}/include/graphviz)
  FIND_LIBRARY(GVC_LIB gvc ${GRAPHVIZHOME}/bin)
  SET(GRAPHVIZ_LIBS ${GRAPHVIZ_LIBS} ${GVC_LIB})
  FIND_LIBRARY(GRAPH_LIB graph ${GRAPHVIZHOME}/bin)
  SET(GRAPHVIZ_LIBS ${GRAPHVIZ_LIBS} ${GRAPH_LIB})
ELSE(WINDOWS)
  SET(GRAPHVIZHOME $ENV{GRAPHVIZHOME})
  IF(NOT GRAPHVIZHOME)
    SET(GRAPHVIZHOME /usr)
  ENDIF(NOT GRAPHVIZHOME)
  FIND_PATH(GRAPHVIZ_INCLUDE_DIR gvc.h ${GRAPHVIZHOME}/include/graphviz)
  FIND_LIBRARY(GVC_LIB gvc ${GRAPHVIZHOME}/lib)
  SET(GRAPHVIZ_LIBS ${GRAPHVIZ_LIBS} ${GVC_LIB})
ENDIF(WINDOWS)

IF(GRAPHVIZ_INCLUDE_DIR)
  SET(GRAPHVIZ_CPPFLAGS -I${GRAPHVIZ_INCLUDE_DIR})
ENDIF(GRAPHVIZ_INCLUDE_DIR)
SET(GRAPHVIZ_LIBADD ${GRAPHVIZ_LIBS})
