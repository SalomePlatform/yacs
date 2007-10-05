include $(top_srcdir)/adm/unix/make_begin.am

IDL_FILES = yacs.idl
IDL_SOURCES = yacsSK.cc
BUILT_SOURCES = $(IDL_SOURCES) yacs_idl.py 

OMNIORB_IDL+= -I$(KERNEL_ROOT_DIR)/idl/salome -I$(GUI_ROOT_DIR)/idl/salome

lib_LTLIBRARIES = libYACSorb.la

nodist_libYACSorb_la_SOURCES = $(IDL_SOURCES)

bin_PROGRAMS=yacsSrv yacs_clt

yacsSrv_SOURCES = yacsSrv.cxx $(IDL_SOURCES)

if SALOME_KERNEL
SALOME_LIBS=-L$(KERNEL_ROOT_DIR)/lib/salome -lSalomeLifeCycleCORBA -lSalomeDSCContainer
endif

AM_CXXFLAGS= \
  -I$(KERNEL_ROOT_DIR)/include/salome \
  -I$(GUI_ROOT_DIR)/include/salome 

IDLPYFLAGS  = \
  -I$(KERNEL_ROOT_DIR)/idl/salome

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

yacsSrv_LDFLAGS = $(PYTHON_EXTRA_LDFLAGS) -lexpat -pthread -lxml2

yacsSrv_LDADD =  ../yacsloader/libYACSloader.la \
               ../runtime/libYACSRuntimeSALOME.la \
               ../engine/libYACSEngine.la \
               $(SALOME_LIBS) \
               $(OMNIORB_LIBS) \
               $(PYTHON_LDFLAGS) \
               $(PYTHON_EXTRA_LIBS)

yacs_clt_SOURCES = yacs_clt.cxx $(IDL_SOURCES)

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

yacs_clt_LDFLAGS = $(PYTHON_EXTRA_LDFLAGS) -lexpat -pthread -lxml2

yacs_clt_LDADD =  ../yacsloader/libYACSloader.la \
               ../runtime/libYACSRuntimeSALOME.la \
               ../engine/libYACSEngine.la \
               $(SALOME_LIBS) \
               $(OMNIORB_LIBS) \
               $(PYTHON_LDFLAGS) \
               $(PYTHON_EXTRA_LIBS)

pkgpython_PYTHON = YACSGui.py

install-exec-local: install-pyidl

install-pyidl: $(IDL_FILES)
	$(INSTALL) -d  $(pkgpythondir)
	@for file in $^ dummy; do \
          if [ $$file != "dummy" ]; then \
             $(OMNIORB_IDL) -bpython $(IDLPYFLAGS) -C$(pkgpythondir) $$file ; \
          fi ; \
        done ;


include $(top_srcdir)/adm/unix/make_end.am
