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

IF(SALOME_BUILD_TESTS)
  ADD_SUBDIRECTORY(Test)
ENDIF(SALOME_BUILD_TESTS)

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${PTHREAD_INCLUDE_DIR}
  )

# libraries to link to
SET(_link_LIBRARIES
  ${PTHREAD_LIBRARIES}
  ${PLATFORM_LIBS}
  )

# --- headers ---

SET(YACSBases_HEADERS
  YACSBasesExport.hxx
  Cstr2d.hxx
  define.hxx
  DynLibLoaderGNU.hxx
  DynLibLoader.hxx
  DynLibLoaderWin.hxx
  Exception.hxx
  AutoRefCnt.hxx
  AutoLocker.hxx
  Mutex.hxx
  MutexPT.hxx
  Semaphore.hxx
  SemaphorePT.hxx
  Thread.hxx
  ThreadPT.hxx
  AlternateThread.hxx
  AlternateThreadPT.hxx
  YacsTrace.hxx
  yacsconfig.h
  )

# --- sources ---

SET(YACSBases_SOURCES
  Exception.cxx
  Thread.cxx
  AlternateThread.cxx
  Mutex.cxx
  Semaphore.cxx
  DynLibLoader.cxx
  Cstr2d.cxx
  YacsTrace.cxx
  chrono.cxx
  )

# --- type of library ---

SET(TYPE)
IF(WIN32)
  SET(TYPE STATIC)
ELSE(WIN32)
  SET(TYPE SHARED)
ENDIF(WIN32)

# --- rules ---

ADD_LIBRARY(YACSBases ${TYPE} ${YACSBases_SOURCES})
TARGET_LINK_LIBRARIES(YACSBases ${_link_LIBRARIES})
INSTALL(TARGETS YACSBases EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${YACSBases_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
