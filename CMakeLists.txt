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
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8 FATAL_ERROR)
INCLUDE(CMakeDependentOption)

PROJECT(SalomeYACS C CXX)

# Ensure a proper linker behavior:
CMAKE_POLICY(SET CMP0003 NEW)
IF(WIN32)
  CMAKE_POLICY(SET CMP0020 OLD) # disable automatic linking to qtmain.lib 
ENDIF(WIN32)

# Versioning
# ===========
# Project name, upper case
STRING(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UC)

SET(${PROJECT_NAME_UC}_MAJOR_VERSION 8)
SET(${PROJECT_NAME_UC}_MINOR_VERSION 2)
SET(${PROJECT_NAME_UC}_PATCH_VERSION 0)
SET(${PROJECT_NAME_UC}_VERSION
  ${${PROJECT_NAME_UC}_MAJOR_VERSION}.${${PROJECT_NAME_UC}_MINOR_VERSION}.${${PROJECT_NAME_UC}_PATCH_VERSION})
SET(${PROJECT_NAME_UC}_VERSION_DEV 0)

# Common CMake macros
# ===================
SET(CONFIGURATION_ROOT_DIR $ENV{CONFIGURATION_ROOT_DIR} CACHE PATH "Path to the Salome CMake configuration files")
IF(EXISTS ${CONFIGURATION_ROOT_DIR})
  LIST(APPEND CMAKE_MODULE_PATH "${CONFIGURATION_ROOT_DIR}/cmake")
  INCLUDE(SalomeMacros)
ELSE()
  MESSAGE(FATAL_ERROR "We absolutely need the Salome CMake configuration files, please define CONFIGURATION_ROOT_DIR !")
ENDIF()

# Find KERNEL (optional)
# ==============
OPTION(SALOME_YACS_USE_KERNEL "Use SALOME KERNEL" ON)
IF( SALOME_YACS_USE_KERNEL )
  SET(KERNEL_ROOT_DIR $ENV{KERNEL_ROOT_DIR} CACHE PATH "Path to the Salome KERNEL")
  IF( EXISTS ${KERNEL_ROOT_DIR} )
    LIST(APPEND CMAKE_MODULE_PATH "${KERNEL_ROOT_DIR}/salome_adm/cmake_files")
    FIND_PACKAGE(SalomeKERNEL REQUIRED)
    KERNEL_WITH_CORBA() # check whether KERNEL builded with CORBA
    ADD_DEFINITIONS(${KERNEL_DEFINITIONS} -DSALOME_KERNEL)
    INCLUDE_DIRECTORIES(${KERNEL_INCLUDE_DIRS})
  ELSE( EXISTS ${KERNEL_ROOT_DIR} )
    MESSAGE(FATAL_ERROR "We absolutely need a Salome KERNEL, please define KERNEL_ROOT_DIR or turn option SALOME_YACS_USE_KERNEL to OFF !")
  ENDIF( EXISTS ${KERNEL_ROOT_DIR} )
  SALOME_LOG_OPTIONAL_PACKAGE(SalomeKERNEL SALOME_YACS_USE_KERNEL)
