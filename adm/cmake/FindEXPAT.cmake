# Copyright (C) 2006-2011  CEA/DEN, EDF R&D
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

IF(WINDOWS)
  SET(EXPATHOME $ENV{EXPATHOME})
  FIND_LIBRARY(EXPAT_LIB libexpat PATHS ${EXPATHOME}/bin ${EXPATHOME}/Bin)
  FIND_PATH(EXPAT_INCLUDE_DIR expat.h PATHS ${EXPATHOME}/include ${EXPATHOME}/Source/lib)
ELSE(WINDOWS)
  FIND_LIBRARY(EXPAT_LIB expat)
ENDIF(WINDOWS)

IF(EXPAT_INCLUDE_DIR)
  SET(EXPAT_INCLUDES -I${EXPAT_INCLUDE_DIR})
ENDIF(EXPAT_INCLUDE_DIR)
SET(EXPAT_LIBS ${EXPAT_LIB})
