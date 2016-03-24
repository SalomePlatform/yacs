# Copyright (C) 2006-2016  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
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

include $(top_srcdir)/adm_local/unix/make_common_starter.am

if SALOME_KERNEL
   SUBDIRS=idl
   DIST_SUBDIRS=idl
else
   SUBDIRS=
   DIST_SUBDIRS=
endif

SUBDIRS += adm_local src doc bin resources
DIST_SUBDIRS += adm_local src doc bin resources

DISTCLEANFILES = a.out

install-exec-local:
	cp ${srcdir}/env_HXX2SALOME_GENERIC_CLASS_NAME.sh ${prefix}

dist-hook:
	rm -rf `find $(distdir) -name CVS`

usr_docs:
	(cd doc && $(MAKE) $(AM_MAKEFLAGS) usr_docs)

docs:usr_docs

dev_docs:
	(cd doc && $(MAKE) $(AM_MAKEFLAGS) dev_docs)

