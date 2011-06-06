# Copyright (C) 2006-2011  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

include $(top_srcdir)/adm/unix/make_begin.am

lib_LTLIBRARIES = libHMI.la

# Implementation files
LIBSOURCES = \
	commands.cxx \
	commandsProc.cxx \
	guiObservers.cxx \
	guiContext.cxx

# Headers
LIBHEADERS = 



libHMI_la_SOURCES = $(LIBSOURCES) $(LIBHEADERS)
libHMI_la_LDFLAGS =

libHMI_la_LIBADD = \
	../runtime/libYACSRuntimeSALOME.la \
	../yacsloader/libYACSloader.la \
	$(EXPAT_LIBS) $(LIBXML_LIBS)

libHMI_la_CXXFLAGS = \
	$(THREAD_DEF) \
	$(PYTHON_CPPFLAGS) \
	$(OMNIORB_CXXFLAGS) $(OMNIORB_INCLUDES) \
	$(LIBXML_INCLUDES) \
	-I$(KERNEL_ROOT_DIR)/include/salome \
	-I$(srcdir)/../bases \
	-I$(srcdir)/../engine \
	-I$(srcdir)/../runtime \
	-I$(srcdir)/../yacsloader \
	-I../../idl \
	-I../yacsorb 

EXTRA_DIST = \
HMIExport.hxx \
commands.hxx      commandsProc.hxx \
guiContext.hxx  guiObservers.hxx  


include $(top_srcdir)/adm/unix/make_end.am
