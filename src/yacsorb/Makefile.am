include $(top_srcdir)/adm/unix/make_begin.am

IDL_FILES = yacs.idl
IDL_SOURCES = yacsSK.cc
BUILT_SOURCES = $(IDL_SOURCES) 

lib_LTLIBRARIES = libYACSorb.la

nodist_libYACSorb_la_SOURCES = $(IDL_SOURCES)

libYACSorb_la_CPPFLAGS =  $(OMNIORB_INCLUDES)

libYACSorb_la_CXXFLAGS =  $(OMNIORB_CXXFLAGS)

bin_PROGRAMS=yacsSrv yacs_clt

yacsSrv_SOURCES = yacsSrv.cxx 
nodist_yacsSrv_SOURCES = $(IDL_SOURCES)

if SALOME_KERNEL
SALOME_LIBS=-L$(KERNEL_ROOT_DIR)/lib/salome -lSalomeLifeCycleCORBA -lSalomeDSCContainer
endif

AM_CXXFLAGS= \
  -I$(KERNEL_ROOT_DIR)/include/salome \
  -I$(GUI_ROOT_DIR)/include/salome 

IDLCXXFLAGS = -I$(KERNEL_ROOT_DIR)/idl/salome -I$(GUI_ROOT_DIR)/idl/salome
IDLPYFLAGS  = -I$(KERNEL_ROOT_DIR)/idl/salome

yacsSrv_CXXFLAGS = -g -DYACS_PTHREAD \
  $(PYTHON_CPPFLAGS) \
  $(OMNIORB_INCLUDES) \
  $(OMNIORB_CXXFLAGS)  \
  -I$(KERNEL_ROOT_DIR)/include/salome \
  -I$(GUI_ROOT_DIR)/include/salome \
  -I$(srcdir)/../bases \
  -I$(srcdir)/../engine \
  -I$(srcdir)/../yacsloader \
  -I$(srcdir)/../runtime

yacsSrv_LDFLAGS = $(PYTHON_EXTRA_LDFLAGS) $(EXPAT_LIBS) $(LIBXML_LIBS) -pthread 

yacsSrv_LDADD =  ../yacsloader/libYACSloader.la \
               ../runtime/libYACSRuntimeSALOME.la \
               ../engine/libYACSEngine.la \
               $(SALOME_LIBS) \
               $(OMNIORB_LIBS) \
               $(PYTHON_LDFLAGS) \
               $(PYTHON_EXTRA_LIBS)

yacs_clt_SOURCES = yacs_clt.cxx 
nodist_yacs_clt_SOURCES = $(IDL_SOURCES)

yacs_clt_CXXFLAGS = \
  $(PYTHON_CPPFLAGS) \
  -I$(srcdir)/../bases \
  -I$(srcdir)/../engine \
  -I$(srcdir)/../yacsloader \
  -I$(srcdir)/../runtime \
  -I$(KERNEL_ROOT_DIR)/include/salome \
  -I$(GUI_ROOT_DIR)/include/salome \
  $(OMNIORB_INCLUDES) \
  $(OMNIORB_CXXFLAGS)

yacs_clt_LDFLAGS = $(PYTHON_EXTRA_LDFLAGS) $(EXPAT_LIBS) -pthread $(LIBXML_LIBS) 

yacs_clt_LDADD =  ../yacsloader/libYACSloader.la \
               ../runtime/libYACSRuntimeSALOME.la \
               ../engine/libYACSEngine.la \
               $(SALOME_LIBS) \
               $(OMNIORB_LIBS) \
               $(PYTHON_LDFLAGS) \
               $(PYTHON_EXTRA_LIBS)

salomepython_PYTHON = YACS.py

EXTRA_DIST = $(IDL_FILES)

install-exec-local: install-pyidl

uninstall-local:
	rm -rf $(DESTDIR)$(salomepythondir)/yacs_idl.py
	rm -rf $(DESTDIR)$(salomepythondir)/YACS_ORB
	rm -rf $(DESTDIR)$(salomepythondir)/YACS_ORB__POA

install-pyidl: $(IDL_FILES)
	$(INSTALL) -d  $(DESTDIR)$(salomepythondir)
	@for file in $^ dummy; do \
          if [ $$file != "dummy" ]; then \
             $(OMNIORB_IDL) -bpython $(IDLPYFLAGS) -C$(DESTDIR)$(salomepythondir) $$file ; \
          fi ; \
        done ;

clean-local:
	rm -rf yacs.hh  YACS_ORB  YACS_ORB__POA  yacsSK.cc

include $(top_srcdir)/adm/unix/make_end.am
