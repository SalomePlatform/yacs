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
  pmml.rst
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
SET(SOURCEDIR ${CMAKE_CURRENT_SOURCE_DIR})
SET(PAPEROPT_a4 -D latex_paper_size=a4)
SET(ALLSPHINXOPTS -d doctrees ${PAPEROPT_a4} ${SPHINXOPTS} ${SOURCEDIR})

# install user's documentation

SALOME_CONFIGURE_FILE(conf.py.in conf.py)

ADD_CUSTOM_TARGET(htm 
  COMMAND ${CMAKE_COMMAND} -E make_directory html 
  COMMAND ${CMAKE_COMMAND} -E make_directory doctrees
  COMMAND ${SPHINX_EXECUTABLE} -c ${CMAKE_BINARY_DIR}/doc -b html ${ALLSPHINXOPTS} html
  DEPENDS ${RSTFILES}
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  )
INSTALL(CODE "EXECUTE_PROCESS(COMMAND \"${CMAKE_COMMAND}\" --build ${PROJECT_BINARY_DIR} --target htm)")
INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html/ 
  DESTINATION ${SALOME_INSTALL_DOC}/gui/YACS 
  USE_SOURCE_PERMISSIONS
  PATTERN ".buildinfo" EXCLUDE
  )
  
SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES html)
SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES doctrees)

# install developer's documentation

SALOME_CONFIGURE_FILE(Doxyfile.in Doxyfile)
SALOME_CONFIGURE_FILE(sources/header.html.in sources/header.html)

ADD_CUSTOM_TARGET(dev_docs ${DOXYGEN_EXECUTABLE})

INSTALL(CODE "FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/YACS/htmldev)")
INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/YACS/htmldev/ DESTINATION ${SALOME_INSTALL_DOC}/tui/YACS)
INSTALL(FILES images/head.png DESTINATION ${SALOME_INSTALL_DOC}/tui/YACS) 
INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/src/yacsloader/samples 
        DESTINATION ${SALOME_INSTALL_DOC}/tui/YACS
        PATTERN "CVS" EXCLUDE
        PATTERN "*.md5" EXCLUDE
        PATTERN "*.map" EXCLUDE
        )
SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES YACS)
