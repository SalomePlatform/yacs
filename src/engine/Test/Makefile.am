
include $(top_srcdir)/adm/unix/make_begin.am

check_PROGRAMS = testPorts testBloc

testPorts_SOURCES = testPorts.cxx ToyNode.cxx

testPorts_LDADD = ../libYACSEngine.la \
                  ../../bases/libYACSBases.la \
                  ../../basicData/libYACSBasicData.la 


testPorts_LDFLAGS = $(CPPUNIT_LIBS) -pthread -ldl

testPorts_CXXFLAGS = $(CPPUNIT_INCLUDES) \
                     -I$(srcdir)/.. \
                     -I$(srcdir)/../../bases \
                     -I$(srcdir)/../../basicData

testBloc_SOURCES = testBloc.cxx ToyNode.cxx

testBloc_LDADD =  ../libYACSEngine.la \
                  ../../bases/libYACSBases.la \
                  ../../basicData/libYACSBasicData.la 


testBloc_LDFLAGS = $(CPPUNIT_LIBS) -pthread -ldl

testBloc_CXXFLAGS = $(THREAD_DEF) $(CPPUNIT_INCLUDES) \
                     -I$(srcdir)/..               \
                     -I$(srcdir)/../../bases      \
                     -I$(srcdir)/../../basicData

TESTS = testPorts testBloc

include $(top_srcdir)/adm/unix/make_end.am
