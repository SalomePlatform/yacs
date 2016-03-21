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

SET(indoxfiles Doxyfile_pmml_user.in static/header.html.in)
SET(builddir ${CMAKE_CURRENT_BINARY_DIR})
SET(srcdir   ${CMAKE_CURRENT_SOURCE_DIR})
FOREACH(indoxfile ${indoxfiles})
  STRING(REGEX REPLACE ".in" "" baseindoxfile ${indoxfile})
  SET(input ${CMAKE_CURRENT_SOURCE_DIR}/${indoxfile})
  SET(output ${CMAKE_CURRENT_BINARY_DIR}/${baseindoxfile})
  CONFIGURE_FILE(${input} ${output})
  MESSAGE(STATUS "Creation of ${output}")
ENDFOREACH(indoxfile ${indoxfiles})
FILE(TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}" output)


# :TRICKY: For ease of maintenance, documentation for code examples is
# splitted in several files. We here splice to a single file before running
# Doxygen.

# Here is the "standard" procedure, as if ${input} was hand-written.
ADD_CUSTOM_TARGET(usr_docs ALL
  COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile_pmml_user
  VERBATIM
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  )
