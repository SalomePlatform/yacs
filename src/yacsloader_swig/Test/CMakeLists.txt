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

# --- rules ---

IF(NOT WIN32)
  SET(SHELL /bin/sh)
  CONFIGURE_FILE(config_appli.xml.in config_appli.xml @ONLY)
  CONFIGURE_FILE(YacsLoaderTest.sh.in YacsLoaderTest.sh @ONLY)
  CONFIGURE_FILE(YacsLoaderInSessionTest.sh.in YacsLoaderInSessionTest.sh @ONLY)
  ADD_TEST(NAME YacsLoaderTest_swig COMMAND ${SHELL} ${CMAKE_CURRENT_BINARY_DIR}/YacsLoaderTest.sh)

  SALOME_GENERATE_TESTS_ENVIRONMENT(tests_env)
  ADD_TEST(NAME StdAloneYacsLoaderTest1 COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/StdAloneYacsLoaderTest1.py)
  SET_TESTS_PROPERTIES(StdAloneYacsLoaderTest1 PROPERTIES ENVIRONMENT "${tests_env}")

  # For salome test
  SET(LOCAL_TEST_DIR ${SALOME_YACS_INSTALL_TEST}/yacsloader_swig)
  SET(LOCAL_TEST_FILES
    StdAloneYacsLoaderTest1.py
    testEdit.py
    testExec.py
    testLoader.py
    testRefcount.py
    testResume.py
    testSave.py
    testSaveLoadRun.py
    optim_plugin.py
    testValidationChecks.py
    testProgress.py
   )
  INSTALL(FILES ${LOCAL_TEST_FILES}
        DESTINATION ${LOCAL_TEST_DIR})
  INSTALL(PROGRAMS runUnitTest.sh
        DESTINATION ${LOCAL_TEST_DIR})
  INSTALL(FILES CTestTestfileInstall.cmake
        DESTINATION ${LOCAL_TEST_DIR}
        RENAME CTestTestfile.cmake)
  # Use relative path for symbolic links in order to allow the copy of the directory.
  # REL_PATH_TO_INSTALL goes to CMAKE_INSTALL_PREFIX from ${CMAKE_INSTALL_PREFIX}/${LOCAL_TEST_DIR}
  IF(IS_ABSOLUTE ${SALOME_YACS_INSTALL_SAMPLES})
    SET(SAMPLES_LINK_TARGET ${SALOME_YACS_INSTALL_SAMPLES})
  ELSE()
    STRING(REGEX REPLACE [^/]+ ".." REL_PATH_TO_INSTALL ${LOCAL_TEST_DIR})
    SET(SAMPLES_LINK_TARGET ${REL_PATH_TO_INSTALL}/${SALOME_YACS_INSTALL_SAMPLES})
  ENDIF()
  INSTALL(CODE "EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E create_symlink
                ${SAMPLES_LINK_TARGET}
                ${CMAKE_INSTALL_PREFIX}/${LOCAL_TEST_DIR}/samples )" )
ENDIF()
