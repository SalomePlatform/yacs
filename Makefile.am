# Copyright (C) 2006-2012  CEA/DEN, EDF R&D
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

ACLOCAL_AMFLAGS = -I adm/unix/config_files

if SALOME_KERNEL
  SUBDIRS = idl src doc Demo
else
  SUBDIRS = src doc Demo
endif

check-local:
	cat /tmp/${USER}/UnitTestsResult

dist-hook:
	rm -rf `find $(distdir) -name CVS`

dev_docs:
	(cd doc && $(MAKE) $(AM_MAKEFLAGS) dev_docs)

EXTRA_DIST = Misc \
             build_cmake \
	     build_cmake.bat \
	     build_configure \
	     clean_configure \
	     configure.in.base \
	     rfind

