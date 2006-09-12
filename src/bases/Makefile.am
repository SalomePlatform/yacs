
include $(top_srcdir)/adm/unix/make_begin.am

SUBDIRS = Test

noinst_LTLIBRARIES = libYACSBases.la

libYACSBases_la_SOURCES = Exception.cxx Thread.cxx Mutex.cxx Semaphore.cxx \
	$(__dummy__)

EXTRA_libYACSBases_la_SOURCES = 
	Thread.hxx ThreadPT.hxx ThreadPT.cxx          \
	Mutex.hxx MutexPT.cxx MutexPT.hxx             \
	Semaphore.hxx SemaphorePT.cxx SemaphorePT.hxx \
	Exception.hxx                                 \
	$(__dummy__)

AM_CXXFLAGS = $(THREAD_DEF)

include $(top_srcdir)/adm/unix/make_end.am
