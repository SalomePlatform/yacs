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

# header files 
salomeinclude_HEADERS = \
	Yacsgui.hxx

# Libraries targets
lib_LTLIBRARIES = libYACS.la

dist_libYACS_la_SOURCES = \
	Yacsgui.cxx \
	Yacsgui_DataModel.cxx

# MOC pre-processing
MOC_FILES = \
	Yacsgui_moc.cxx

nodist_libYACS_la_SOURCES = $(MOC_FILES)

libYACS_la_CXXFLAGS = \
	$(THREAD_DEF) \
	$(PYTHON_CPPFLAGS) \
	$(OMNIORB_INCLUDES) \
	$(LIBXML_INCLUDES) \
	$(BOOST_CPPFLAGS) \
	-I$(KERNEL_ROOT_DIR)/include/salome \
	-I$(GUI_ROOT_DIR)/include/salome \
	-I$(srcdir)/../bases \
	-I$(srcdir)/../engine \
	-I$(srcdir)/../runtime \
	-I$(srcdir)/../yacsloader \
	-I$(srcdir)/../hmi \
	-I$(srcdir)/../salomewrap \
	-I$(srcdir)/../genericgui \
	$(qt4_cppflags) \
	$(qsci4_cppflags) \
	@GRAPHVIZ_CPPFLAGS@

libYACS_la_LDFLAGS  = \
	 @GRAPHVIZ_LDFLAGS@ \
	 -L$(GUI_ROOT_DIR)/lib/salome -L$(KERNEL_ROOT_DIR)/lib/salome \
	 $(qt4_ldflags) $(qsci4_ldflags)

libYACS_la_LIBADD  = \
	../genericgui/libGenericGui.la \
	$(qt4_libs) $(qsci4_libs) \
	-lSalomeApp

# resources files

LIBICONS =  \
	SalomeApp.xml \
	YACSCatalog.xml \
	ModuleYacs.png \
	ExecYACS.png

QMFILES =

# \
#	YACS_msg_en.qm \
#	YACS_msg_fr.qm \
#	YACS_icons.qm

dist_salomeres_DATA = $(QMFILES) ${ICONS}
ICONS = $(LIBICONS:%=resources/%)


ui_%.h: %.ui
	$(QT_UIC) $< -o $@

# --------------------------------------------
# *.h --> *_moc.cxx
# --------------------------------------------

SUFFIXES = .hxx _moc.cxx .qrc _qrc.cxx

.hxx_moc.cxx :
	$(QT_MOC) -p . -o $@ $<

.qrc_qrc.cxx :
	$(QT_RCC) -name $(*F)  $< -o $@

#.ui.h :
#	$(QT_UIC) -o $@ $<


clean-local-qt :
	rm -f *_moc.cxx *_qrc.cxx


clean-local: clean-local-qt