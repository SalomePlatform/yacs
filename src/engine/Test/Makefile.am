
include $(top_srcdir)/adm/unix/make_begin.am

check_PROGRAMS = TestEngine

TestEngine_SOURCES = \
	TestEngine.cxx \
	engineTest.cxx

TestEngine_LDADD = \
                 ../libYACSEngine.la \
                 ../../bases/libYACSBases.la


TestEngine_LDFLAGS = $(CPPUNIT_LIBS) -pthread -ldl

TestEngine_CXXFLAGS = \
	$(CPPUNIT_INCLUDES) \
	-I$(srcdir)/.. \
	-I$(srcdir)/../../bases \
	-I$(srcdir)/../../bases/Test


TESTS =  TestEngine

include $(top_srcdir)/adm/unix/make_end.am
