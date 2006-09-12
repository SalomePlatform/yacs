
include $(top_srcdir)/adm/unix/make_begin.am

	$(__dummy__)

check_PROGRAMS = testData

testData_SOURCES = testData.cxx

testData_LDADD = ../libYACSBasicData.la \
                 ../../bases/libYACSBases.la

testData_LDFLAGS = $(CPPUNIT_LIBS) -pthread -ldl

testData_CXXFLAGS = $(CPPUNIT_INCLUDES)       \
                    -I$(srcdir)/..            \
                    -I$(srcdir)/../../bases

TESTS = testData

include $(top_srcdir)/adm/unix/make_end.am
