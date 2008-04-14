
include $(top_srcdir)/adm/unix/make_begin.am

IDL_FILES = yacsgui.idl
IDL_SRC = yacsguiSK.cc
BUILT_SOURCES = $(IDL_SRC) yacsgui.hh

IDLCXXFLAGS = -I$(KERNEL_ROOT_DIR)/idl/salome -I$(GUI_ROOT_DIR)/idl/salome

IDLPYFLAGS  = -I$(KERNEL_ROOT_DIR)/idl/salome

EXTRA_DIST = $(IDL_FILES)

install-exec-local: install-pyidl

uninstall-local:
	rm -rf $(DESTDIR)$(salomepythondir)/yacsgui_idl.py
	rm -rf $(DESTDIR)$(salomepythondir)/YACS_ORB
	rm -rf $(DESTDIR)$(salomepythondir)/YACS_ORB__POA

install-pyidl: $(IDL_FILES)
	$(INSTALL) -d  $(DESTDIR)$(salomepythondir)
	@for file in $^ dummy; do \
          if [ $$file != "dummy" ]; then \
             $(OMNIORB_IDL) -bpython $(IDLPYFLAGS) -C$(DESTDIR)$(salomepythondir) $$file ; \
          fi ; \
        done ;

dist-hook:
	rm -f $(distdir)/yacsguiSK.cc
	rm -f $(distdir)/yacsgui.hh

clean-local:
	rm -rf YACS_ORB  YACS_ORB__POA yacsgui.hh  yacsgui_idl.py yacsguiSK.cc

include $(top_srcdir)/adm/unix/make_end.am
