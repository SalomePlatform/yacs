# Copyright (C) 2012-2024  CEA, EDF
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

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR}/..
  ${CMAKE_CURRENT_SOURCE_DIR}/../../bases
)

# --- sources ---

SET(PluginSimplex_SOURCES
  PluginSimplex.cxx
  maestro.cxx 
  decode.cxx 
  point.cxx 
  aleas.cxx
  salomevent.cxx 
  simplex.cxx 
  solution.cxx
  local.cxx 
  mt19937ar.cxx 
)

# --- rules ---
if(WIN32)
  add_library(PluginSimplex_lib STATIC  ${PluginSimplex_SOURCES})
  TARGET_LINK_LIBRARIES(PluginSimplex_lib ${_link_LIBRARIES})
  set_target_properties(PluginSimplex_lib PROPERTIES OUTPUT_NAME PluginSimplex)
endif(WIN32)
ADD_LIBRARY(PluginSimplex ${PluginSimplex_SOURCES})
TARGET_LINK_LIBRARIES(PluginSimplex YACSlibEngine)
INSTALL(TARGETS PluginSimplex EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})
