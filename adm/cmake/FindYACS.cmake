# Copyright (C) 2013-2016  CEA/DEN, EDF R&D
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

SET(YACS_CXXFLAGS -I${YACS_ROOT_DIR}/include/salome) # to be removed
SET(YACS_INCLUDE_DIRS ${YACS_ROOT_DIR}/include/salome)

FIND_LIBRARY(YACS_YACSloader YACSloader ${YACS_ROOT_DIR}/lib/salome)
FIND_LIBRARY(YACS_YACSBases YACSBases ${YACS_ROOT_DIR}/lib/salome)
FIND_LIBRARY(YACS_YACSlibEngine YACSlibEngine ${YACS_ROOT_DIR}/lib/salome)
FIND_LIBRARY(YACS_YACSRuntimeSALOME YACSRuntimeSALOME ${YACS_ROOT_DIR}/lib/salome)
FIND_LIBRARY(YACS_YACSDLTest YACSDLTest ${YACS_ROOT_DIR}/lib/salome)
FIND_LIBRARY(YACS_SalomeIDLYACS SalomeIDLYACS ${YACS_ROOT_DIR}/lib/salome)
FIND_LIBRARY(YACS_TestComponentLocal TestComponentLocal ${YACS_ROOT_DIR}/lib/salome)
FIND_LIBRARY(YACS_PluginSimplex PluginSimplex ${YACS_ROOT_DIR}/lib/salome)
FIND_LIBRARY(YACS_PluginOptEvTest1 PluginOptEvTest1 ${YACS_ROOT_DIR}/lib/salome)
FIND_LIBRARY(YACS_HMI HMI ${YACS_ROOT_DIR}/lib/salome)
FIND_LIBRARY(YACS_GenericGui GenericGui ${YACS_ROOT_DIR}/lib/salome)
FIND_LIBRARY(YACS_YACS YACS ${YACS_ROOT_DIR}/lib/salome)
FIND_LIBRARY(YACS_SalomeWrap SalomeWrap ${YACS_ROOT_DIR}/lib/salome)
