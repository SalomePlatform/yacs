
include $(top_srcdir)/adm/unix/make_begin.am

SUBDIRS = Test

lib_LTLIBRARIES = libYACSBasicData.la

libYACSBasicData_la_SOURCES =      \
	Data.cxx                   \
	ContentOfDataFlow.cxx      \
	ConversionException.cxx    \
	TypeCheckerDataFlow.cxx    \
	TypeCheckerDataStream.cxx  \
	$(__dummy__)

EXTRA_libYACSBasicData_la_SOURCES =  \
	Data.hxx                     \
	$(__dummy__)

libYACSBasicData_la_LIBADD = ../bases/libYACSBases.la

AM_CXXFLAGS = $(THREAD_DEF) \
              -I$(srcdir)/../bases \
              -D__OLD_GCC__

include $(top_srcdir)/adm/unix/make_end.am
