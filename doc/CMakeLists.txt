# Copyright (C) 2012-2013  CEA/DEN, EDF R&D, OPEN CASCADE
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

INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/SalomeMacros.cmake)

SET(top_builddir ${CMAKE_BINARY_DIR})
SET(top_srcdir ${CMAKE_SOURCE_DIR})
SET(srcdir ${CMAKE_CURRENT_SOURCE_DIR})
SET(builddir ${CMAKE_CURRENT_BINARY_DIR})
SET(datadir ${CMAKE_INSTALL_PREFIX}/share)
SET(docdir ${datadir}/doc/salome)
SET(yacsdocdir ${docdir}/gui/YACS)
SET(yacsdevdocdir ${docdir}/tui/YACS)

SET(RSTFILES
  accesCorba.rst
  accesLocal.rst
  advancepy.rst
  annexe_common.rst
  batch.rst
  calcium.rst
  calciumyacs.rst
  calciumapi.rst
  calciummod.rst
  calculator.rst
  compInterne.rst
  components.rst
  console.rst
  cppsalome.rst
  dataflow.rst
  etapes.rst
  execpy.rst
  execution.rst
  execxml.rst
  gui_components.rst
  gui.rst
  hxx2salome.rst
  index.rst
  integration.rst
  using.rst
  main_menu.rst
  modification.rst
  operations.rst
  principes.rst
  progdsc.rst
  pysalome.rst
  python.rst
  rappels.rst
  references.rst
  representation.rst
  salome.rst
  schemapy.rst
  schemaxml.rst
  scratch.rst
  toolbars.rst
  optimizer.rst
  yacsgen.rst
  )

SET(SPHINXOPTS )
SET(SOURCEDIR ${srcdir})
SET(PAPEROPT_a4 -D latex_paper_size=a4)
SET(ALLSPHINXOPTS -d doctrees ${PAPEROPT_a4} ${SPHINXOPTS} ${SOURCEDIR})

SALOME_CONFIGURE_FILE(Doxyfile.in Doxyfile)
SALOME_CONFIGURE_FILE(conf.py.in conf.py)
SALOME_CONFIGURE_FILE(sources/header.html.in sources/header.html)

# install user's documentation
ADD_CUSTOM_TARGET(htm 
  COMMAND mkdir -p html doctrees
  COMMAND ${SPHINX_EXECUTABLE} -c ${top_builddir}/doc -b html ${ALLSPHINXOPTS} html
  DEPENDS ${RSTFILES}
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  )
INSTALL(CODE "EXECUTE_PROCESS(COMMAND \"${CMAKE_COMMAND}\" --build ${PROJECT_BINARY_DIR} --target htm)")
INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html/ 
  DESTINATION ${yacsdocdir} 
  USE_SOURCE_PERMISSIONS
  PATTERN ".buildinfo" EXCLUDE
  )

# install developer's documentation
ADD_CUSTOM_TARGET(dev_docs
  COMMAND ${DOXYGEN_EXECUTABLE} -u
  COMMAND ${DOXYGEN_EXECUTABLE}
  COMMAND mkdir -p ${yacsdevdocdir}
  COMMAND cp -fv ${srcdir}/images/head.png ${yacsdevdocdir}/head.png
  COMMAND find "${builddir}/htmldev" -name \"*.*\" -exec cp -rfv {} ${yacsdevdocdir} '\;'
  COMMAND cp -rfv ${top_srcdir}/src/yacsloader/samples ${yacsdevdocdir}
  COMMAND rm -rf ${yacsdevdocdir}/samples/CVS
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  )
