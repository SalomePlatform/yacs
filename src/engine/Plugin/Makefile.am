
include $(top_srcdir)/adm/unix/make_begin.am

lib_LTLIBRARIES = libPluginSimplex.la

libPluginSimplex_la_SOURCES = PluginSimplex.cxx \
	maestro.cxx decode.cxx point.cxx aleas.cxx \
	salomevent.cxx simplex.cxx solution.cxx \
        local.cxx mt19937ar.cxx 

libPluginSimplex_la_CXXFLAGS = $(THREAD_DEF) \
	-I$(srcdir)/..                       \
	-I$(srcdir)/../../bases

EXTRA_DIST = \
aleas.hxx     critere.hxx    decode.hxx         distrib.hxx  fonction.hxx \
local.hxx     maestro.hxx    PluginSimplex.hxx  point.hxx    saclass.hxx \
saemul.hxx    salomesup.hxx  salomevent.hxx     sasimpl.hxx  simplex.hxx \
solution.hxx  saconst.h

include $(top_srcdir)/adm/unix/make_end.am
