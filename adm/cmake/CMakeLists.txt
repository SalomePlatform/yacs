# Copyright (C) 2012-2015  CEA/DEN, EDF R&D
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

# ===============================================================
# Files to be installed
# ===============================================================

# These files are data, module or lib files
SET(_adm_data
  FindQScintilla.cmake
  FindSalomeQScintilla.cmake
  FindSphinx.cmake
  FindSalomeSphinx.cmake
  FindYACS.cmake
  FindSalomeYACS.cmake
)
IF(NOT SALOME_YACS_USE_KERNEL)
  LIST(APPEND _adm_data
    FindGraphviz.cmake
    FindSalomeGraphviz.cmake
    FindCppUnit.cmake
    FindOmniORB.cmake
    FindOmniORBPy.cmake
    FindPThread.cmake
    FindSalomeBoost.cmake
    FindSalomeCppUnit.cmake
    FindSalomeDoxygen.cmake
    FindSalomeLibXml2.cmake
    FindSalomeOmniORB.cmake
    FindSalomeOmniORBPy.cmake
    FindSalomePThread.cmake
    FindSalomePythonLibs.cmake
    FindSalomePythonInterp.cmake
    FindSalomeNumPySciPy.cmake
    FindSalomeSWIG.cmake
    SalomeMacros.cmake
    SalomeSetupPlatform.cmake
    UseOmniORB.cmake
  )
ENDIF(NOT SALOME_YACS_USE_KERNEL)
INSTALL(FILES ${_adm_data} DESTINATION ${SALOME_INSTALL_CMAKE_LOCAL})
