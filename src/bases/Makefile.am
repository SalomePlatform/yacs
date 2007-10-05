
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
           $(__dummy__)

AM_CXXFLAGS = $(THREAD_DEF)

include $(top_srcdir)/adm/unix/make_end.am