ELSE( SALOME_YACS_USE_KERNEL )
  MESSAGE(STATUS "Option SALOME_YACS_USE_KERNEL is set to OFF, so YACS will be built without CORBA!!!")
  LIST(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/adm/cmake")
  INCLUDE(SalomeMacros)
ENDIF( SALOME_YACS_USE_KERNEL )

# Find SALOME GUI (optional)
# ==============
CMAKE_DEPENDENT_OPTION(SALOME_BUILD_GUI "Build GUI of YACS" ON
                       "SALOME_YACS_USE_KERNEL" OFF)
IF(SALOME_BUILD_GUI)
  SET(GUI_ROOT_DIR $ENV{GUI_ROOT_DIR} CACHE PATH "Path to the Salome GUI")
  IF(EXISTS ${GUI_ROOT_DIR})
    LIST(APPEND CMAKE_MODULE_PATH "${GUI_ROOT_DIR}/adm_local/cmake_files")
    FIND_PACKAGE(SalomeGUI)
    SALOME_GUI_WITH_CORBA() # check whether GUI builded with CORBA
    SALOME_GUI_MODE(SALOME_USE_QXGRAPHVIEWER) # check whether GUI is built with the QxGraphViewer
  ELSE(EXISTS ${GUI_ROOT_DIR})
    MESSAGE(FATAL_ERROR "We absolutely need a Salome GUI, please define GUI_ROOT_DIR or turn option SALOME_BUILD_GUI to OFF !")
  ENDIF(EXISTS ${GUI_ROOT_DIR})
  SALOME_LOG_OPTIONAL_PACKAGE(SalomeGUI SALOME_BUILD_GUI)
ENDIF(SALOME_BUILD_GUI)

# Platform setup
# ==============
INCLUDE(SalomeSetupPlatform)   # From SalomeMacros
# Always build libraries as shared objects:
SET(BUILD_SHARED_LIBS TRUE)
# Local macros:
LIST(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/adm/cmake")

# additional preprocessor / compiler flags
ADD_DEFINITIONS(-DCMAKE_BUILD)
IF(WIN32)
  ADD_DEFINITIONS(-DNOGDI)
ENDIF(WIN32)

# User options
# ============ 
OPTION(SALOME_BUILD_DOC "Generate SALOME YACS documentation" ON)
OPTION(SALOME_BUILD_TESTS "Build SALOME YACS tests" ON)

OPTION(SALOME_YACS_USE_SWIG       "Enable Python wrapper"    ON)
OPTION(SALOME_YACS_USE_DSC        "Use DSC KERNEL extension" ON)

CMAKE_DEPENDENT_OPTION(SALOME_YACS_USE_PYEDITOR "Use built-in py editor" ON
                       "SALOME_USE_PYVIEWER" OFF)

IF(SALOME_YACS_USE_PYEDITOR)
    ADD_DEFINITIONS(-DHAS_PYEDITOR)
ENDIF(SALOME_YACS_USE_PYEDITOR)

# Advanced options:
IF(SALOME_YACS_USE_KERNEL AND SALOME_YACS_USE_DSC)
  IF(EXISTS ${KERNEL_ROOT_DIR}/idl/salome/DSC_Engines.idl)
    ADD_DEFINITIONS(-DDSC_PORTS)
  ELSE()
    MESSAGE(FATAL_ERROR "We absolutely need a KERNEL DSC extension. Please rebuild KERNEL with DSC support or turn option SALOME_YACS_USE_DSC to OFF !")
  ENDIF()
ENDIF()

# Prerequisites
# =============
# Find "big" prerequisites first - they reference themselves many others
#   -> this can helps to find the smaller prerequisites and detect conflicts.
# In our case KERNEL may have already loaded many prerequisites which are also used in YACS:

##
## From KERNEL:
##

# Mandatory products
FIND_PACKAGE(SalomePythonInterp REQUIRED)
FIND_PACKAGE(SalomePythonLibs REQUIRED)
FIND_PACKAGE(SalomeNumPySciPy)
FIND_PACKAGE(SalomePThread   REQUIRED)
ADD_DEFINITIONS(-DYACS_PTHREAD)
FIND_PACKAGE(SalomeBoost     REQUIRED)
FIND_PACKAGE(SalomeOmniORB   REQUIRED)
FIND_PACKAGE(SalomeOmniORBPy REQUIRED)
ADD_DEFINITIONS(-DOMNIORB)
FIND_PACKAGE(SalomeLibXml2   REQUIRED)

# Optional products:
IF(SALOME_YACS_USE_SWIG)
  FIND_PACKAGE(SalomeSWIG)
  SALOME_LOG_OPTIONAL_PACKAGE(SWIG SALOME_YACS_USE_SWIG)
#  ADD_DEFINITIONS(-DSWIG)
ENDIF()

IF(SALOME_BUILD_TESTS)
  ENABLE_TESTING()
  FIND_PACKAGE(SalomeCppUnit)
  SALOME_LOG_OPTIONAL_PACKAGE(CppUnit SALOME_BUILD_TESTS)  
ENDIF()

IF(SALOME_BUILD_DOC)
  FIND_PACKAGE(SalomeDoxygen)
  FIND_PACKAGE(SalomeSphinx)
  SALOME_LOG_OPTIONAL_PACKAGE(Doxygen SALOME_BUILD_DOC)
  SALOME_LOG_OPTIONAL_PACKAGE(Sphinx SALOME_BUILD_DOC)
  ADD_DEFINITIONS(-DDOXYGEN_IS_OK)
ENDIF()

# Find optional GUI libraries
IF(SALOME_BUILD_GUI)
  # Graphviz
  FIND_PACKAGE(SalomeGraphviz  REQUIRED)
  # Qt
  IF(NOT SALOME_GUI_BUILD_WITH_QT5)
    FIND_PACKAGE(SalomeQt4 REQUIRED COMPONENTS QtCore QtGui)
  ELSE()
    FIND_PACKAGE(SalomeQt5 REQUIRED)
  ENDIF()
ENDIF(SALOME_BUILD_GUI)

##
## YACS specifics
##

# Detection summary:
SALOME_PACKAGE_REPORT_AND_CHECK()

# Directories
# ===========
IF( SALOME_YACS_USE_KERNEL )
  SET(SALOME_INSTALL_BINS "${SALOME_INSTALL_BINS}" CACHE PATH "Install path: SALOME binaries")
  SET(SALOME_INSTALL_LIBS "${SALOME_INSTALL_LIBS}" CACHE PATH "Install path: SALOME libs")
  SET(SALOME_INSTALL_IDLS "${SALOME_INSTALL_IDLS}" CACHE PATH "Install path: SALOME IDL files")
  SET(SALOME_INSTALL_HEADERS "${SALOME_INSTALL_HEADERS}" CACHE PATH "Install path: SALOME headers")
  SET(SALOME_INSTALL_SCRIPT_SCRIPTS "${SALOME_INSTALL_SCRIPT_SCRIPTS}" CACHE PATH 
    "Install path: SALOME scripts")
  SET(SALOME_INSTALL_SCRIPT_DATA "${SALOME_INSTALL_SCRIPT_DATA}" CACHE PATH 
    "Install path: SALOME script data")
  SET(SALOME_INSTALL_SCRIPT_PYTHON "${SALOME_INSTALL_SCRIPT_PYTHON}" CACHE PATH 
    "Install path: SALOME Python scripts")
  SET(SALOME_INSTALL_PYTHON "${SALOME_INSTALL_PYTHON}" CACHE PATH "Install path: SALOME Python stuff")
  SET(SALOME_INSTALL_PYTHON_SHARED "${SALOME_INSTALL_PYTHON_SHARED}" CACHE PATH 
    "Install path: SALOME Python shared modules")
  SET(SALOME_INSTALL_CMAKE_LOCAL adm/cmake CACHE PATH 
    "Install path: local SALOME CMake files") 
  SET(SALOME_INSTALL_AMCONFIG_LOCAL adm/unix CACHE PATH
    "Install path: local SALOME config files (obsolete, to be removed)")

  SET(SALOME_INSTALL_RES "${SALOME_INSTALL_RES}" CACHE PATH "Install path: SALOME resources")
  SET(SALOME_INSTALL_DOC "${SALOME_INSTALL_DOC}" CACHE PATH "Install path: SALOME documentation")

ELSE(SALOME_YACS_USE_KERNEL)
  SET(SALOME_INSTALL_BINS bin/salome CACHE PATH "Install path: SALOME binaries")
  SET(SALOME_INSTALL_LIBS lib/salome CACHE PATH "Install path: SALOME libs")
  SET(SALOME_INSTALL_IDLS idl/salome CACHE PATH "Install path: SALOME IDL files")
  SET(SALOME_INSTALL_HEADERS include/salome CACHE PATH "Install path: SALOME headers")
  SET(SALOME_INSTALL_SCRIPT_SCRIPTS ${SALOME_INSTALL_BINS} CACHE PATH 
    "Install path: SALOME scripts")
  SET(SALOME_INSTALL_SCRIPT_DATA ${SALOME_INSTALL_BINS} CACHE PATH 
    "Install path: SALOME script data")
  SET(SALOME_INSTALL_SCRIPT_PYTHON ${SALOME_INSTALL_BINS} CACHE PATH 
    "Install path: SALOME Python scripts")
  SET(SALOME_INSTALL_CMAKE_LOCAL adm/cmake CACHE PATH 
    "Install path: local SALOME CMake files")
  SET(SALOME_INSTALL_AMCONFIG_LOCAL adm/unix CACHE PATH 
    "Install path: local SALOME config files (obsolete, to be removed)")

  SET(_pydir lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages)
  SET(SALOME_INSTALL_PYTHON ${_pydir}/salome CACHE PATH "Install path: SALOME Python stuff")
  SET(SALOME_INSTALL_PYTHON_SHARED ${SALOME_INSTALL_PYTHON}/shared_modules CACHE PATH 
    "Install path: SALOME Python shared modules")

  SET(SALOME_INSTALL_RES share/salome/resources CACHE PATH "Install path: SALOME resources")

  SET(SALOME_INSTALL_DOC share/doc/salome CACHE PATH "Install path: SALOME documentation")
ENDIF(SALOME_YACS_USE_KERNEL)

# Specific to YACS:
SET(SALOME_YACS_INSTALL_RES_DATA "${SALOME_INSTALL_RES}/yacs" CACHE PATH 
  "Install path: SALOME YACS specific data")
SET(SALOME_YACS_INSTALL_RES_SCRIPTS "${SALOME_INSTALL_RES}/yacs" CACHE PATH 
  "Install path: SALOME YACS scripts")
SET(SALOME_YACS_INSTALL_SAMPLES share/salome/yacssamples CACHE PATH
  "Install path: SALOME YACS samples")
SET(SALOME_YACS_INSTALL_SUPERV_SAMPLES share/salome/yacssupervsamples CACHE PATH
  "Install path: SALOME YACS SUPERV samples")
SET(SALOME_INSTALL_HXX2SALOME bin/HXX2SALOME_Test CACHE PATH 
  "Install path: SALOME HXX2SALOME Test files")
SET(SALOME_YACS_INSTALL_TEST ${SALOME_INSTALL_SCRIPT_SCRIPTS}/test CACHE PATH 
  "Install path: SALOME YACS Test files")
MARK_AS_ADVANCED(
  SALOME_YACS_INSTALL_RES_DATA
  SALOME_YACS_INSTALL_RES_SCRIPTS
  SALOME_YACS_INSTALL_SAMPLES
  SALOME_YACS_INSTALL_SUPERV_SAMPLES
  SALOME_INSTALL_HXX2SALOME
  SALOME_YACS_INSTALL_TEST
  )

# Accumulate environment variables for YACS module
SALOME_ACCUMULATE_ENVIRONMENT(PYTHONPATH NOCHECK ${CMAKE_INSTALL_PREFIX}/${SALOME_INSTALL_BINS}
                                                 ${CMAKE_INSTALL_PREFIX}/${SALOME_INSTALL_PYTHON})
SALOME_ACCUMULATE_ENVIRONMENT(LD_LIBRARY_PATH NOCHECK ${CMAKE_INSTALL_PREFIX}/${SALOME_INSTALL_LIBS}) 

# Sources 
# ========
IF(SALOME_YACS_USE_KERNEL)
  ADD_SUBDIRECTORY(idl)
ENDIF()

ADD_SUBDIRECTORY(adm)
ADD_SUBDIRECTORY(src)
IF(SALOME_BUILD_DOC)
  ADD_SUBDIRECTORY(doc)
ENDIF()
ADD_SUBDIRECTORY(Demo)

# Header configuration
# ====================
SALOME_XVERSION(${PROJECT_NAME})
SALOME_CONFIGURE_FILE(YACS_version.h.in YACS_version.h INSTALL ${SALOME_INSTALL_HEADERS})

# Configuration export
# (here only the level 1 prerequisites are exposed)
# ====================
INCLUDE(CMakePackageConfigHelpers)

# List of targets in this project we want to make visible to the rest of the world.
# They all have to be INSTALL'd with the option "EXPORT ${PROJECT_NAME}TargetGroup"
SET(_${PROJECT_NAME}_exposed_targets 
  YACSBases YACSlibEngine 
)

IF(SALOME_YACS_USE_KERNEL)
  LIST(APPEND _${PROJECT_NAME}_exposed_targets 
    SalomeIDLYACS YACSRuntimeSALOME YACSloader YACSevalYFX
  )
ENDIF(SALOME_YACS_USE_KERNEL)

IF(SALOME_BUILD_TESTS)
  LIST(APPEND _${PROJECT_NAME}_exposed_targets 
    YACSDLTest PluginSimplex PluginOptEvTest1
  )
  IF(SALOME_YACS_USE_KERNEL)
    LIST(APPEND _${PROJECT_NAME}_exposed_targets 
      TestComponentLocal yacsloader_echo
    )
  ENDIF(SALOME_YACS_USE_KERNEL)
ENDIF(SALOME_BUILD_TESTS)

IF(SALOME_BUILD_GUI)
  LIST(APPEND _${PROJECT_NAME}_exposed_targets 
    HMI ydfxwidgets GenericGui YACS SalomeWrap
  )
ENDIF(SALOME_BUILD_GUI)

# Add all targets to the build-tree export set
EXPORT(TARGETS ${_${PROJECT_NAME}_exposed_targets}
  FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake)

# Create the configuration files:
#   - in the build tree:

#      Ensure the variables are always defined for the configure:
SET(KERNEL_ROOT_DIR "${KERNEL_ROOT_DIR}")
SET(GUI_ROOT_DIR "${GUI_ROOT_DIR}")
IF(NOT SALOME_GUI_BUILD_WITH_QT5)
  SET(QT_ROOT_DIR "${QT4_ROOT_DIR}")
ELSE()
  SET(QT_ROOT_DIR "${QT5_ROOT_DIR}")
ENDIF()
 
SET(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include" "${PROJECT_BINARY_DIR}/include")

# Build variables that will be expanded when configuring Salome<MODULE>Config.cmake:
IF(NOT SALOME_GUI_BUILD_WITH_QT5)
  SALOME_CONFIGURE_PREPARE(Qt4)
ELSE()
  SALOME_CONFIGURE_PREPARE(Qt5)
ENDIF()

CONFIGURE_PACKAGE_CONFIG_FILE(${PROJECT_NAME}Config.cmake.in 
    ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION "${SALOME_INSTALL_CMAKE_LOCAL}"
    PATH_VARS CONF_INCLUDE_DIRS SALOME_INSTALL_CMAKE_LOCAL CMAKE_INSTALL_PREFIX
    KERNEL_ROOT_DIR GUI_ROOT_DIR QT_ROOT_DIR)

#   - in the install tree (VSR 16/08/2013: TEMPORARILY COMMENT THIS - TO REMOVE?):
#       Get the relative path of the include directory so 
#       we can register it in the generated configuration files:
#SET(CONF_INCLUDE_DIRS "${CMAKE_INSTALL_PREFIX}/${INSTALL_INCLUDE_DIR}")
#CONFIGURE_PACKAGE_CONFIG_FILE(adm_local/cmake_files/${PROJECT_NAME}Config.cmake.in 
#    ${PROJECT_BINARY_DIR}/to_install/${PROJECT_NAME}Config.cmake
#    INSTALL_DESTINATION "${SALOME_INSTALL_CMAKE_LOCAL}"
#    PATH_VARS CONF_INCLUDE_DIRS SALOME_INSTALL_CMAKE_LOCAL CMAKE_INSTALL_PREFIX 
#    KERNEL_ROOT_DIR GUI_ROOT_DIR QT_ROOT_DIR)

WRITE_BASIC_PACKAGE_VERSION_FILE(${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${${PROJECT_NAME_UC}_VERSION}
    COMPATIBILITY AnyNewerVersion)
  
# Install the CMake configuration files:
INSTALL(FILES
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  DESTINATION "${SALOME_INSTALL_CMAKE_LOCAL}")

# Install the export set for use with the install-tree
INSTALL(EXPORT ${PROJECT_NAME}TargetGroup DESTINATION "${SALOME_INSTALL_CMAKE_LOCAL}" 
        FILE ${PROJECT_NAME}Targets.cmake)
