# Copyright (C) 2012-2014  CEA/DEN, EDF R&D
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

# --- rules ---

IF(NOT WIN32)
  SET(SHELL /bin/sh)
  CONFIGURE_FILE(config_appli.xml.in config_appli.xml)
  CONFIGURE_FILE(YacsLoaderTest.sh.in YacsLoaderTest.sh)
  CONFIGURE_FILE(YacsLoaderInSessionTest.sh.in YacsLoaderInSessionTest.sh)
  ADD_TEST(NAME YacsLoaderTest_swig COMMAND ${SHELL} ${CMAKE_CURRENT_BINARY_DIR}/YacsLoaderTest.sh)
  ADD_TEST(NAME StdAloneYacsLoaderTest1 COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/StdAloneYacsLoaderTest1.py)
ENDIF() 
