
include $(top_srcdir)/adm/unix/make_begin.am

check_PROGRAMS = TestEngine IntegrationTestEngine

TestEngine_SOURCES =              \
	TestEngine.cxx            \
	RuntimeForEngineTest.cxx  \
	engineTest.cxx

TestEngine_LDADD = \
                 ../libYACSlibEngine.la         \
                 ../../bases/libYACSBases.la


TestEngine_LDFLAGS = $(CPPUNIT_LIBS) -pthread -ldl

TestEngine_CXXFLAGS = \
	$(CPPUNIT_INCLUDES)           \
	-I$(srcdir)/..                \
	-I$(srcdir)/../../bases       \
	-I$(srcdir)/../../bases/Test

lib_LTLIBRARIES = libPluginOptEvTest1.la

libPluginOptEvTest1_la_SOURCES = PluginOptEvTest1.cxx

libPluginOptEvTest1_la_CXXFLAGS = $(THREAD_DEF) \
	-I$(srcdir)/..                          \
	-I$(srcdir)/../../bases

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


TESTS =  TestEngine IntegrationTestEngine

EXTRA_DIST = \
ComponentInstanceTest.hxx  ContainerTest.hxx     engineIntegrationTest.hxx \
engineTest.hxx             PluginOptEvTest1.hxx  RuntimeForEngineIntegrationTest.hxx \
RuntimeForEngineTest.hxx   ToyNode.hxx

clean-local:
	rm -f traceExec_* dumpState.xml

include $(top_srcdir)/adm/unix/make_end.am
