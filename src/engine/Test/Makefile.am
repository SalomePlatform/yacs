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

if CPPUNIT_IS_OK
check_PROGRAMS = TestEngine IntegrationTestEngine
endif

TestEngine_SOURCES =              \
	TestEngine.cxx            \
	RuntimeForEngineTest.cxx  \
	engineTest.cxx

TestEngine_LDADD = \
                 ../libYACSlibEngine.la         \
                 ../../bases/libYACSBases.la


TestEngine_LDFLAGS = $(CPPUNIT_LIBS) -pthread -ldl

TestEngine_CXXFLAGS = $(THREAD_DEF) \
	$(CPPUNIT_INCLUDES)           \
	-I$(srcdir)/..                \
	-I$(srcdir)/../../bases       \
	-I$(srcdir)/../../bases/Test

lib_LTLIBRARIES = libPluginOptEvTest1.la

libPluginOptEvTest1_la_SOURCES = PluginOptEvTest1.cxx

libPluginOptEvTest1_la_CXXFLAGS = $(THREAD_DEF) \
	-I$(srcdir)/..                          \
	-I$(srcdir)/../../bases

libPluginOptEvTest1_la_LIBADD = ../libYACSlibEngine.la

IntegrationTestEngine_SOURCES =              \
	ToyNode.cxx                          \
	ContainerTest.cxx                    \
	ComponentInstanceTest.cxx            \
	IntegrationTestEngine.cxx            \
	engineIntegrationTest.cxx            \
	RuntimeForEngineIntegrationTest.cxx

IntegrationTestEngine_LDADD =                   \
                 ../libYACSlibEngine.la            \
                 ../../bases/libYACSBases.la

IntegrationTestEngine_LDFLAGS = $(CPPUNIT_LIBS) -pthread -ldl

IntegrationTestEngine_CXXFLAGS =      \
	$(CPPUNIT_INCLUDES)           \
	$(THREAD_DEF)                 \
	-I$(srcdir)/..                \
	-I$(srcdir)/../../bases       \
	-I$(srcdir)/../../bases/Test


if CPPUNIT_IS_OK
TESTS =  TestEngine IntegrationTestEngine
endif

EXTRA_DIST = \
ComponentInstanceTest.hxx  ContainerTest.hxx     engineIntegrationTest.hxx \
engineTest.hxx             PluginOptEvTest1.hxx  RuntimeForEngineIntegrationTest.hxx \
RuntimeForEngineTest.hxx   ToyNode.hxx

clean-local:
	rm -f traceExec_* dumpState.xml

include $(top_srcdir)/adm/unix/make_end.am
