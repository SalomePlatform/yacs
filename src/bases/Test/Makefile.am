# Copyright (C) 2006-2013  CEA/DEN, EDF R&D
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

include $(top_srcdir)/adm/unix/make_begin.am

AM_CXXFLAGS = $(THREAD_DEF) -I$(srcdir)/..

check_PROGRAMS = InitTests
if CPPUNIT_IS_OK
check_PROGRAMS += TestBases
endif

lib_LTLIBRARIES = libYACSDLTest.la

libYACSDLTest_la_SOURCES = DLTest.cxx

libYACSDLTest_la_CXXFLAGS = $(THREAD_DEF)

InitTests_SOURCES = InitTests.cxx

InitTests__CXXFLAGS =  -I$(srcdir)/..

TestBases_SOURCES = \
	TestBases.cxx \
	basesTest.cxx

TestBases_LDADD = ../libYACSBases.la

TestBases_LDFLAGS = $(CPPUNIT_LIBS) -pthread -ldl

TestBases_CXXFLAGS = $(THREAD_DEF) $(CPPUNIT_INCLUDES) -I$(srcdir)/..

TESTS = InitTests
if CPPUNIT_IS_OK
TESTS += TestBases 
endif

EXTRA_DIST = BasicMainTest.hxx  basesTest.hxx  UnitTestsResult.hxx

include $(top_srcdir)/adm/unix/make_end.am
