#  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
include $(top_srcdir)/adm/unix/make_begin.am

SUBDIRS = Test

noinst_LTLIBRARIES = libYACSBases.la

libYACSBases_la_SOURCES = Exception.cxx Thread.cxx Mutex.cxx Semaphore.cxx \
	DynLibLoader.cxx  DrivenCondition.cxx                              \
	Cstr2d.cxx   YacsTrace.cxx                    \
	$(__dummy__)

EXTRA_libYACSBases_la_SOURCES = 
	Thread.hxx ThreadPT.hxx ThreadPT.cxx          \
	Mutex.hxx MutexPT.cxx MutexPT.hxx             \
	Semaphore.hxx SemaphorePT.cxx SemaphorePT.hxx \
	DynLibLoader.hxx DynLibLoaderGNU.hxx          \
	DrivenCondition.hxx DrivenConditionPT.hxx     \
	Exception.hxx                                 \
	$(__dummy__)

salomeinclude_HEADERS =                               \
           Cstr2d.hxx \
           define.hxx \
           DrivenCondition.hxx \
           DrivenConditionPT.hxx \
           DynLibLoaderGNU.hxx \
           DynLibLoader.hxx \
           DynLibLoaderWin.hxx \
           Exception.hxx \
           Mutex.hxx \
           MutexPT.hxx \
           Semaphore.hxx \
           SemaphorePT.hxx \
           Thread.hxx \
           ThreadPT.hxx \
           YacsTrace.hxx \
					 yacsconfig.h \
           $(__dummy__)

AM_CXXFLAGS = $(THREAD_DEF)

EXTRA_DIST=ThreadPT.cxx DrivenConditionPT.cxx  MutexPT.cxx  SemaphorePT.cxx  DynLibLoaderGNU.cxx

include $(top_srcdir)/adm/unix/make_end.am
