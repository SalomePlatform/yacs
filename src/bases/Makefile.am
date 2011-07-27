#  Copyright (C) 2006-2011  CEA/DEN, EDF R&D
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

libYACSBases_la_SOURCES =                         \
    Exception.cxx Thread.cxx AlternateThread.cxx  \
    Mutex.cxx Semaphore.cxx DynLibLoader.cxx      \
    Cstr2d.cxx YacsTrace.cxx chrono.cxx           \
	$(__dummy__)

EXTRA_libYACSBases_la_SOURCES =                   \
	Thread.hxx ThreadPT.hxx ThreadPT.cxx          \
	AlternateThread.hxx AlternateThreadPT.hxx     \
	AlternateThreadPT.cxx                         \
	Mutex.hxx MutexPT.cxx MutexPT.hxx             \
	Semaphore.hxx SemaphorePT.cxx SemaphorePT.hxx \
	DynLibLoader.hxx Exception.hxx chrono.hxx     \
	$(__dummy__)

if WINDOWS
EXTRA_libYACSBases_la_SOURCES += DynLibLoaderWin.hxx
else
EXTRA_libYACSBases_la_SOURCES += DynLibLoaderGNU.hxx
endif

libYACSBases_la_LIBADD = $(PTHREAD_LIBS)

salomeinclude_HEADERS =        \
           YACSBasesExport.hxx \
           Cstr2d.hxx \
           define.hxx \
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
           AlternateThread.hxx \
           AlternateThreadPT.hxx \
           YacsTrace.hxx \
           yacsconfig.h \
           chrono.hxx \
           $(__dummy__)

AM_CXXFLAGS = $(THREAD_DEF)

EXTRA_DIST=ThreadPT.cxx AlternateThreadPT.cxx MutexPT.cxx \
		   SemaphorePT.cxx  DynLibLoaderGNU.cxx

include $(top_srcdir)/adm/unix/make_end.am
