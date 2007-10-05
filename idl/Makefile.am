
include $(top_srcdir)/adm/unix/make_begin.am

IDL_FILES = yacsgui.idl
IDL_SRC = yacsguiSK.cc
BUILT_SOURCES = $(IDL_SRC) yacsgui_idl.py 

OMNIORB_IDL+= -I$(KERNEL_ROOT_DIR)/idl/salome -I$(GUI_ROOT_DIR)/idl/salome

IDLPYFLAGS  = \
  -I$(KERNEL_ROOT_DIR)/idl/salome

install-exec-local: install-pyidl

install-pyidl: $(IDL_FILES)
	$(INSTALL) -d  $(pkgpythondir)
	@for file in $^ dummy; do \
          if [ $$file != "dummy" ]; then \
             $(OMNIORB_IDL) -bpython $(IDLPYFLAGS) -C$(pkgpythondir) $$file ; \
          fi ; \
        done ;

include $(top_srcdir)/adm/unix/make_end.am
