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

#
# TODO  : URANIE AND WIN32 : to compile on Windows, user uranietm 
#         
#         To adapt when YACS will be available on Windows
# 
# cmake options are :
# 
# cmake  
# -DURANIE=ON
# -DSALOME_BUILD_TESTS=ON
# -DSALOME_YACS_USE_SWIG=OFF 
# -DCMAKE_VERBOSE_MAKEFILE=ON 
# -DSALOME_CMAKE_DEBUG=ON 
# -DSALOME_BUILD_DOC:BOOL=FALSE  
# -G"NMake Makefiles JOM" 
# -DCMAKE_INSTALL_PREFIX=<chemin install>
# <chemin sources>
#   

IF(URANIE AND WIN32) 
  CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8 FATAL_ERROR)	
  SET(SALOME_INSTALL_CMAKE_LOCAL adm/cmake CACHE PATH 
    "Install path: local SALOME CMake files") 
  ENABLE_TESTING()
ENDIF(URANIE AND WIN32) 

IF(SALOME_YACS_USE_SWIG)
	FIND_PACKAGE(SalomePythonInterp REQUIRED)
	FIND_PACKAGE(SalomePythonLibs REQUIRED)
	FIND_PACKAGE(SalomeSWIG)
	SALOME_LOG_OPTIONAL_PACKAGE(PythonInterp SALOME_YACS_USE_SWIG)
	SALOME_LOG_OPTIONAL_PACKAGE(PythonLibs SALOME_YACS_USE_SWIG)
	SALOME_LOG_OPTIONAL_PACKAGE(SWIG   SALOME_YACS_USE_SWIG)
ENDIF(SALOME_YACS_USE_SWIG)

# Directories
#
# Directories have to be given after prerequisites (to be able to use
# Python version string for example).adm_local
# ===========
SET(SALOME_INSTALL_BINS bin/salome CACHE PATH "Install path: SALOME binaries")
SET(SALOME_INSTALL_LIBS lib/salome CACHE PATH "Install path: SALOME libs")
SET(SALOME_INSTALL_HEADERS include/salome CACHE PATH "Install path: SALOME headers")

IF(SALOME_YACS_USE_SWIG)
  SET(_pydir lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages)
  SET(SALOME_INSTALL_PYTHON ${_pydir}/salome CACHE PATH "Install path: SALOME Python stuff")
  SET(SALOME_INSTALL_PYTHON_SHARED ${SALOME_INSTALL_PYTHON}/shared_modules CACHE PATH 
    "Install path: SALOME Python shared modules")
ENDIF(SALOME_YACS_USE_SWIG)

SET(SALOME_PMML_INSTALL_RES_DATA "${SALOME_INSTALL_RES}/pmml" CACHE PATH "Install path: SALOME PMML specific data")

# Sources 
# ========
IF(WIN32)
  ADD_DEFINITIONS("-D_USE_MATH_DEFINES")
ENDIF(WIN32)

IF(URANIE AND WIN32) 
    FIND_PACKAGE(LibXml2 REQUIRED) 
    LINK_DIRECTORIES( ${LIBXML2_LIBRARIES} )
    INCLUDE_DIRECTORIES( ${LIBXML2_INCLUDE_DIR} )
ELSE(URANIE AND WIN32) 
    FIND_PACKAGE(SalomeLibXml2 REQUIRED) 
ENDIF(URANIE AND WIN32)    

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_BINARY_DIR}/..
  ${LIBXML2_INCLUDE_DIR}
  )

SET(pmml_SOURCES
  PMMLlib.cxx
  )

ADD_LIBRARY(pmmlLib SHARED ${pmml_SOURCES})
TARGET_LINK_LIBRARIES(pmmlLib  ${LIBXML2_LIBRARIES} ) 
INSTALL(TARGETS pmmlLib EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

FILE(GLOB pmml_HEADERS_HXX "${CMAKE_CURRENT_SOURCE_DIR}/*.hxx")
FILE(GLOB pmml_HEADERS_TXX "${CMAKE_CURRENT_SOURCE_DIR}/*.txx")
INSTALL(FILES ${pmml_HEADERS_HXX} ${pmml_HEADERS_TXX} DESTINATION ${SALOME_INSTALL_HEADERS})

# To allow usage as SWIG dependencies:
IF (NOT URANIE)
    SET(pmml_HEADERS_HXX PARENT_SCOPE)
    SET(pmml_HEADERS_TXX PARENT_SCOPE)
ENDIF (NOT URANIE)



IF(SALOME_BUILD_TESTS)
    ADD_SUBDIRECTORY(Test)
ENDIF(SALOME_BUILD_TESTS)


IF(SALOME_YACS_USE_SWIG)
    ADD_SUBDIRECTORY(pmml_swig)
ENDIF(SALOME_YACS_USE_SWIG)


IF(SALOME_BUILD_DOC)
    ADD_SUBDIRECTORY(doc)
ENDIF(SALOME_BUILD_DOC)

# Configuration export
# ====================

#   - in the install tree:
#       Get the relative path of the include directory so 
#       we can register it in the generated configuration files:
SET(CONF_INCLUDE_DIRS "${CMAKE_INSTALL_PREFIX}/${INSTALL_INCLUDE_DIR}")

# Install the export set for use with the install-tree
INSTALL(EXPORT ${PROJECT_NAME}TargetGroup DESTINATION "${SALOME_INSTALL_CMAKE_LOCAL}" 
        FILE ${PROJECT_NAME}Targets.cmake)
