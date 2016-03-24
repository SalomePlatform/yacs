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


INCLUDE(${SWIG_USE_FILE})

ADD_DEFINITIONS(${PYTHON_DEFINITIONS} ${NUMPY_DEFINITIONS})

SET_SOURCE_FILES_PROPERTIES(PMMLsalome.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(PMMLsalome.i PROPERTIES SWIG_DEFINITIONS "-shadow")

SET(SWIG_MODULE_PMML_EXTRA_FLAGS "${NUMPY_DEFINITIONS}")

SET (PMML_SWIG_DPYS_FILES
    PMMLsalome.i
    PMML.i)

INCLUDE_DIRECTORIES(
  ${PYTHON_INCLUDE_DIRS}
  ${NUMPY_INCLUDE_DIR}
  ${PTHREAD_INCLUDE_DIR} # pthread dependancy due to python2.7 library
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/..
  )

# _ABR_ Ensure dependency mechanism on all SWIG files and headers
SET (SWIG_MODULE_PMML_EXTRA_DEPS ${PMML_SWIG_DPYS_FILES} 
    ${pmml_HEADERS_HXX} ${pmml_HEADERS_TXX})

# Enusre proper dependencies mechanism for CMake since SWIG generates files 
# whose names are built from the SWIG *module* name, and not .i file name:
SET_SOURCE_FILES_PROPERTIES(PMMLsalome.i PROPERTIES SWIG_MODULE_NAME PMML)
SWIG_ADD_MODULE(PMML python PMMLsalome.i)
SWIG_LINK_LIBRARIES(PMML ${PYTHON_LIBRARIES} ${PLATFORM_LIBS} pmmlLib)

# _ABR_ Ensure dependency mechanism on all SWIG files and headers

IF(WIN32)
  SET_TARGET_PROPERTIES(_PMML PROPERTIES DEBUG_OUTPUT_NAME _PMML_d)
ENDIF(WIN32)


INSTALL(TARGETS ${SWIG_MODULE_PMML_REAL_NAME} DESTINATION ${SALOME_INSTALL_PYTHON})

SET(PYFILES_TO_INSTALL ${CMAKE_CURRENT_BINARY_DIR}/PMML.py)

SALOME_INSTALL_SCRIPTS("${PYFILES_TO_INSTALL}" ${SALOME_INSTALL_SCRIPT_PYTHON})

INSTALL(FILES PMML.i PMMLsalome.i DESTINATION ${SALOME_INSTALL_HEADERS})
INSTALL(FILES PMMLBasicsTest.py DESTINATION ${SALOME_INSTALL_SCRIPT_PYTHON})

SALOME_GENERATE_TESTS_ENVIRONMENT(tests_env)
ADD_TEST(PMMLBasicsTest ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/PMMLBasicsTest.py)
SET_TESTS_PROPERTIES(PMMLBasicsTest PROPERTIES ENVIRONMENT "${tests_env}")

#

SET(LOCAL_TEST_DIR ${SALOME_YACS_INSTALL_TEST}/pmml/pmml_swig)
FILE(COPY ${CMAKE_CURRENT_SOURCE_DIR}/PMMLBasicsTest.py DESTINATION "${CMAKE_INSTALL_PREFIX}/${LOCAL_TEST_DIR}")
FILE(COPY ${CMAKE_CURRENT_SOURCE_DIR}/PMMLBasicsTestLauncher.py DESTINATION "${CMAKE_INSTALL_PREFIX}/${LOCAL_TEST_DIR}")
