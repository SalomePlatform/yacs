#  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
include $(top_srcdir)/adm/unix/make_begin.am

IDL_FILES = yacsgui.idl
IDL_SRC = yacsguiSK.cc
BUILT_SOURCES = $(IDL_SRC) yacsgui.hh

IDLCXXFLAGS = -I$(KERNEL_ROOT_DIR)/idl/salome -I$(GUI_ROOT_DIR)/idl/salome

IDLPYFLAGS  = -I$(KERNEL_ROOT_DIR)/idl/salome

EXTRA_DIST = $(IDL_FILES)

lib_LTLIBRARIES = libSalomeIDLYACS.la

libSalomeIDLYACS_la_SOURCES = $(IDL_SRC)

libSalomeIDLYACS_la_LIBADD = \
	$(OMNIORB_LIBS)

libSalomeIDLYACS_la_CXXFLAGS = \
	$(OMNIORB_INCLUDES) \
	-I$(KERNEL_ROOT_DIR)/include/salome \
	$(OMNIORB_CXXFLAGS)

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
