include $(top_srcdir)/adm/unix/make_begin.am

BUILT_SOURCES = echoSK.cc schema.xml xmlrpcprog.py
bin_PROGRAMS=echoSrv 
echoSrv_SOURCES = echoSrv.cxx echoSK.cc 
echoSrv_CXXFLAGS = $(OMNIORB_INCLUDES) $(OMNIORB_CXXFLAGS) 
echoSrv_LDFLAGS = $(OMNIORB_LIBS)
AM_CXXFLAGS = $(THREAD_DEF)

schema.xml:schema_orig.xml
	cp $(srcdir)/schema_orig.xml schema.xml
xmlrpcprog.py:xmlrpcprog_orig.py
	cp $(srcdir)/xmlrpcprog_orig.py xmlrpcprog.py

CLEANFILES = *.hh *SK.cc schema.xml xmlrpcprog.py

include $(top_srcdir)/adm/unix/make_end.am
