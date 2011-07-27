#  Copyright (C) 2006-2011  CEA/DEN, EDF R&D
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

# Libraries targets
lib_LTLIBRARIES = libSalomeWrap.la

dist_libSalomeWrap_la_SOURCES = \
	SalomeWrapExport.hxx \
	SalomeWrap_Module.hxx \
	SalomeWrap_Module.cxx \
	SalomeWrap_DataModel.hxx \
	SalomeWrap_DataModel.cxx \
	SuitWrapper.hxx \
	SuitWrapper.cxx \
	WrapGraphicsView.hxx \
	WrapGraphicsView.cxx

# MOC pre-processing
MOC_FILES = \
	SalomeWrap_Module_moc.cxx \
	WrapGraphicsView_moc.cxx

nodist_libSalomeWrap_la_SOURCES = $(MOC_FILES)

libSalomeWrap_la_CXXFLAGS = \
	$(THREAD_DEF) \
	$(PYTHON_CPPFLAGS) \
	$(OMNIORB_CXXFLAGS) $(OMNIORB_INCLUDES) \
	$(BOOST_CPPFLAGS) \
	-I$(KERNEL_ROOT_DIR)/include/salome \
	-I$(GUI_ROOT_DIR)/include/salome \
	-I$(srcdir)/../bases \
	$(qt4_cppflags)

libSalomeWrap_la_LDFLAGS  = \
	$(KERNEL_LDFLAGS) \
	-L$(GUI_ROOT_DIR)/lib/salome \
	 $(qt4_ldflags)

libSalomeWrap_la_LIBADD  = \
	$(qt4_libs) $(QT_LIBS) $(OMNIORB_LIBS) \
	../engine/libYACSlibEngine.la \
	-lSalomeIDLKernel \
	-lCAM -lsuit -lLightApp -lqtx -lQxScene -lObjBrowser -lSalomeApp

# resources files


# --------------------------------------------
# *.h --> *_moc.cxx
# --------------------------------------------

SUFFIXES = .hxx _moc.cxx .qrc _qrc.cxx

.hxx_moc.cxx :
	$(QT_MOC) -p . -o $@ $<


clean-local-qt :
	rm -f *_moc.cxx *_qrc.cxx


clean-local: clean-local-qt
