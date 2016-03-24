# Copyright (C) 2006-2016  CEA/DEN, EDF R&D
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

SET(HXX2SALOMESCRIPTS
  hxx2salome
  hxx2salome_check
  hxx2salome_cpp
  hxx2salome_corba
  parse01.awk
  parse0.awk
  parse1.awk
  parse2.awk
  parse30.awk
  parse3.awk
  parse4.awk
  parse5.awk
  renameSalomeModule
  runIDLparser
  compile_HXX2SALOME_GENERIC_CLASS_NAME.sh
  )

SET(ENV_SH  ${CMAKE_CURRENT_BINARY_DIR}/env_HXX2SALOME.sh)
SET(ENV_CSH ${CMAKE_CURRENT_BINARY_DIR}/env_HXX2SALOME.csh)

INSTALL(FILES ${HXX2SALOMESCRIPTS} 
  DESTINATION ${SALOME_INSTALL_HXX2SALOME} 
  )

INSTALL(DIRECTORY HXX2SALOME_GENERIC_CLASS_NAME_SRC
  DESTINATION ${SALOME_INSTALL_HXX2SALOME} 
  USE_SOURCE_PERMISSIONS
  PATTERN "CVS" EXCLUDE
  )

FILE(WRITE ${ENV_SH} 
  "#------ HXX2SALOME ------\n"
  "export HXX2SALOME_ROOT_DIR=${CMAKE_INSTALL_PREFIX}/bin/HXX2SALOME_Test\n"
  "export PATH=\${HXX2SALOME_ROOT_DIR}:\${PATH}\n"
  "##\n"
  )
FILE(WRITE ${ENV_CSH} 
  "#------ HXX2SALOME ------\n"
  "setenv HXX2SALOME_ROOT_DIR ${CMAKE_INSTALL_PREFIX}/bin/HXX2SALOME_Test\n"
  "setenv PATH \${HXX2SALOME_ROOT_DIR}:\${PATH}\n"
  "##\n"
  )
INSTALL(FILES ${ENV_SH} ${ENV_CSH} DESTINATION ${SALOME_INSTALL_HXX2SALOME})
