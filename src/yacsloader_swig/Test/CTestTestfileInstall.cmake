# Copyright (C) 2015-2024  CEA, EDF
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

IF(NOT WIN32)
  SET(TEST_NAME ${COMPONENT_NAME}_YacsLoaderTest_swig)
  ADD_TEST(${TEST_NAME} testYacsLoaderSwig.py)
  SET_TESTS_PROPERTIES(${TEST_NAME} PROPERTIES
                                    LABELS "${COMPONENT_NAME}"
                                    ENVIRONMENT "SALOME_EMB_SERVANT=1"
                      )

  SET(TEST_NAME ${COMPONENT_NAME}_StdAloneYacsLoaderTest1)
  ADD_TEST(${TEST_NAME} StdAloneYacsLoaderTest1.py)
  SET_TESTS_PROPERTIES(${TEST_NAME} PROPERTIES
                                    LABELS "${COMPONENT_NAME}"
                                    ENVIRONMENT "SALOME_EMB_SERVANT=1"
                      )

  SET(TEST_NAME ${COMPONENT_NAME}_PyNodeWithCache_swig)
  ADD_TEST(${TEST_NAME} testPynodeWithCache.py)
  SET_TESTS_PROPERTIES(${TEST_NAME} PROPERTIES
                                    LABELS "${COMPONENT_NAME}"
                                    ENVIRONMENT "SALOME_EMB_SERVANT=1"
                      )

  SET(TEST_NAME ${COMPONENT_NAME}_WorkloadManager_swig)
  ADD_TEST(${TEST_NAME} testWorkloadManager.py)
  SET_TESTS_PROPERTIES(${TEST_NAME} PROPERTIES
                                    LABELS "${COMPONENT_NAME}"
                                    ENVIRONMENT "SALOME_EMB_SERVANT=1"
                      )

  SET(TEST_NAME ${COMPONENT_NAME}_Progress_swig)
  ADD_TEST(${TEST_NAME} testProgress.py)
  SET_TESTS_PROPERTIES(${TEST_NAME} PROPERTIES
                                    LABELS "${COMPONENT_NAME}"
                                    ENVIRONMENT "SALOME_EMB_SERVANT=1"
                      )

  SET(TEST_NAME ${COMPONENT_NAME}_Refcount_swig)
  ADD_TEST(${TEST_NAME} testRefcount.py)
  SET_TESTS_PROPERTIES(${TEST_NAME} PROPERTIES
                                    LABELS "${COMPONENT_NAME}"
                                    ENVIRONMENT "SALOME_EMB_SERVANT=1"
                      )

  SET(TEST_NAME ${COMPONENT_NAME}_Resume_swig)
  ADD_TEST(${TEST_NAME} testResume.py)
  SET_TESTS_PROPERTIES(${TEST_NAME} PROPERTIES
                                    LABELS "${COMPONENT_NAME}"
                                    ENVIRONMENT "SALOME_EMB_SERVANT=1"
                      )

  SET(TEST_NAME ${COMPONENT_NAME}_SaveLoadRun_swig)
  ADD_TEST(${TEST_NAME} testSaveLoadRun.py)
  SET_TESTS_PROPERTIES(${TEST_NAME} PROPERTIES
                                    LABELS "${COMPONENT_NAME}"
                                    ENVIRONMENT "SALOME_EMB_SERVANT=1"
                      )

  SET(TEST_NAME ${COMPONENT_NAME}_ProxyTest_swig)
  ADD_TEST(${TEST_NAME} testYacsProxy.py)
  SET_TESTS_PROPERTIES(${TEST_NAME} PROPERTIES
                                    LABELS "${COMPONENT_NAME}"
                      )
                      
  SET(TEST_NAME ${COMPONENT_NAME}_PerfTest0_swig)
  ADD_TEST(${TEST_NAME} testYacsPerfTest0.py)
  SET_TESTS_PROPERTIES(${TEST_NAME} PROPERTIES
                                    LABELS "${COMPONENT_NAME}"
                      )

  SET(TEST_NAME ${COMPONENT_NAME}_Driver_Overrides)
  ADD_TEST(${TEST_NAME} testYacsDriverOverrides.py)
  SET_TESTS_PROPERTIES(${TEST_NAME} PROPERTIES
                                    LABELS "${COMPONENT_NAME}"
                      )

  SET(TEST_NAME ${COMPONENT_NAME}_ValidationChecks_swig)
  ADD_TEST(${TEST_NAME} testValidationChecks.py)
  SET_TESTS_PROPERTIES(${TEST_NAME} PROPERTIES
                                    LABELS "${COMPONENT_NAME}"
                                    ENVIRONMENT "SALOME_EMB_SERVANT=1"
                      )

  SET(TEST_NAME ${COMPONENT_NAME}_Fixes_swig)
  ADD_TEST(${TEST_NAME} testFixes.py)
  SET_TESTS_PROPERTIES(${TEST_NAME} PROPERTIES
                                    LABELS "${COMPONENT_NAME}"
                                    ENVIRONMENT "SALOME_EMB_SERVANT=1"
                      )

ENDIF()
