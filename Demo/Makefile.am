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

IDL_FILES = echo.idl
BUILT_SOURCES = echoSK.cc schema.xml xmlrpcprog.py
bin_PROGRAMS=echoSrvDemo 
echoSrvDemo_SOURCES = echoSrv.cxx 
nodist_echoSrvDemo_SOURCES = echoSK.cc
echoSrvDemo_CXXFLAGS = -I. $(OMNIORB_INCLUDES) $(OMNIORB_CXXFLAGS) 
echoSrvDemo_LDFLAGS = $(OMNIORB_LIBS)
AM_CXXFLAGS = $(THREAD_DEF)

EXTRA_DIST = echo.idl schema_orig.xml xmlrpcprog_orig.py

schema.xml:schema_orig.xml
	cp $(srcdir)/schema_orig.xml schema.xml
xmlrpcprog.py:xmlrpcprog_orig.py
	cp $(srcdir)/xmlrpcprog_orig.py xmlrpcprog.py

CLEANFILES = *.hh *SK.cc schema.xml xmlrpcprog.py

include $(top_srcdir)/adm/unix/make_end.am
