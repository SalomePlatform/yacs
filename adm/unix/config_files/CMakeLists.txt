# Copyright (C) 2007-2016  CEA/DEN, EDF R&D
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

SET(_m4_data
  ac_cxx_option.m4
  ac_linker_options.m4
  ac_pkg_swig.m4
  ac_python_devel.m4
  check_boost.m4
  check_cas.m4
  check_cppunit.m4
  check_htmlgen.m4
  check_libxml.m4
  check_omniorb.m4
  check_salome.m4
  check_sphinx.m4
  i2_check_qt4.m4
  production.m4
  thread.m4
  )

INSTALL(FILES ${_m4_data} DESTINATION ${SALOME_INSTALL_AMCONFIG_LOCAL}/config_files)
